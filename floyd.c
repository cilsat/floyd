#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <time.h>
#include <mpi.h>

#define DEBUG 0
#define BIDIRECTIONAL 0

static inline int min(int, int);
static inline int block_low(int, int, int);
static inline int block_owner(int, int, int);
static inline int block_size(int, int, int);


// Construct a *contiguous* square matrix
int **generate_matrix(int rows, int cols) {
    int **matrix = (int **)malloc(rows*sizeof(int*));
    int *data = (int *)malloc(rows*cols*sizeof(int));
    for (int i = 0; i < rows; i++)
        matrix[i] = &(data[cols*i]);
    return matrix;
}

// Populate a square matrix randomly
// The generated vertices are bidirectional (same weight forwards and backwards)
// Hence the result is a square symmetrical matrix
void rand_adj_matrix(int** matrix, int size) {
    srand(time(NULL));
    for (int i = 0; i < size; i++) {
        if (BIDIRECTIONAL == 1) {
            for (int j = i; j < size; j++) {
                int r;
                if (i == j) r = 0;
                else {
                    r = (int) rand() % 12 + 1;
                    if (r >= 10) r = 999;
                }
                matrix[i][j] = r;
                matrix[j][i] = r;
            }
        } else {
            for (int j = 0; j < size; j++) {
                int r;
                if (i == j) r = 0;
                else {
                    r = (int) rand() % 12 + 1;
                    if (r >= 10) r = 999;
                }
                matrix[i][j] = r;
            }
        }
    }
}

// Output a matrix to stdout
void print_matrix(int** matrix, int rows, int cols) {
    for (int i = 0; i < rows; i++) {
        for (int j = 0; j < cols; j++) {
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

// Sequential implementation of Floyd's algorithm
void floyd_seq(int** matrix, int size) {
    for (int k = 0; k < size; k++) {
        for (int i = 0; i < size; i++) {
            for (int j = 0; j < size; j++) {
                matrix[i][j] = min(matrix[i][j], matrix[i][k] + matrix[k][j]);
            }
        }
    }
}

// Parallel implementation of Floyd's algorithm
void floyd_par(int proc_id, int proc_sz, int** m, int size) {
    int offset;
    int root;
    int* temp = (int *)malloc(size*sizeof(int));

    for (int k = 0; k < size; k++) {
        root = block_owner(k, proc_sz, size);
        if (root == proc_id) {
            offset = k - block_low(proc_id, proc_sz, size)/size;
            for (int j = 0; j < size; j++)
                temp[j] = m[offset][j];
        }
        //broadcasts "temp", which is size*MPI_INT long, by rank root
        MPI_Bcast(temp, size, MPI_INT, root, MPI_COMM_WORLD);
        for (int i = 0; i < block_size(proc_id, proc_sz, size)/size; i ++) {
            for (int j = 0; j < size; j++)
                m[i][j] = min(m[i][j], m[i][k] + temp[j]);
        }
    }
    free(temp);
} 

// Simple function that returns minimum of two input integers
inline int min(int a, int b) {
    int x = a > b ? b : a;
    return x;
}

// Returns the rank of the process assigned to this row
inline int block_owner(int row_id, int proc_sz, int row_sz) {
    return proc_sz*row_id/row_sz;
}

// Returns the number of rows assigned to a particular process
inline int block_size(int proc_id, int proc_sz, int row_sz) {
    return row_sz*row_sz/proc_sz;
}

// Returns the matrix row mapped to a particular process
inline int block_low(int proc_id, int proc_sz, int row_sz) {
    return proc_id*row_sz*row_sz/proc_sz;
}

int main(int argc, char** argv) {
    int proc;    //number of processes
    int rank;    //process rank
    int** m;    //global matrix
    int** m_part;   // local matrix
    int args[argc];

    MPI_Init(NULL, NULL);
    MPI_Comm_size(MPI_COMM_WORLD, &proc);
    MPI_Comm_rank(MPI_COMM_WORLD, &rank);

    // read arguments
    for (int i = 0; i < argc-1; i++)
        args[i] = (int) strtol(argv[i+1], NULL, 10);

    // start up
    int sz_matrix = args[0];    //size of adjacency matrix
    if (sz_matrix % proc != 0) {
        fprintf(stderr,"%s: only works when n is factor of %d\n", argv[0], sz_matrix);
        MPI_Abort(MPI_COMM_WORLD,1);
    }

    // generate matrix in proc 0 and scatter to all processes
    m = generate_matrix(sz_matrix, sz_matrix);
    if (rank == 0) {
        rand_adj_matrix(m, sz_matrix);
        if (DEBUG == 1)
            print_matrix(m, sz_matrix, sz_matrix);
    }

    // generate scatter data and local data
    int proc_blksz[proc];
    int proc_offset[proc];
    for (int i = 0; i < proc; i++) {
        proc_blksz[i] = block_size(i, proc, sz_matrix);
        proc_offset[i] = block_low(i, proc, sz_matrix);
    }

    // init local matrices
    m_part = generate_matrix(proc_blksz[rank]/sz_matrix, sz_matrix);

    // scatter
    MPI_Scatterv(&(m[0][0]), proc_blksz, proc_offset, MPI_INT, &(m_part[0][0]), proc_blksz[rank], MPI_INT, 0, MPI_COMM_WORLD);

    // print local data
    if (DEBUG == 1) {
        for (int i = 0; i < proc; i++) {
            if (rank == i) {
                printf("rank: %d size: %d offset: %d\n", rank, proc_blksz[rank], proc_offset[rank]);
                print_matrix(m_part, proc_blksz[rank]/sz_matrix, sz_matrix);
            }
            MPI_Barrier(MPI_COMM_WORLD);
        }
    }

    // parallel floyd
    floyd_par(rank, proc, m_part, sz_matrix);

    if (DEBUG == 1) {
        // sequential floyd for reference
        if (rank == 0) {
            printf("sequential floyd:\n");
            floyd_seq(m, sz_matrix);
            print_matrix(m, sz_matrix, sz_matrix);
        }

        for (int i = 0; i < proc; i++) {
            if (rank == i) {
                printf("parallel floyd for rank: %d\n", rank);
                print_matrix(m_part, proc_blksz[rank]/sz_matrix, sz_matrix);
            }
            MPI_Barrier(MPI_COMM_WORLD);
        }
    }

    // clean up
    free_matrix(m);
    free_matrix(m_part);

    MPI_Finalize();
    return 0;
}
