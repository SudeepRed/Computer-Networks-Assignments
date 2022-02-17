set terminal png
set output 'q1Qdelay.png'
set ylabel 'Queueing Delay'
set xlabel 'Time'
plot 'q.txt' using 1:2 with lines  title "Queueing  delay with Time"