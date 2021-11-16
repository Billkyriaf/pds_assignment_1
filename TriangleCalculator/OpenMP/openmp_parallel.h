#ifndef TRIANGLECALCULATOR_OPENMP_PARALLEL_H
#define TRIANGLECALCULATOR_OPENMP_PARALLEL_H
#include "matrix_manipulation.h"

void openmpProduct(CSR *input, CSR *output, int numberOfThreads);

#endif
