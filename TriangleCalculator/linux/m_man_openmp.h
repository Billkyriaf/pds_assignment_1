#ifndef TRIANGLECALCULATOR_M_MAN_OPENMP_H
#define TRIANGLECALCULATOR_M_MAN_OPENMP_H
#include "matrix_manipulation.h"

void productOpenmp(CSR *input, CSR *output, int numberOfThreads);

#endif
