set terminal png
set output 'q2Qdelay.png'
set ylabel 'Q'
set xlabel 'Time'
plot 'q.txt' using 1:2 with lines  title "Q delay with Time"