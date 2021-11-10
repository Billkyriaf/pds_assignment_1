#include <stdlib.h>
#include "matrix_manipulation.h"

int colRowProduct(int elements[], int colStart, int colEnd, int rowStart, int rowEnd){
    int res = 0;
    int offset = 0;

    if (colEnd - colStart > rowEnd - rowStart){
        for (int i = 0; i <= rowEnd - rowStart; ++i) {
            if (offset > colEnd - colStart)
                break;


            if (elements[rowStart + i] < elements[colStart + offset]){
                continue;

            } else if (elements[rowStart + i] > elements[colStart + offset]) {
                i--;
                offset++;
                continue;
            } else {
                res++;
                offset++;
                continue;
            }
        }
    } else {
        for (int i = 0; i <= colEnd - colStart; ++i) {
            if (offset > rowEnd - rowStart)
                break;

            if (elements[rowStart + offset] < elements[colStart + i]){
                i--;
                offset++;
                continue;

            } else if (elements[rowStart + offset] > elements[colStart + i]) {
                continue;
            } else {
                res++;
                offset++;
                continue;
            }
        }
    }
    return res;
}

void productImproved(CSR *input, CSR *output) {
    int counter = 0;
    int firstRow;
    int lastRow;
    int firstCol;
    int lastCol;
    int nnzInRow = 0;
    int nnzInCol = 0;

    output->A = (int *) malloc(input->nonzero * sizeof(int));
    output->JA = (int *) malloc(input->nonzero * sizeof(int));
    output->IA = (int *) calloc(input->size + 1, sizeof(int));
    output->nonzero = input->nonzero;
    output->size = input->size;

    for (int row = 0; row < input->size; row++) {
        nnzInRow = input->IA[row + 1] - input->IA[row];  // find how many non-zero elements in i-th row
        firstRow = input->IA[row];
        lastRow = input->IA[row] + nnzInRow - 1;

        // We want from JA[input->IA[row]] to JA[input->IA[row] + nnzInRow]
        // JA[firstRow + colNumber] gives me the index of the colNumber th column
        for (int colNumber = 0; colNumber < nnzInRow; ++colNumber) {
            nnzInCol = input->IA[input->JA[firstRow + colNumber] + 1] - input->IA[input->JA[firstRow + colNumber]];
            firstCol = input->IA[input->JA[firstRow + colNumber]];
            lastCol = input->IA[input->JA[firstRow + colNumber]] + nnzInCol - 1;

            output->A[counter] = colRowProduct(input->JA, firstCol, lastCol, firstRow, lastRow);
            output->JA[counter] = input->JA[firstRow + colNumber];
            output->IA[row + 1]++;
            ++counter;
        }
    }
    for (int i = 1; i <= output->size; i++) {
        output->IA[i] += output->IA[i - 1];
    }
}

int measureTriangles(CSR input){
    int triangles = 0;

    for (int i = 0; i < input.nonzero; ++i) {
        triangles += input.A[i];
    }

    return triangles/6;
}



