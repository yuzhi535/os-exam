#!/usr/bin/env bash

declare -a arr=("printf" "open" "close" "read" "write" "fork" "signal") # 声明变量为数组类型

for var in ${arr[@]}; do
    echo "$var 的情况"
    for file in $(ls $1/*.c); do
        echo "$file 文件"
        # 第一种方法 使用grep自带的参数
        grep -n --color=auto "[^a-zA-Z0-9_]${var}\s*(" $file
        # 第二种方法 使用管道通信
        # cat $file | grep -n --color=auto "[^a-zA-Z0-9_]${var}\s*("
    done
done