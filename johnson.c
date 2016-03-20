#include <stdio.h>
#include <stdlib.h>
#include <omp.h>
#include "matrix2d.h"

#define DEBUG 1

const int n_threads = omp_get_num_procs();
