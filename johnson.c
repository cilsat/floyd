#include <stdio.h>
#include <stdlib.h>
#include <omp.h>
#include "matrix2d.h"
#include "pqueue.h"

#define DEBUG -1
#define INF 9999

void johnson_seq(int **m, int m_size, int source) {
    heap_t *h = calloc(1, sizeof(heap_t));
    int dist[m_size];

    dist[source] = 0;
    push(h, 0, source);
    for (int i = 0; i < m_size; i++) {
        if (i != source) dist[i] = INF;
    }

    while (1) {
        int u = pop(h);
        if (u == -INF) break;
        for (int i = 0; i < m_size; i++) {
            int cur_dist = m[u][i];
            if (DEBUG == 1) printf("%d %d %d\n", i, cur_dist, dist[i]);
            if (cur_dist != INF && cur_dist + dist[u] < dist[i]) {
                dist[i] = cur_dist + dist[u];
                if (!check_node(h, i)) push(h, dist[i], i);
            }
        }
        if (DEBUG == 2) {
            for (int i = 0; i < m_size; i++) printf("%d ", dist[i]);
            print_heap(h);
        }
    }
    if (DEBUG == 3) {
        for (int i = 0; i < m_size; i++) printf("%d ", dist[i]);
        printf("\n");
        print_heap(h);
    }
    free(h);
}

void johnson_par(int **m, int m_size, int source) {
    heap_t *h = calloc(1, sizeof(heap_t));
    int dist[m_size];
    int i, u;
    int cur_dist;
    omp_set_num_threads(4);

    dist[source] = 0;
    push(h, 0, source);
    for (int i = 0; i < m_size; i++) {
        if (i != source) dist[i] = INF;
    }

    while (1) {
        u = pop(h);
        if (u == -INF) {
            break;
        }
        else {
            #pragma omp parallel for private(i, cur_dist)
            for (i = 0; i < m_size; i++) {
                cur_dist = m[u][i];
                if (DEBUG == 1) printf("%d %d %d\n", i, cur_dist, dist[i]);
                if (cur_dist != INF && cur_dist + dist[u] < dist[i]) {
                    dist[i] = cur_dist + dist[u];
                    #pragma omp critical
                    if (!check_node(h, i)) push(h, dist[i], i);
                }
            }
            if (DEBUG == 2) {
                for (int i = 0; i < m_size; i++) printf("%d ", dist[i]);
                printf("\n");
                print_heap(h);
                printf("\n");
            }
        }
    }
    if (DEBUG == 3) {
        for (int i = 0; i < m_size; i++) printf("%d ", dist[i]);
        print_heap(h);
    }
    free(h);
}

int main(int argc, char** argv) {
    int m_size = strtol(argv[1], NULL, 10);
    int **m = generate_matrix(m_size, m_size);
    clock_t tstart, tstop;

    rand_adj_matrix(m, m_size);
    if (DEBUG==1) print_matrix(m, m_size, m_size);

    tstart = clock();
    johnson_par(m, m_size, 0);
    tstop = clock();
    if (DEBUG) printf("Time elapsed: %.5f s\n\n", (tstop-tstart)/(double)CLOCKS_PER_SEC);

    tstart = clock();
    johnson_seq(m, m_size, 0);
    tstop = clock();
    if (DEBUG) printf("Time elapsed: %.5f s\n\n", (tstop-tstart)/(double)CLOCKS_PER_SEC);

    free_matrix(m);

    return 0;
}
