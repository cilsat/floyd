/******************************************************************************
* Nama: Cil Hardianto Satriawan
* NIM: 23515053
* keterangan: 
*   compilation: gcc -Wall -std=c11 -fopenmp quicksort.c -o quicksort
*   execute: ./quicksort <n>
*   example: ./quicksort 1000000
*   execute test: ./test.sh <name>
*
*   For comparison with sequential version of algorithm and other info, set
*   DEBUG to 1
*******************************************************************************/

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#include <omp.h>

#define DEBUG 1
#define ARR_MAX 100000

// generate random list of numbers for testing
int *random_array(long n) {
    int *a = (int *)malloc(n*sizeof(int));
    for (long i = 0; i < n; i++)
        a[i] = ((int) rand())%ARR_MAX;
    return a;
}

// output list to stdout
void print_array(int *a, long start, long stop) {
    for (long i = start; i < stop; i++)
        printf("%d ", a[i]);
    printf("\n");
}

// swap the values contained in two (different) list indexes
static inline void swap(int *a, long s, long i) {
    int temp = a[s];
    a[s] = a[i];
    a[i] = temp;
}

// find pivot point of input list
long partition(int *a, long p, long r) {
    int *lt = (int *) malloc((r-p)*sizeof(int));
    int *gt = (int *) malloc((r-p)*sizeof(int));
    int idx = a[r];
    long i, j;
    long lt_n = 0;
    long gt_n = 0;

    for (i = p; i < r; i++) {
        if (a[i] < a[r]) {
            lt[lt_n++] = a[i];
        } else {
            gt[gt_n++] = a[i];
        }
    }

#pragma omp parallel for
    for (i = 0; i < lt_n; i++) {
        a[p+i] = lt[i];
    }

    a[p+lt_n] = idx;

#pragma omp parallel for
    for (j = 0; j < gt_n; j++) {
        a[p+lt_n+j+1] = gt[j];
    }

    free(lt);
    free(gt);

    return p+lt_n;
}

// embarrassingly parallel version of quicksort
void quicksort_par(int *a, long lo, long hi) {
    long div;
    if (lo < hi) {
        div = partition(a, lo, hi);

#pragma omp parallel sections
        {
#pragma omp section
            quicksort_par(a, lo, div-1);
#pragma omp section
            quicksort_par(a, div+1, hi);
        }
    }
}

// sequential version of quicksort for comparison
void quicksort_seq(int *a, long lo, long hi) {
    long i, div;
    if (lo < hi) {
        int x = a[lo];
        div = lo;
        for (i = lo+1; i < hi; i++) {
            if (a[i] <= x) {
                div++;
                swap(a, div, i);
            }
        }
        swap(a, lo, div);
        quicksort_seq(a, lo, div);
        quicksort_seq(a, div+1, hi);
    }
}

int main(int argc, char** argv) {
    long n = strtol(argv[1], NULL, 10);
    int *a, *b;
    double dstart, dstop;

    a = random_array(n);
    b = random_array(n);
    for (long i = 0; i < n; i++) {
        b[i] = a[i];
    }

    if (DEBUG) {
        printf("initial array:\n");
        print_array(a, 0, n);
    }

    dstart = omp_get_wtime();
    quicksort_seq(a, 0, n);
    dstop = omp_get_wtime();

    if (DEBUG) {
        printf("\nsorted array:\n");
        print_array(a, 0, n);
        printf("\n");
    }

    //printf("%.5f,", dstop-dstart);

    if (DEBUG) {
        printf("initial array:\n");
        print_array(b, 0, n);
    }

    // start off with 2 threads for upper and lower partition of list
    omp_set_num_threads(2);
    // set to 1 to ensure that threads are created for subsequent recursion
    // depths.
    omp_set_nested(1);

    dstart = omp_get_wtime();
    quicksort_par(b, 0, n-1);
    dstop = omp_get_wtime();

    if (DEBUG) {
        printf("\nsorted array:\n");
        print_array(b, 0, n);
        printf("\n");
    }

    //printf("%.5f\n", dstop-dstart);
    free(a);
    free(b);

    return 0;
}

