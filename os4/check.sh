#!/usr/bin/env bash

declare -a arr=("printf" "open" "close" "read" "write" "fork" "signal") # 声明变量为数组类型

# 创建结果文件夹
if [ ! -d "c_results" ]; then
    mkdir -p c_results
fi

function getNum() {
    local ret=0
    # 第一种方法 使用grep自带的参数
    let "ret=$(grep -c "[^0-9a-zA-Z_]$1\s*(" $2)"
    # 或者
    # let "ret=`grep -c "[^0-9a-zA-Z_]$1\s*(" $2`"
    # 第二种方法
    # let "ret=`grep -o "[^0-9a-zA-Z_]$1\s*(" $2  | wc -l`"
    return $ret
}

function main() {
    for var in ${arr[@]}; do
        local sum=0
        for file in $(ls $1/*.c); do
            echo "$file 文件" >>$var"_results"

            getNum $var $file
            let "temp=$?"
            echo "$temp" >>"${var}_results"
            let "sum+=temp"
        done
        echo "总出现次数：$sum" >>$var"_results"
        echo "$var总出现次数: $sum"
        mv $var"_results" c_results #转移到c_result目录下
        echo "$var: $sum" >>"final_results"
    done
    mv "final_results" c_results
}

main $1
