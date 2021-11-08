#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include "matrix_manipulation.h"

struct matrixRepresantion{
    int values[10];
    int columns[10];
    int nnz[5];

};

struct hadamard{
    int hadamardValues[10];
    int hadamardCols[10];
    int hadamardNNZ[5];
};


/**
 * calculates the common elements of two arrays
 * @param array1
 * @param array2
 * @param size1 of array1
 * @param size2 of array2
 * @return the number of the commons
 */

int commonElements(int *array1, int* array2, int size1, int size2){
    int counter = 0;
    for(int i = 0; i < size1; i++){
        for(int j = 0; j < size2; j++){
            if(array1[i] == array2[j])
                ++counter;
        }
    }
    return counter;
}


/**
 * calcualtes csr-hadamard result
 * @return a structure that concludes the three vectors
 */

int product(){
    bool flag = false;
    int nnzRows = 0;
    int nnzColumns = 0;
    struct matrixRepresantion array;
    array.values[10] = (1, 1, 1, 1, 1, 1, 1, 1, 1, 1);
    array.columns[10] = (1, 2, 3, 0, 3, 0, 3, 0, 1, 2);
    array.nnz[5] = (0, 3, 5, 7, 10);

    for(int i = 0; i < 4; i++){
        nnzRows = array.nnz[i+1] - array.nnz[i];                                   //find how many non-zero elements in i-th row
        int *holdTheColumns = (int*) malloc(nnzRows * sizeof(int));
        for(int k = 0; k < nnzRows; k++){
            holdTheColumns[k] =  array.columns[array.nnz[i]] + k;                             //find the position of them
            nnzColumns += array.nnz[holdTheColumns[k] + 1] - array.nnz[holdTheColumns[k]];    //find the number of non-zero elements in holdTheColumns[k]-th column
        }
        for(int j = 0; j < 4; j++){                                         //run a for loop for the cols of the sparse array
            if(i == j) continue;                                            //I am in the diagonal
            nnzRows = array.nnz[i+1] - array.nnz[i];                        //find how many non-zero elements in i-th row
            int *holdTheColumns = (int*) malloc(nnzRows * sizeof(int));     //hold memory for the non-zero elements you find in i-th row


            int *holdTheRows = (int*) malloc(nnzColumns * sizeof(int));

            for(int l = 0; l < nnzRows; l++){
                if(holdTheColumns[l] == j){
                    flag = true;
                    break;
                }
                else flag = false;
            }
            if(flag) continue;

            //find the row-position of non-zero elements
            for(int k = 0; k < nnzColumns; k++){
                holdTheRows[k] =  array.columns[array.nnz[j]] + k;
            }
            int theCommons = commonElements(holdTheColumns,holdTheRows,nnzRows,nnzColumns);
            printf("%d\n", theCommons);
            free(holdTheRows);
        }
        free(holdTheColumns);
    }
    return 0;
}


