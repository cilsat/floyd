#include <stdio.h>
#include <stdlib.h>
#include <time.h>

int **generate_matrix(long rows, long cols);
void rand_adj_matrix(int **matrix, long size);
void print_matrix(int **matrix, long rows, long cols);
void free_matrix(int **matrix);
