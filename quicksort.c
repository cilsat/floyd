#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#include <omp.h>

#define DEBUG 0
#define ARR_MAX 1000

int *random_array(long int n) {
    int *a = (int *)malloc(n*sizeof(int));
    for (long int i = 0; i < n; i++)
        a[i] = ((int) rand())%ARR_MAX;
    return a;
}

void print_array(int *a, long int start, long int stop) {
    for (long int i = start; i < stop; i++)
        printf("%d ", a[i]);
    printf("\n");
}

static inline void swap(int *a, long int s, long int i) {
    int temp = a[s];
    a[s] = a[i];
    a[i] = temp;
}

long int partition(int *a, long int p, long int r) {
    int *lt = (int *) malloc((r-p)*sizeof(int));
    int *gt = (int *) malloc((r-p)*sizeof(int));
    int idx = a[r];
    long int i, j;
    long int lt_n = 0;
    long int gt_n = 0;

#pragma omp parallel for
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

void quicksort_seq(int *a, long int lo, long int hi) {
    long int i, div;
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

void quicksort_par(int *a, long int lo, long int hi) {
    long int div;
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

int main(int argc, char** argv) {
    long int n = strtol(argv[1], NULL, 10);
    int *a, *b;
    double dstart, dstop;

    a = random_array(n);
    b = random_array(n);
    memcpy(b, a, sizeof(*a));

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

    printf("%.5f,", dstop-dstart);

    if (DEBUG) {
        printf("initial array:\n");
        print_array(b, 0, n);
    }

    dstart = omp_get_wtime();
    quicksort_par(b, 0, n-1);
    dstop = omp_get_wtime();

    if (DEBUG) {
        printf("\nsorted array:\n");
        print_array(b, 0, n);
        printf("\n");
    }

    printf("%.5f\n", dstop-dstart);
    free(a);
    free(b);

    return 0;
}
