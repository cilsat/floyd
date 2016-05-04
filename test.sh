for n in `python -c 'print(range(10**4, 10**6, 10**4))' | sed 's/,//g' | sed 's/\[//g' | sed 's/\]//g'`;
do
    echo -n "$n," | tee -a xeon2.txt
    ./quicksort $n | tee -a xeon2.txt
done
