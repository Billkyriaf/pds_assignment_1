#include <stdio.h>
#include <stdlib.h>
#include "csr_matrix.h"
#include "matrix_manipulation.h"
#include "mmio.h"

// Reads matrix from file
int readMTX(char *fileName, CSR *matrix){
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

    /* allocate memory for csr vectors */
    matrix->A = (int *) malloc(2 * nz * sizeof(int));  // All the nonzero elements of the array
    matrix->JA = (int *) malloc(2 * nz * sizeof(int));  // The columns (j) of all the nonzero elements in the array

    /*
     * The ith element is the sum of the (i - 1)th element with the number of nonzero elements in the ith row.
     */
    matrix->IA = (int *) malloc((M + 1) * sizeof(int));

    // Initialize the array to zeros
    for(int i = 0; i <= M; i++){
        matrix->IA[i] = 0;
    }

    //FIXME !!!! add nonzero size
    matrix->size = M;  // The number of nonzero elements and consequently the length of the A and JA vectors

    int indexI, indexJ;  // The i and j read from the mtx file for each line

    // For all the nonzero elements...
    for (int i = 0; i < nz; i++){
        fscanf(f, "%d %d\n", &indexI, &indexJ); // ...read the next line from the file

        // Adjust from 1-based to 0-based
        indexJ--;

        matrix->A[i] = 1;  // add the weight of the nonzero element (always 1)
        matrix->JA[i] = indexJ;  // add the column of the nonzero element
        matrix->IA[indexI]++;  // +1 for a nonzero element in the indexI row
    }

    for(int i = 1; i <= M; i++){
        matrix->IA[i] = matrix->IA[i] + matrix->IA[i - 1];
    }

    if (f != stdin)
        fclose(f);

    return 0;
}

void printCSR(CSR matrix){
    //FIXME nonzero elements
    printf("A:  [");
    for (int i = 0; i < 7; i++){
        printf("%d ", matrix.A[i]);
    }
    printf("]\n");

    printf("JA: [");
    for (int i = 0; i < 7; i++){
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

int main(int argc, char **argv){

    if (argc < 2){
        fprintf(stderr, "Usage: %s [martix-market-filename]\n", argv[0]);
        exit(1);
    }

    CSR testMatrix;

    readMTX(argv[1], &testMatrix);

    printCSR(testMatrix);

    return 0;
}
