#!/usr/bin/env bash
set -euo pipefail

# run_bench.sh - build plugin and examples, then run bench or test
# Usage: ./run_bench.sh [bench|test|all] [effects] [iterations] [frames] [cpu_bind]
# Example: ./run_bench.sh bench 10 20000 480 0
# build plugin, build examples, run bench with defaults
#bash src/audio/audio_effect/run_bench.sh bench
# 自定义参数： bench <effects> <iterations> <frames> <cpu_bind>
#bash src/audio/audio_effect/run_bench.sh bench 10 20000 480 0
# 运行仅测试（不做基准）
#bash src/audio/audio_effect/run_bench.sh test
# 先 test 再 bench
#bash src/audio/audio_effect/run_bench.sh all 10 20000 480 0

SCRIPTDIR="$(cd "$(dirname "$0")" && pwd)"
# repository root (three levels up from src/audio/audio_effect)
REPO_ROOT="$(cd "$SCRIPTDIR/../../.." && pwd)"
TOPDIR="$SCRIPTDIR"
PLUGIN_SRC="$REPO_ROOT/src/audio/audio_effect/effect_module/audio_gain/hd_audio_gain_v2.c"
PLUGIN_SO="$TOPDIR/libhd_audio_gain_v2.so"
EX_DIR="$TOPDIR/examples"

MODE=${1:-bench}
EFFECTS=${2:-10}
ITER=${3:-20000}
FRAMES=${4:-480}
CPU_BIND=${5:-0}

echo "mode=$MODE effects=$EFFECTS iter=$ITER frames=$FRAMES cpu_bind=$CPU_BIND"

build_plugin() {
    echo "building plugin..."
    gcc -std=c11 -I."$TOPDIR" -I."$REPO_ROOT/src/audio/audio_effect" -fPIC -shared -O2 -o $PLUGIN_SO $PLUGIN_SRC
    # copy plugin into examples dir so examples can dlopen ./libhd_audio_gain_v2.so
    mkdir -p "$EX_DIR"
    cp -f "$PLUGIN_SO" "$EX_DIR/"
}

build_examples() {
    echo "building examples..."
    # compile core + loader
    gcc -std=c11 -I."$TOPDIR" -fPIC -c -o /tmp/hd_audio_effect_v2.o $TOPDIR/hd_audio_effect_v2.c -pthread
    gcc -std=c11 -I."$TOPDIR" -fPIC -c -o /tmp/plugin_loader_v2.o $TOPDIR/plugin_loader_v2.c -ldl
    # compile bench/test
    gcc -std=c11 -I."$TOPDIR" -o $EX_DIR/bench_effect_v2 $EX_DIR/bench_effect_v2.c /tmp/plugin_loader_v2.o /tmp/hd_audio_effect_v2.o -ldl -lpthread -O2
    gcc -std=c11 -I."$TOPDIR" -o $EX_DIR/test_effect_v2 $EX_DIR/test_effect_v2.c /tmp/plugin_loader_v2.o /tmp/hd_audio_effect_v2.o -ldl -lpthread -O2
}

run_bench() {
    pushd $EX_DIR > /dev/null
    export LD_LIBRARY_PATH="$TOPDIR:$REPO_ROOT${LD_LIBRARY_PATH:+:$LD_LIBRARY_PATH}"
    if [ "$CPU_BIND" -ge 0 ]; then
        # use taskset to bind current run to given cpu
        taskset -c $CPU_BIND $EX_DIR/bench_effect_v2 $EFFECTS $ITER
    else
        $EX_DIR/bench_effect_v2 $EFFECTS $ITER
    fi
    popd > /dev/null
}

run_test() {
    pushd $EX_DIR > /dev/null
    export LD_LIBRARY_PATH="$TOPDIR:$REPO_ROOT${LD_LIBRARY_PATH:+:$LD_LIBRARY_PATH}"
    $EX_DIR/test_effect_v2
    popd > /dev/null
}

main() {
    build_plugin
    build_examples
    case "$MODE" in
        bench)
            run_bench
            ;;
        test)
            run_test
            ;;
        all)
            run_test
            run_bench
            ;;
        *)
            echo "unknown mode: $MODE"; exit 2
            ;;
    esac
}

main
