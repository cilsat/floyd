#include <stdio.h>
#include <stdlib.h>

typedef struct {
    int priority;
    int data;
} node_t;

typedef struct {
    node_t *nodes;
    int len;
    int size;
} heap_t;

void push (heap_t *h, int priority, int data);
int pop (heap_t *h);
int check_node(heap_t *h, int data);
void print_heap(heap_t *h);


