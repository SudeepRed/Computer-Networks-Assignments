set terminal png
set output 'q2cwnd.png'
set ylabel 'Congestion Window'
set xlabel 'Time'
plot 'tcp-example.cwnd' using 1:2 with lines title "Congestion Window with Time"