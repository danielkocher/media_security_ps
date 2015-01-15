#!/bin/bash

# echo on
set -x

#histograms
for dir in */ ; do
	for file in $dir/hist*.dat ; do
		echo $file
		fn_in=$(basename "$file")

		fn_out=$file
		fn_out="${fn_out%.*}.png"
		
		gnuplot -e "fn_in='$file'; fn_out='$fn_out'" hist.gnu
	done
done