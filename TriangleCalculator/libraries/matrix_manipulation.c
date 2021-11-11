#include <stdlib.h>
#include <pthread.h>
#include "matrix_manipulation.h"
#define MAX_THREADS 16

typedef struct params {
    int *elements;
    int colStart;
    int colEnd;
    int rowStart;
    int rowEnd;
    int columnNumber;
    int currentRow;
    CSR *output;
} Params;


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

void *colRowParallel(void *arg){
    Params *params = (Params *)arg;
    int res = 0;
    int offset = 0;

    if (params->colEnd - params->colStart > params->colEnd - params->rowStart){
        for (int i = 0; i <= params->rowEnd - params->rowStart; ++i) {
            if (offset > params->colEnd - params->colStart)
                break;


            if (params->elements[params->rowStart + i] < params->elements[params->colStart + offset]){
                continue;

            } else if (params->elements[params->rowStart + i] > params->elements[params->colStart + offset]) {
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
        for (int i = 0; i <= params->colEnd - params->colStart; ++i) {
            if (offset > params->rowEnd - params->rowStart)
                break;

            if (params->elements[params->rowStart + offset] < params->elements[params->colStart + i]){
                i--;
                offset++;
                continue;

            } else if (params->elements[params->rowStart + offset] > params->elements[params->colStart + i]) {
                continue;
            } else {
                res++;
                offset++;
                continue;
            }
        }
    }

    params->output->A[params->rowStart + params->columnNumber] = res;
    params->output->JA[params->rowStart + params->columnNumber] = params->elements[params->rowStart + params->columnNumber];
    params->output->IA[params->currentRow + 1]++;


    pthread_exit(NULL);
}

void productParallel(CSR *input, CSR *output, int numberOfThreads) {
    pthread_t *threads;
    pthread_attr_t pthread_custom_attr;
    Params *p;
    int counter = 0;
    int firstRow;
    int lastRow;
    int firstCol;
    int lastCol;
    int nnzInRow = 0;
    int nnzInCol = 0;

    threads = (pthread_t *)malloc(numberOfThreads*sizeof(pthread_t));
    pthread_attr_init(&pthread_custom_attr);

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

            Params params;

            params.elements = input->JA;
            params.rowStart = firstRow;
            params.rowEnd = lastRow;
            params.colStart = firstCol;
            params.colEnd = lastCol;
            params.columnNumber = colNumber;
            params.currentRow = row;
            params.output = output;



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



