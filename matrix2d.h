#include <stdio.h>
#include <stdlib.h>
#include <time.h>

int **generate_matrix(int rows, int cols);
void rand_adj_matrix(int **matrix, int size);
void print_matrix(int **matrix, int rows, int cols);
void free_matrix(int **matrix);
