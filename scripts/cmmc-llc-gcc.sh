#!/bin/bash

# 获取传入的文件名参数
file_name=$1
executable_file_name=$2

# 检查文件名参数是否为空
if [ -z "$file_name" ]; then
  echo "请提供文件名作为参数。"
  exit 1
fi


# 检查文件是否存在
if [ ! -f "$file_name" ]; then
  echo "文件 $file_name 不存在。"
  exit 1
fi

# 提取文件名和扩展名
file_base="${file_name%.*}"
extension="${file_name##*.}"

echo $file_name
echo $file_base
echo $extension

# 如果文件名没有扩展名，则添加 .bc 扩展名
if [ "$extension" == "$file_name" ]; then
  file_base="$file_name"
  new_file_name="$file_name.bc"
else
  # 生成 bc 文件名
  new_file_name="$file_base.bc"
fi

# 调用 cmmc 生成 bc 文件
./cmmc "$file_name" 

# 检查 cmmc 是否成功生成 bc 文件
if [ ! -f "$new_file_name" ]; then
  echo "cmmc 未能成功生成 bc 文件。"
  exit 1
fi

if [ -z "$executable_file_name" ]; then
  executable_file_name="$file_base.out"
fi
# 生成可执行文件名


# 调用 clang 生成可执行文件
llc "$new_file_name" -o "$file_base.s"
gcc "$file_base.s" -o "$executable_file_name"

# 检查 clang 是否成功生成可执行文件
if [ ! -f "$executable_file_name" ]; then
  echo "gcc 未能成功生成可执行文件。"
  exit 1
fi

echo "成功生成可执行文件 $executable_file_name。"
