#include <stdio.h>
#include <stdlib.h>
//#include <omp.h>

#define DEBUG 1
#define ARR_MAX 100

void random_array(int n, int a[n]) {
    for (int i = 0; i < n; i++)
        a[i] = ((int) rand())%ARR_MAX;
}

void print_array(int n, int a[n]) {
    for (int i = 0; i < n; i++)
        printf("%d ", a[i]);
    printf("\n");
}

static inline void swap(int a[], int s, int i) {
    int temp = a[s];
    a[s] = a[i];
    a[i] = temp;
}

void quicksort_seq(int a[], int lo, int hi) {
    if (lo < hi) {
        int x = a[lo];
        int s = lo;
        for (int i = lo+1; i < hi; i++) {
            if (a[i] <= x) {
                s++;
                swap(a, s, i);
            }
        }
        swap(a, lo, hi);
        quicksort_seq(a, lo, s);
        quicksort_seq(a, s+1, hi);
    }
}

int main(int argc, char** argv) {

    int n = (int) strtol(argv[1], NULL, 10);
    int a[n];
    random_array(n, a);
    print_array(n, a);

    quicksort_seq(a, 0, n);
    print_array(n, a);

    return 0;
}
