#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <time.h>
#include "csr_matrix.h"
#include "matrix_manipulation.h"
#include "mmio.h"


void printArray(int **arr, int size)
{
    int i;
    for (i=0; i < size; i++)
        fprintf(stdout, "[%d, %d]\n", arr[i][0], arr[i][1]);
}

void swap(int *i1, int *j1, int *i2, int *j2){
    int temp = *i1;
    *i1 = *i2;
    *i2 = temp;

    temp = *j1;
    *j1 = *j2;
    *j2 = temp;
}

// A function to implement bubble sort
void bubbleSort(int **graph, int size){
    bool swapped;
    for (int i = 0; i < size - 1; i++) {
        swapped = false;

        // Last i elements are already in place
        for (int j = 0; j < size - i - 1; j++) {
            if (graph[j][0] > graph[j + 1][0]) {
                swap(&graph[j][0], &graph[j][1], &graph[j + 1][0], &graph[j + 1][1]);
                swapped = true;
            }
        }

        // IF no two elements were swapped by inner loop, then break
        if (swapped == false)
            break;
    }
}

void createCSR(CSR *matrix, int **graph){
    int currentCol = 0;

    for (int i = 0; i < matrix->nonzero; ++i) {
        printf("%d\n", i);
        for (int j = 0; j < matrix->nonzero; ++j) {
            if (graph[j][0] == currentCol){
                matrix->A[i] = 1;
                matrix->JA[i] = graph[j][1];
                matrix->IA[graph[j][0] + 1]++;
                graph[j][0] = -1;
                break;
            } else if (j == matrix->nonzero - 1){
                currentCol++;
                i--;
            }
        }
    }
}

// Reads matrix from file
int readMTX(char *fileName, CSR *matrix){
    FILE *f;
    MM_typecode matrixCode;
    int M, N, nz;

    time_t t1 = time(NULL);

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


    // Import the entire list to an array including both elements (from the lower and upper triangular matrix)
    int **graph = (int **) malloc(2 * nz * sizeof (int *));
    for (int i = 0; i < 2 * nz; ++i) {
        graph[i] = (int*) malloc(2 * sizeof (int));
    }

    int indexI, indexJ;  // The i and j read from the mtx file for each line

    time_t t2 = time(NULL);

    printf("Prep time: %lds\n", (t2 - t1));

    t1 = time(NULL);

    for (int i = 0; i < 2 * nz; i+=2){
        fscanf(f, "%d %d\n", &indexI, &indexJ); // ...read the next line from the file

        // Adjust from 1-base to 0-base
        indexJ--;
        indexI--;

        graph[i][0] = indexJ;
        graph[i][1] = indexI;

        graph[i + 1][0] = indexI;
        graph[i + 1][1] = indexJ;
    }

    t2 = time(NULL);
    printf("Read time: %lds\n", (t2 - t1));

    // Close the file
    if (f != stdin)
        fclose(f);
    free(f);

    /*
    t1 = time(NULL);
    // Sort the graph
    bubbleSort(graph, 2 * nz);

    t2 = time(NULL);
    printf("Sorting time: %lds\n", (t2 - t1));

    // printArray(graph, 2 * nz);
    */


    t1 = time(NULL);
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

    t2 = time(NULL);
    printf("CSR prep time: %lds\n", (t2 - t1));

    t1 = time(NULL);

    /*
    // For all the nonzero elements...
    for (int i = 0; i < matrix->nonzero; i++){
        matrix->A[i] = 1;  // add the weight of the nonzero element (always 1)
        matrix->JA[i] = graph[i][1];  // add the column of the nonzero element
        matrix->IA[graph[i][0] + 1]++;  // +1 for a nonzero element in the indexI row
    }
    */

    createCSR(matrix, graph);

    for(int i = 1; i <= matrix->size; i++){
        matrix->IA[i] = matrix->IA[i] + matrix->IA[i - 1];
    }

    t2 = time(NULL);
    printf("CSR create time: %lds\n", (t2 - t1));

    return 0;
}

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
