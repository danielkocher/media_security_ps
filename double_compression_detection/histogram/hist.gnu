if(!exists("fn_in")) fn_in='hist.dat'
if(!exists("fn_out")) fn_out='hist.png'
reset
unset key; set xtics nomirror; set ytics nomirror; set border front;
div=1.1; bw = 0.9; h=1.0; BW=0.9; wd=10; LIMIT=255-wd; white = 0
set auto x
set auto y
set xlabel ' '
set style data histogram
set style histogram cluster gap 1
set style fill solid
set boxwidth bw
plot fn_in u 2, '' u 0:(0):xticlabel(1) w l title ''

set term png
set output fn_out
replot
set term x11