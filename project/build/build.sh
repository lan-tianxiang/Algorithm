#!/bin/bash

# 路径初始化
init_vars() {
    ROOT_PATH=$(dirname "$(cd "$(dirname "$0")"; pwd)")
    CODE_PATH="$ROOT_PATH/src"
    OUT_PATH="$ROOT_PATH/out"
    OUT_FILE_PATH="$OUT_PATH/audio_app"
    TOOL_PATH="$ROOT_PATH/tools"
    TOOL_GN_PATH="$TOOL_PATH/gn/gn"
    TOOL_NINJA_PATH="$TOOL_PATH/ninja/ninja"
    TOOL_OBJDUMP_PATH="objdump"
}


# 彩色输出
echo_with_color() {
    local color=$1
    local msg=$2
    case $color in
        green) echo -e "\033[32m${msg}\033[0m" ;;
        red) echo -e "\033[31m${msg}\033[0m" ;;
        blue) echo -e "\033[34m${msg}\033[0m" ;;
        purple) echo -e "\033[35m${msg}\033[0m" ;;
        yellow) echo -e "\033[33m${msg}\033[0m" ;;
        *) echo "${msg}" ;;
    esac
}


# 当前时间输出
echo_current_time() {
    echo_with_color yellow "Current time: $(date "+%Y-%m-%d %H:%M:%S")"
}


# 工具检查
check_tools() {
    for tool in "$TOOL_GN_PATH" "$TOOL_NINJA_PATH"; do
        if [ ! -x "$tool" ]; then
            echo_with_color red "工具未找到或不可执行: $tool"
            exit 2
        fi
    done
}

# 编译模块
build() {
    check_tools
    local start_time_in_ms=$(($(date +%s%N)/1000000))
    "$TOOL_GN_PATH" gen "$OUT_PATH" --args=""
    "$TOOL_NINJA_PATH" -C "$OUT_PATH" -j 32
    "$TOOL_OBJDUMP_PATH" --source --all-headers --line-numbers --demangle --file-headers --disassemble --reloc --syms --dynamic-syms --section-headers --wide "$OUT_FILE_PATH" > "$OUT_FILE_PATH.lst"
    local spend_ms=$(($(date +%s%N)/1000000 - $start_time_in_ms))
    echo_with_color green "Build success! spend : ${spend_ms} ms"
}


# 运行模块
run() {
    if [ ! -x "$OUT_FILE_PATH" ]; then
        echo_with_color red "可执行文件不存在: $OUT_FILE_PATH"
        exit 3
    fi
    "$OUT_FILE_PATH"
}


# 清理模块
clean() {
    rm -rf "$OUT_PATH"
    mkdir -p "$OUT_PATH"
    echo_with_color blue "Cleaned: $OUT_PATH"
}


# 主入口
main() {
    echo_current_time
    init_vars
    case "$1" in
        build)
            build
            ;;
        run)
            run
            ;;
        clean)
            clean
            ;;
        *)
            echo_with_color yellow "用法: $0 {build|run|clean}"
            exit 1
            ;;
    esac
}

main "$1"