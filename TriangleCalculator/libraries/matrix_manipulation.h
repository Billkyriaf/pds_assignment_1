#ifndef TRIANGLECALCULATOR_MATRIX_MANIPULATION_H
#define TRIANGLECALCULATOR_MATRIX_MANIPULATION_H

/**
 *
 */
typedef struct csr {
    int *A;
    int *IA;
    int *JA;

    int size;
    int nonzero;
} CSR;




#endif //TRIANGLECALCULATOR_MATRIX_MANIPULATION_H
