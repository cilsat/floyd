#include <stdio.h>
#include <stdlib.h>
#include <time.h>
//#include <omp.h>

#define DEBUG 1
#define ARR_MAX 100

void random_array(long n, int a[n]) {
    for (long i = 0; i < n; i++)
        a[i] = ((int) rand())%ARR_MAX;
}

void print_array(long n, int a[n]) {
    for (long i = 0; i < n; i++)
        printf("%d ", a[i]);
    printf("\n");
}

static inline void swap(int a[], long s, long i) {
    int temp = a[s];
    a[s] = a[i];
    a[i] = temp;
}

void quicksort_seq(int a[], long lo, long hi) {
    if (lo < hi) {
        int x = a[lo];
        long s = lo;
        for (long i = lo+1; i < hi; i++) {
            if (a[i] <= x) {
                s++;
                swap(a, s, i);
            }
        }
        swap(a, lo, s);
        quicksort_seq(a, lo, s);
        quicksort_seq(a, s+1, hi);
    }
}

void quicksort_par(int a[], long lo, long hi) {
}

int main(int argc, char** argv) {
    long n = strtol(argv[1], NULL, 10);
    int a[n];
    clock_t tstart, tstop;

    //printf("%s\n", argv[1]);
    //printf("%ld\n", n);

    random_array(n, a);
    printf("initial array:\n");
    //print_array(n, a);

    tstart = clock();
    quicksort_seq(a, 0, n);
    tstop = clock();

    printf("\nsorted array:\n");
    //print_array(n, a);
    printf("time elapsed: %.5f s\n",
            (tstop-tstart)/(double)CLOCKS_PER_SEC);

    return 0;
}
