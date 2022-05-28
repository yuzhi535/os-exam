#!/usr/bin/env bash

# 创建结果文件夹
if [ ! -d "c_results" ]; then
	mkdir -p c_results
fi

declare -a arr=("printf" "open" "close" "read" "write" "fork" "signal") # 声明变量为数组类型

function read_c_files() {
	for i in ${arr[@]}; do #遍历字符串
		local sum=0

		#找到该目录下所有的c文件
		for fp in $(
			find $1 -type f -name "*.c"
		); do

			echo $fp >>$i"_results"

			#统计字符串个数，使用正则表达式
			let "temp=`grep -o "[^0-9a-zA-Z_]$i\s*(" $fp | wc -l`" 
			# 第二种方法
			# let "temp=`grep -c "[^0-9a-zA-Z_]$i\s*(" $fp`"

			echo "$temp" >> "${i}_results"
			let "sum+=temp" #字符串个数求和
		done
		echo "总出现次数：$sum" >>$i"_results"
		echo "$i总出现次数: $sum"
		mv $i"_results" c_results #转移到c_result目录下
		echo "$i: $sum" >>"final_results"
	done

	mv "final_results" c_results
}

read_c_files $1
