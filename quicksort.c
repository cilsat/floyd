#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <omp.h>

#define DEBUG 1
#define ARR_MAX 100
omp_set_num_threads(4);

void random_array(long n, int a[n]) {
    for (long i = 0; i < n; i++)
        a[i] = ((int) rand())%ARR_MAX;
}

void print_array(int a[], long start, long stop) {
    for (long i = start; i < stop; i++)
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
        print_array(a, lo, hi);
        quicksort_seq(a, lo, s);
        quicksort_seq(a, s+1, hi);
    }
}

void quicksort_par(int a[], int pivot, long lo, long hi) {
    #pragma omp parallel for
    if (lo < hi) {
        long s = lo;
        for (long i = lo+1; i < hi; i++) {
            if (a[i] <= pivot) {
                s++;
                swap(a, s, i);
            }
        }
        swap(a, lo, s);
        quicksort_seq(a, lo, s);
        quicksort_seq(a, s+1, hi);
    }
}

int main(int argc, char** argv) {
    long n = strtol(argv[1], NULL, 10);
    int a[n];
    clock_t tstart, tstop;

    random_array(n, a);
    if (DEBUG) {
        printf("initial array:\n");
        print_array(a, 0, n);
        printf("\n");
    }

    tstart = clock();
    quicksort_par(a, 0, n);
    tstop = clock();

    if (DEBUG) {
        printf("\nsorted array:\n");
        print_array(a, 0, n);
        printf("\n");
    }

    printf("time elapsed: %.5f s\n",
            (tstop-tstart)/(double)CLOCKS_PER_SEC);

    return 0;
}
