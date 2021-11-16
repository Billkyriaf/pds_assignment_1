#ifndef TRIANGLECALCULATOR_MATRIX_MANIPULATION_H
#define TRIANGLECALCULATOR_MATRIX_MANIPULATION_H


/**
 * Stores all the information needed to represent a matrix in CSR format
 */
typedef struct csr {
    int *A;  // Values vector. Initially all ones
    int *IA;  // Number of nonzero elements per row
    int *JA;  // Column indexes of nonzero elements

    int size;  // The size of the matrix represented (square matrices only)
    int nonzero;  // The number of the nonzero elements of the matrix
    int triangles;  // The number of triangles in the graph represented by the matrix
} CSR;


int colRowProduct(const int *elements, int colStart, int colEnd, int rowStart, int rowEnd);

int measureTriangles(CSR *matrix);

#endif
