/*********************************************************************
* nama: Cil Hardianto Satriawan
* nim: 23515053
* keterangan: 
*   compilation: gcc -std=c11 -c matrix2d.h pqueue.h && gcc -std=c11 -fopenmp -o johnson matrix2d.c pqueue.c johnson.c
*   execute: ./johnson <#vertices>
*   example: mpirun -np 16 --hostfile ~/.mpi_hostfile ./floyd 1600
*
*   For comparison with sequential version of algorithm, set DEBUG to 1
********************************************************************/

#include <stdio.h>
#include <stdlib.h>
#include <omp.h>
#include "matrix2d.h"
#include "pqueue.h"

#define DEBUG -1
#define INF 9999

void johnson_seq(int **m, long m_size, int source) {
    heap_t *h = calloc(1, sizeof(heap_t));
    int dist[m_size];

    dist[source] = 0;
    push(h, 0, source);
    for (long i = 0; i < m_size; i++) {
        if (i != source) dist[i] = INF;
    }

    while (1) {
        int u = pop(h);
        if (u == -INF) break;
        for (long i = 0; i < m_size; i++) {
            int cur_dist = m[u][i];
            if (cur_dist != INF && cur_dist + dist[u] < dist[i]) {
                dist[i] = cur_dist + dist[u];
                if (!check_node(h, i)) push(h, dist[i], i);
            }
        }
    }
    if (DEBUG == 3) {
        for (long i = 0; i < m_size; i++) 
            printf("%d ", dist[i]);
        printf("\n");
        print_heap(h);
    }
    free(h);
}

void johnson_par(int **m, long m_size, int source) {
    heap_t *h = calloc(1, sizeof(heap_t));
    int dist[m_size];
    int cur_dist;
    long i;

    dist[source] = 0;
    push(h, 0, source);
    #pragma omp parallel for
    for (i = 0; i < m_size; i++) {
        if (i != source) dist[i] = INF;
    }

    while (1) {
        int u = pop(h);
        if (u == -INF) {
            break;
        }
        else {
            #pragma omp parallel for private(cur_dist)
            for (i = 0; i < m_size; i++) {
                cur_dist = m[u][i];
                if (cur_dist != INF && cur_dist + dist[u] < dist[i]) {
                    dist[i] = cur_dist + dist[u];
                    #pragma omp critical
                    if (!check_node(h, i)) push(h, dist[i], i);
                }
            }
        }
    }
    if (DEBUG == 3) {
        for (long i = 0; i < m_size; i++) printf("%d ", dist[i]);
        print_heap(h);
    }
    free(h);
}

int main(int argc, char** argv) {
    int m_size = strtol(argv[1], NULL, 10);
    int **m = generate_matrix(m_size, m_size);
    double tseq_start, tseq_stop, tpar_start, tpar_stop;

    rand_adj_matrix(m, m_size);
    if (DEBUG==1) print_matrix(m, m_size, m_size);

    int p = omp_get_num_procs();
    omp_set_num_threads(p);

    tpar_start = omp_get_wtime();
    johnson_par(m, m_size, 0);
    tpar_stop = omp_get_wtime();
    if (DEBUG) printf("par,%d,%d,%f s\n", 
            m_size, p, (tpar_stop-tpar_start));

    tseq_start = omp_get_wtime();
    johnson_seq(m, m_size, 0);
    tseq_stop = omp_get_wtime();
    if (DEBUG) printf("seq,%d,%d,%f s\n", 
            m_size, p, (tseq_stop-tseq_start));

    free_matrix(m);

    return 0;
}
