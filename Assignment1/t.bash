for N in {1..5}
do
    ./c 127.0.0.1 9000 
    cat <<< ""  &
done
wait