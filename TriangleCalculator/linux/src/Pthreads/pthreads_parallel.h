#ifndef TRIANGLECALCULATOR_PTHREADS_PARALLEL_H
#define TRIANGLECALCULATOR_PTHREADS_PARALLEL_H

#include "matrix_manipulation.h"

void pthreadProduct(CSR *input, CSR *output, int numberOfThreads);

#endif
