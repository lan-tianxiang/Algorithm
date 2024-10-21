# 路径设定
root_path=$(cd `dirname $0`; pwd)/..
outname=$root_path
out_path=$root_path
code_path=$root_path
build_path=$root_path
tool_gn_path=$root_path
tool_ninja_path=$root_path
tool_objdump_path=$root_path

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

move_build_file_func() {
    cd $build_path
    local temp_path=$build_path/out

    mkdir -p $temp_path
    mkdir -p $out_path

    mv $outname             $temp_path/
    mv $outname.lst         $temp_path/
    mv $outname.map         $temp_path/
    mv obj                  $temp_path/
    mv .ninja_deps          $temp_path/
    mv .ninja_log           $temp_path/
    mv args.gn              $temp_path/
    mv build.ninja          $temp_path/
    mv build.ninja.d        $temp_path/
    mv build.ninja.stamp    $temp_path/
    mv toolchain.ninja      $temp_path/

    cp -rf $temp_path $out_path
    rm -rf $temp_path
}

build_func() {
    local start_time_in_ms=$(($(date +%s%N)/1000000))
    # 生成编译文件
    $tool_gn_path gen $build_path
    $tool_ninja_path -C $build_path -j 32
    $tool_objdump_path -d -s -x -t -r -D -S -g -h -C -f -p -l $outname > $outname.lst
    move_build_file_func
    echo_with_color green "Build success! spend : $(($(($(date +%s%N)/1000000)) - $start_time_in_ms)) ms"
}

run_func() {
    # 运行
    cd $out_path
    ./$outname
}

clean_func() {
    # 清理
    rm -rf $out_path
}

init_vars_func() {
# 输出子名称
    outname="myapp"

# 初始化变量
    build_path=$root_path
    code_path=$root_path/src
    out_path=$root_path/build/out

    tool_gn_path=gn
    tool_ninja_path=ninja
    tool_objdump_path=objdump
}

main_func() {
    echo_current_time
    init_vars_func
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
            echo_with_color red "Usage: $0 {build|run|clean}"
            build_func
            run_func
            clean_func
            exit 1
            ;;
    esac
}

main_func $1