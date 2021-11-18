#include <stdio.h>
#include <stdlib.h>
#include <cilk/cilk.h>
#include "cilk_parallel.h"


/**
 * Calculates the A .* (A * A) product. The Hadamard product is automatically calculated since we calculate the product
 * only on the non zero element positions and all the values of the initial array are 1. The function directly multiplies
 * A matrix with its self in CSR format.
 *
 * @param input    The pointer to the initial CSR matrix
 * @param output   The pointer to the result CSR matrix. The output CSR object MUST be initialized
 */
void cilkProduct(CSR *input, CSR *output, int nThreads) {
    int counter = 0;

    char threads[5];
    sprintf(threads, "%d", nThreads);
    __cilkrts_set_param("nworkers", threads);

    #pragma cilk grainsize 10
    // For every ith row of the matrix...
    cilk_for (int row = 0; row < input->size; row++) {
        int rowStart;  // The starting index of the row
        int rowEnd;  // The ending index of the row
        int colStart;  // The starting index of the column
        int colEnd;  // The ending index of the column
        int nnzInRow;  // The number of the nonzero elements in the row
        int nnzInCol;  // The number of the nonzero elements in the column

        nnzInRow = input->IA[row + 1] - input->IA[row];  // ...find how many nonzero elements in ith row from the IA vector
        rowStart = input->IA[row];  // The starting index of the row in the JA vector
        rowEnd = rowStart + nnzInRow - 1;  // The ending index of the row in the JA vector

        // We want the elements from JA[input->IA[row]] to JA[input->IA[row] + nnzInRow]
        // JA[rowStart + colNumber] gives the index of the colNumber th column
        // So for every column found in the row...
        for (int col = rowStart; col <= rowEnd; ++col) {
            // ...find how many nonzero elements in ith col column from the IA vector
            nnzInCol = input->IA[input->JA[col] + 1] - input->IA[input->JA[col]];
            colStart = input->IA[input->JA[col]];  // The starting index of the column in the JA vector
            colEnd = colStart + nnzInCol - 1;  // The ending index of the column in the JA vector

            // Find the common elements in the row and the column and pass the values to the output JA vector
            int res = colRowProduct(input->JA, colStart, colEnd, rowStart, rowEnd);
            output->A[col] = res;
            output->JA[col] = input->JA[col];
            output->IA[row + 1]++;
        }
    }

    // IA vector is the same, so we just copy the values. Not really ideal for performance but it only happens once and
    // is very simple. We want input and output to be independent that's why we don't just point output->IA to the same
    // memory address as the input->IA
    for (int i = 1; i <= output->size; i++) {
        output->IA[i] = input->IA[i];
    }

    output->triangles = measureTriangles(output);
    input->triangles = output->triangles;
}