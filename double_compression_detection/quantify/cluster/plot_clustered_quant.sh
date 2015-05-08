#!/bin/bash

for file in clustered_quantifications/*.dat
do
	fn_in=$(basename "$file")

	fn_out=$file
	fn_out="${fn_out%.*}.png"

	gnuplot -e "file_in='$file'; file_out='$fn_out'" plot_clustered_quant.gnu
done