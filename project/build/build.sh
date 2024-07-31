# 路径设定
root_path=$(cd `dirname $0`; pwd)/..
build_path=$root_path/build
code_path=$root_path/src
out_path=$root_path/build/out

# 输出子名称
outname="myapp"

# 清理
rm -rf $out_path

# 生成编译文件
cd $code_path
gn gen $out_path/$outname
ninja -C $out_path/$outname

# 运行
cd $out_path/$outname
./$outname