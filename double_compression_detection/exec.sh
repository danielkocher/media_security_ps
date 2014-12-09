#!/bin/bash

sh cleanup.sh

#make
make

#execute
./main ../pictures/mandrill_4.2.03.jxr mandrill_4.2.03.tif

#histograms
for file in ./hist*.dat ; do
	fn_in=$(basename "$file")

	fn_out=$(basename "$file")
	fn_out="${fn_out%.*}.png"
	
	gnuplot -e "fn_in='$fn_in'; fn_out='$fn_out'" hist.gnu
done