#ifndef TRIANGLECALCULATOR_MATRIX_MANIPULATION_H
#define TRIANGLECALCULATOR_MATRIX_MANIPULATION_H

typedef struct csr {
    int *A;
    int *IA;
    int *JA;

    int size;
    int nonzero;
} CSR;


void product(CSR *input, CSR *output);

int measureTriangles(CSR input);

void productImproved(CSR *input, CSR *output);

#endif
