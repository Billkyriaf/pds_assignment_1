#ifndef TRIANGLE_CALCULATOR_CSR_MATRIX_H
#define TRIANGLE_CALCULATOR_CSR_MATRIX_H

#include "matrix_manipulation.h"

void printCSR(CSR matrix, int nPrint);

int createCSR(CSR *matrix, char* fileName);

void createSparseArray(const int *I, const int *J, int m, int n, int nz);

#endif
