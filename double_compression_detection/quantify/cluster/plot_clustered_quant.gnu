#gnuplot -e "file_in = \"clustered_quantifications/quant_all_c0_m0-1.dat\"; file_out = \"out.png\"" plot_quant.gnu

set auto x
set auto y
set key autotitle columnhead
#set key outside
#set key bottom

plot file_in using 1:2 with linespoints, file_in using 1:3 with linespoints, file_in using 1:4 with linespoints, file_in using 1:5 with linespoints, file_in using 1:6 with linespoints, file_in using 1:7 with linespoints, file_in using 1:8 with linespoints, file_in using 1:9 with linespoints, file_in using 1:10 with linespoints, file_in using 1:11 with linespoints

set term png
set output file_out
replot
set term x11