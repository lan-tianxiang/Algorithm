HD Audio Effect Chain — v2 (RCU-style)
===================================

简介
----
这是仓库中针对实时音频路径设计的 v2 音效链实现（RCU-style）。主要目标：

- 音频线程读取路径无锁、低延迟；
- 控制路径可以安全地发布配置与参数；
- 插件以共享库（.so）形式提供，并以版本化 ABI 保证兼容性。

核心概念（速览）
-----------------
- RCU-style 快照：链管理器通过原子指针发布快照，快照持有原子引用计数；音频线程读取快照并在使用后 release；发布者在 refcount 归零时回收旧快照。
- 实时安全接口：插件必须提供实时安全的 `process()` 回调（不得 malloc/free/阻塞/持锁等）。
- 控制通道：对小参数，可使用原子 mailbox（atomic_exchange）；对复杂更新，应在控制线程构建新的 rt_work 或快照并原子 publish。
- ABI 版本化：宏 `HD_EFFECT_ABI_V2`（见头文件），加载器 (`plugin_loader_v2`) 在加载时会验证插件的 api_version。

目录与文件（关键）
-------------------
- `hd_audio_effect_v2.h/.c` — v2 ABI 与链管理实现
- `plugin_loader_v2.c/.h` — v2 插件加载器（dlopen + create/release）
- `effect_module/` — 插件示例（如 `audio_gain/hd_audio_gain_v2.c`）
- `examples/` — 示例与基准：
  - `bench_effect_v2.c` — micro-benchmark
  - `test_effect_v2.c` — 简单功能测试
- `run_bench.sh` — 一键构建并运行脚本（支持 bench/test/all）

快速开始（构建与运行）
------------------------
在仓库根目录下运行（或直接进入 `src/audio/audio_effect/`）：

构建并运行基准（示例）

    # 在仓库根目录运行脚本：
    bash src/audio/audio_effect/run_bench.sh bench [effects] [iterations] [frames] [cpu_bind]

示例：

    bash src/audio/audio_effect/run_bench.sh bench 10 20000 480 0

脚本会：
- 编译插件（生成 `libhd_audio_gain_v2.so` 到 `src/audio/audio_effect/`）
- 编译 core 与 loader（临时对象放 /tmp）
- 编译 examples 并在 `src/audio/audio_effect/examples/` 下运行目标

直接手动构建（如果你不使用脚本）

    gcc -std=c11 -I./src/audio/audio_effect -fPIC -shared -O2 -o libhd_audio_gain_v2.so src/audio/audio_effect/effect_module/audio_gain/hd_audio_gain_v2.c
    gcc -std=c11 -I./src/audio/audio_effect -fPIC -c -o /tmp/hd_audio_effect_v2.o src/audio/audio_effect/hd_audio_effect_v2.c -pthread
    gcc -std=c11 -I./src/audio/audio_effect -fPIC -c -o /tmp/plugin_loader_v2.o src/audio/audio_effect/plugin_loader_v2.c -ldl
    gcc -std=c11 -I./src/audio/audio_effect -o src/audio/audio_effect/examples/bench_effect_v2 src/audio/audio_effect/examples/bench_effect_v2.c /tmp/plugin_loader_v2.o /tmp/hd_audio_effect_v2.o -ldl -lpthread -O2

如何运行 micro-benchmark
------------------------

    cd src/audio/audio_effect/examples
    LD_LIBRARY_PATH=..:.. ./bench_effect_v2 <num_effects> <iterations>

基准与性能注意事项
--------------------
- 使用 -O2/-O3（release）构建以获得真实性能；可试 `-march=native` 在你的 CPU 上启用指令集优化。
- 在单独的、空闲的 CPU 上运行（使用 `taskset` 或在脚本里用 `sched_setaffinity`）以减少测量噪声。
- 增加 frames（如 480）与 iterations（>=20000）以降低抖动。
- 多次运行并统计 p50/p90/p99；用 perf/profiler 分析热点。
- 用 volatile checksum 或避免被优化掉的工作以防短路优化影响测量。

实时安全与插件开发要点
-----------------------
- process() 必须为实时安全：不要进行内存分配、系统阻塞或任何会导致优先级反转的操作。
- 小参数更新：使用原子 mailbox（示例 `hd_audio_gain_v2` 使用 atomic_exchange）或预分配 rt_work。
- 复杂更新：在控制线程构建新快照并通过链管理器原子发布，音频线程通过 snapshot 引用计数安全读取。
- 导出接口（插件）：
  - `effect_create_v2(effect_handle_v2_t** out, const effect_descriptor_v2_t* desc)`
  - `effect_release_v2(effect_handle_v2_t* h)`
  - 在 vtable 中提供 `process()`（必需）、`command()` 和（可选的）`create()`/`release()` 等

如何贡献、扩展与后续改进
--------------------------------
- 把构建步骤写入 Makefile / BUILD.gn 以方便 CI 集成。
- 扩展基准：自动化多次测量并输出 p50/p90/p99；加入 cache-warmup、热身周期以及更真实的信号流。
- 增加更多示例插件（EQ、reverb、滤波器）和自动化单元测试（libcheck 或自建测试框架）。

许可与联系
--------------
参见仓库根 `README.txt`。如需讨论设计或性能，请在 issue 中开启话题。

