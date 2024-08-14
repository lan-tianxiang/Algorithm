# 路径设定
root_path=$(cd `dirname $0`; pwd)/..
build_path=$root_path/build
code_path=$root_path/src
out_path=$root_path/build/out

tool_gn_path=gn
tool_ninja_path=ninja
tool_objdump_path=objdump

# 输出子名称
outname="myapp"

echo_with_color() {
    case $1 in
        green)
            echo -e "\033[32m$2\033[0m"
            ;;
        red)
            echo -e "\033[31m$2\033[0m"
            ;;
        blue)
            echo -e "\033[34m$2\033[0m"
            ;;
        purple)
            echo -e "\033[35m$2\033[0m"
            ;;
        yellow)
            echo -e "\033[33m$2\033[0m"
            ;;
        *)
            echo $2
            ;;
    esac
}

echo_current_time() {
    echo_with_color yellow "Current time: $(date "+%Y-%m-%d %H:%M:%S")"
}

build_func() {
    local start_time_in_ms=$(($(date +%s%N)/1000000))
    # 生成编译文件
    cd $code_path
    $tool_gn_path gen $out_path/$outname
    $tool_ninja_path -C $out_path/$outname -j 32
    $tool_objdump_path -d -s -x -t -r -D -S -g -h -C -f -p $out_path/$outname/$outname > $out_path/$outname/$outname.lst
    echo_with_color green "Build success! spend : $(($(($(date +%s%N)/1000000)) - $start_time_in_ms)) ms"
}

run_func() {
    # 运行
    cd $out_path/$outname
    ./$outname
}

clean_func() {
    # 清理
    rm -rf $out_path
}

echo_current_time
case $1 in
    build)
        build_func
        ;;
    run)
        run_func
        ;;
    clean)
        clean_func
        ;;
    *)
        echo "Usage: $0 {build|run|clean}"
        clean_func
        exit 1
        ;;
esac