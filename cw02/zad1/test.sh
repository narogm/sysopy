#! /bin/bash

sizes=(1 4 512 1024 8192)
amounts=(100 500)

for amount in ${amounts[*]}
do
	#echo "amount:  $amount"
	for size in ${sizes[*]}
	do
		make clean
		make
		./main generate dane $amount $size
		./main copy dane dane_sys $amount $size sys
		./main copy dane dane_lib $amount $size lib
		./main sort dane_sys $amount $size sys
		./main sort dane_lib $amount $size lib
	done
done
