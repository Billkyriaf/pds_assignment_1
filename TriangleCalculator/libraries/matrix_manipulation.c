#include <stdio.h>
#include <stdlib.h>
#include "matrix_manipulation.h"

/**
 * calculates the common elements of two arrays
 * @param array1
 * @param array2
 * @param size1 of array1
 * @param size2 of array2
 * @return the number of the commons
 */
int commonElements(int *array1, int *array2, int size1, int size2){
    int counter = 0;
    int start = 0;

    if(size1 <= size2) {
        for (int i = 0; i < size1; i++) {
            for (int j = start; j < size2; j++) {
                if (array1[i] == array2[j]) {
                    ++counter;
                    start = j + 1;
                    break;
                }
                if (array2[j] > array1[i]) {
                    start = j - 1;
                    break;
                }
            }
        }
    } else {
        for (int i = 0; i < size2; i++) {
            for (int j = start; j < size1; j++) {
                if (array1[j] == array2[i]) {
                    start = j + 1;
                    ++counter;
                    break;
                }
                if (array1[j] > array2[i]){
                    start = j - 1;
                    break;
                }
            }
        }
    }
    return counter;
}


/**
 * calcualtes csr-hadamard result
 * @return a structure that concludes the three vectors
 */

void product(CSR *input, CSR *output){
    int counter = 0;
    int nnzRows = 0;
    int nnzColumns = 0;

    output->A = (int*) malloc(input->nonzero * sizeof (int));
    output->JA = (int*) malloc(input->nonzero * sizeof (int));
    output->IA = (int*) calloc(input->size + 1, sizeof (int));
    output->nonzero = input->nonzero;
    output->size = input->size;

    for(int i = 0; i < input->size; i++) {
        nnzRows = input->IA[i + 1] - input->IA[i];  // find how many non-zero elements in i-th row
        int *holdTheColumns = (int *) malloc(nnzRows * sizeof(int));

        for (int k = 0; k < nnzRows; k++) {
            holdTheColumns[k] = input->JA[input->IA[i] + k];  // find the position of them
        }

        for (int j = 0; j < nnzRows; j++) {
            nnzColumns = input->IA[holdTheColumns[j] + 1] - input->IA[holdTheColumns[j]];
            int *holdTheRows = (int *) malloc(nnzColumns * sizeof(int));

            for (int k = 0; k < nnzColumns; k++) {
                holdTheRows[k] = input->JA[input->IA[holdTheColumns[j]] + k];  // find the position of them
            }
            output->A[counter] = commonElements(holdTheColumns, holdTheRows, nnzRows, nnzColumns);
            output->JA[counter] = holdTheColumns[j];
            output->IA[i + 1]++;
            ++counter;

            free(holdTheRows);
        }
        free(holdTheColumns);
    }
    for (int i = 1; i <= output->size; i++) {
        output->IA[i] = output->IA[i] + output->IA[i - 1];
    }
}

int measureTriangles(CSR input){
    int triangles = 0;

    for (int i = 0; i < input.nonzero; ++i) {
        triangles += input.A[i];
    }

    return triangles/6;
}


