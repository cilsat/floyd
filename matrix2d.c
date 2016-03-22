#include "matrix2d.h"

#define BIDIRECTIONAL 0
#define INF 9999

// Construct a *contiguous* square matrix
int **generate_matrix(long rows, long cols) {
    int **matrix = (int **)malloc(rows*sizeof(int*));
    int *data = (int *)malloc(rows*cols*sizeof(int));
    for (long i = 0; i < rows; i++)
        matrix[i] = &(data[cols*i]);
    return matrix;
}

// Populate a square matrix randomly
// The generated vertices are bidirectional (same weight forwards and backwards)
// Hence the result is a square symmetrical matrix
void rand_adj_matrix(int** matrix, long size) {
    srand(time(NULL));
    int r;
    long i, j;

    for (i = 0; i < size; i++) {
        if (BIDIRECTIONAL == 1) {
            for (j = i; j < size; j++) {
                if (i == j) r = 0;
                else {
                    r = (int) rand() % 12 + 1;
                    if (r >= 10) r = INF;
                }
                matrix[i][j] = r;
                matrix[j][i] = r;
            }
        } else {
            for (j = 0; j < size; j++) {
                if (i == j) r = 0;
                else {
                    r = (int) rand() % 12 + 1;
                    if (r >= 10) r = INF;
                }
                matrix[i][j] = r;
            }
        }
    }
}

// Print matrix to stdout
void print_matrix(int** matrix, long rows, long cols) {
    for (long i = 0; i < rows; i++) {
        for (long j = 0; j < cols; j++) {
            int r = matrix[i][j];
            if (r <= 100) printf("%d ", r);
            else printf("& ");
        }
        printf("\n");
    }
    printf("\n");
}

// Destructor for square matrix
void free_matrix(int** matrix) {
    free(matrix[0]);
    free(matrix);
}

