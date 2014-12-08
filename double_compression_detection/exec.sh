#!/bin/bash

#clean up
rm -i coeffs.csv hist*.dat

#make
make

#execute
./main ../pictures/lena512.jxr lena512.tif

#histograms
for file in ./hist*.dat ; do
	fn_in=$(basename "$file")

	fn_out=$(basename "$file")
	fn_out="${fn_out%.*}.png"
	
	gnuplot -e "fn_in='$fn_in'; fn_out='$fn_out'" hist.gnu
done