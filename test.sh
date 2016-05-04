for f in $1{0,1,2,3,4,5,6,7,8,9};
do
    for n in `python -c 'print(range(5*10**4, 1.5*10**6+5*10**4, 5*10**4))' | sed 's/,//g' | sed 's/\[//g' | sed 's/\]//g'`;
    do
        echo -n "$n," | tee -a $f
        ./quicksort $n | tee -a $f
    done
done
