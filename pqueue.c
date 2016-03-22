#include "pqueue.h"
#define INF 9999

void push (heap_t *h, int priority, int data) {
    if (h->len + 1 >= h->size) {
        h->size = h->size ? h->size*2 : 4;
        h->nodes = realloc(h->nodes, h->size*sizeof(node_t));
    }
    int i = h->len + 1;
    int j = i / 2;
    while (i > 1 && h->nodes[j].priority > priority) {
        h->nodes[i] = h->nodes[j];
        i = j;
        j /= 2;
    }
    h->nodes[i].priority = priority;
    h->nodes[i].data = data;
    h->len++;
}

int pop (heap_t *h) {
    int i, j, k;
    if (!h->len) {
        return -INF;
    }
    int data = h->nodes[1].data;
    h->nodes[1] = h->nodes[h->len];
    h->len--;
    i = 1;
    while (i) {
        k = 1;
        j = 2*i;
        if (j <= h->len && h->nodes[j].priority < h->nodes[k].priority) {
            k = j;
        }
        if (j + 1 <= h->len && h->nodes[j + 1].priority < h->nodes[k].priority) {
            k = j + 1;
        }
        if (k == i) {
            break;
        }
        h->nodes[i] = h->nodes[k];
        i = k;
    }
    h->nodes[i] = h->nodes[h->len + 1];
    return data;
}

int check_node(heap_t *h, int data) {
    int found = 0;
    for (int i = 1; i <= h->len; i++) {
        if (data == h->nodes[i].data) {
            found = 1;
            break;
        }
    }
    return found;
}

void print_heap(heap_t *h) {
    for (int i = 1; i <= h->len; i++) {
        printf("%d ", h->nodes[i].data);
    }
    printf("\n");
}

