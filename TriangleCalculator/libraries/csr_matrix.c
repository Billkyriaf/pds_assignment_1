#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <time.h>
#include "csr_matrix.h"
#include "matrix_manipulation.h"
#include "mmio.h"

/**
 * @var cr
 * @var nextItem
 */
struct crs{
    int cr;
    int init;
    struct crs *nextItem;
};

typedef struct crs RowCol;

void printCSR(CSR matrix){
    printf("A:  [");
    for (int i = 0; i < matrix.nonzero; i++){
        printf("%d ", matrix.A[i]);
    }
    printf("]\n");

    printf("JA: [");
    for (int i = 0; i < matrix.nonzero; i++){
        printf("%d ", matrix.JA[i]);
    }
    printf("]\n");

    printf("IA: [");
    for (int i = 0; i <= matrix.size; i++){
        printf("%d ", matrix.IA[i]);
    }
    printf("]\n");

    printf("Matrix size: %d\n", matrix.size);
}

int importFromFile(CSR *matrix, char* fileName){
    FILE *f;
    MM_typecode matrixCode;
    int M, N, nz;

    if ((f = fopen(fileName, "r")) == NULL){
        fprintf(stderr, "File %s not found\n", fileName);
        return MM_COULD_NOT_READ_FILE;
    }

    if (mm_read_banner(f, &matrixCode) != 0){
        printf("Could not process Matrix Market banner.\n");
        return MM_NO_HEADER;
    }

    /*  This is how one can screen matrix types if their application */
    /*  only supports a subset of the Matrix Market data types.      */
    if (mm_is_complex(matrixCode) && mm_is_matrix(matrixCode) && mm_is_sparse(matrixCode) ){
        printf("Sorry, this application does not support ");
        printf("Market Market type: [%s]\n", mm_typecode_to_str(matrixCode));
        return MM_UNSUPPORTED_TYPE;
    }

    /* find out size of sparse matrix .... */
    if ((mm_read_mtx_crd_size(f, &M, &N, &nz)) !=0)
        return MM_PREMATURE_EOF;


    RowCol *lowerGraph = (RowCol *) malloc(N * sizeof (RowCol));
    RowCol *upperGraph = (RowCol*) malloc(M * sizeof (RowCol));

    // allocate memory for csr vectors
    matrix->A = (int *) malloc(2 * nz * sizeof(int));  // All the nonzero elements of the array
    matrix->JA = (int *) malloc(2 * nz * sizeof(int));  // The columns (j) of all the nonzero elements in the array


    // The ith element is the sum of the (i - 1)th element with the number of nonzero elements in the ith row.
    matrix->IA = (int *) malloc((M + 1) * sizeof(int));

    // Initialize the array to zeros
    for(int i = 0; i <= M; i++){
        matrix->IA[i] = 0;
    }

    matrix->size = M;
    matrix->nonzero = 2 * nz;  // The number of nonzero elements and consequently the length of the A and JA vectors

    int indexI, indexJ;

    for (int i = 0; i < nz; i++){
        fscanf(f, "%d %d\n", &indexI, &indexJ); // ...read the next line from the file

        matrix->IA[indexI]++;
        matrix->IA[indexJ]++;

        // Adjust from 1-base to 0-base
        indexJ--;
        indexI--;

        if (upperGraph[indexJ].init != 1){

            upperGraph[indexJ].cr = indexI;
            upperGraph[indexJ].init = 1;
            upperGraph[indexJ].nextItem = NULL;
            //printf("IndexI: %d, IndexJ: %d, cr: %d\n", indexI, indexJ, upperGraph[indexJ].cr);
        } else {
            RowCol *next = &upperGraph[indexJ];

            while (true){
                if (next->nextItem != NULL){
                    next = next->nextItem;
                } else {
                    RowCol *temp = (RowCol *) malloc(sizeof(RowCol));
                    temp->cr = indexI;
                    temp->init = 1;
                    temp->nextItem = NULL;
                    next->nextItem = temp;
                    //printf("IndexI: %d, IndexJ: %d, cr: %d\n", indexI, indexJ, next->nextItem->cr);
                    break;
                }
            }
        }

        if (lowerGraph[indexI].init != 1){

            lowerGraph[indexI].cr = indexJ;
            lowerGraph[indexI].init = 1;
            lowerGraph[indexI].nextItem = NULL;
            //printf("IndexI: %d, IndexJ: %d, cr: %d\n", indexI, indexJ, lowerGraph[indexI].cr);

        } else {
            RowCol *next = &lowerGraph[indexI];

            while (true) {
                if (next->nextItem != NULL) {
                    next = next->nextItem;
                } else {
                    RowCol *temp = (RowCol *) malloc(sizeof(RowCol));
                    temp->cr = indexJ;
                    temp->init = 1;
                    temp->nextItem = NULL;
                    next->nextItem = temp;
                    //printf("IndexI: %d, IndexJ: %d, cr: %d\n", indexI, indexJ, next->nextItem->cr);
                    break;
                }
            }
        }
    }

    for(int i = 1; i <= matrix->size; i++){
        matrix->IA[i] = matrix->IA[i] + matrix->IA[i - 1];
    }

    int aIndex = 0;
    int lowIndex = 0;
    RowCol *next;

    for (int i = 0; i < M; ++i) {
        next = &upperGraph[i];
        while (next->init != 0){
            matrix->JA[aIndex] = next->cr;
            matrix->A[aIndex] = 1;
            aIndex++;


            if(next->nextItem == NULL){
                break;
            } else {
                next = next->nextItem;
            }
        }

        if (lowIndex < N) {
            while (lowerGraph[lowIndex].init != 1) {
                lowIndex++;
            }

            next = &lowerGraph[lowIndex];

            while (next->init == 1) {
                matrix->JA[aIndex] = next->cr;
                matrix->A[aIndex] = 1;
                aIndex++;

                if (next->nextItem == NULL) {
                    break;
                } else {
                    next = next->nextItem;
                }
            }
            lowIndex++;
        }
    }

    return 0;
}

int main(int argc, char **argv){

    if (argc < 2){
        fprintf(stderr, "Usage: %s [martix-market-filename]\n", argv[0]);
        exit(1);
    }

    CSR testMatrix;
    time_t t1 = time(NULL);

    importFromFile(&testMatrix, argv[1]);

    time_t t2 = time(NULL);
    printf("Time for matrix creation: %lds\n", t2 - t1);
    //printCSR(testMatrix);

    return 0;
}
