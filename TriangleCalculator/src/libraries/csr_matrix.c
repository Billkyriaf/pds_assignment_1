#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <string.h>
#include <sys/time.h>

#include "csr_matrix.h"
#include "mmio.h"
#include "write_csr_to_file.h"

/**
 * Struct for the linked list data structure that stores the rows.
 */
struct crs {
    int cr;  /// the number of column
    int init;  /// flag that shows if the current object is initialized
    struct crs *nextItem;  /// pointer to the next item in the list
};

typedef struct crs RowCol;

void createSparseArray(const int *I, const int *J, int m, int n, int nz){
    fprintf(stdout, "Printing matrix %d x %d...\n", n, m);
    int **matrix = (int **)malloc(n * sizeof(int*));
    for (int i = 0; i < n; ++i) {
        matrix[i] = (int *) malloc(m * sizeof(int));
    }

    for (int j = 0; j < n; ++j) {
        for (int i = 0; i < m; ++i) {
            matrix[j][i] = 0;
        }
    }

    for (int k = 0; k < nz; ++k) {
        matrix[I[k]][J[k]] = 1;
        matrix[J[k]][I[k]] = 1;
    }

    for (int j = 0; j < n; ++j) {
        for (int i = 0; i < m; ++i) {
            printf("%d ", matrix[j][i]);
        }
        printf("\n");
    }
}

/**
 * The print CSR function simply prints the csr matrix. if nPrint is > 0 the function prints the first nPrint and the
 * last nPrint elements
 * @param matrix The scr matrix the be printed
 * @param nPrint The number of elements to print. If smaller or equal to 0 prints all
 */
void printCSR(CSR matrix, int nPrint) {
    if (nPrint <= 0 || nPrint > matrix.nonzero) {
        if (matrix.A != NULL) {
            printf("A:  [");
            for (int i = 0; i < matrix.nonzero; i++) {
                printf("%d ", matrix.A[i]);
            }
            printf("]\n");
        }

        printf("JA: [");
        for (int i = 0; i < matrix.nonzero; i++) {
            printf("%d ", matrix.JA[i]);
        }
        printf("]\n");

        printf("IA: [");
        for (int i = 0; i <= matrix.size; i++) {
            printf("%d ", matrix.IA[i]);
        }
        printf("]\n");

        printf("Matrix size: %d\n", matrix.size);

    } else if (nPrint < matrix.nonzero) {
        if (matrix.A != NULL) {
            printf("A:  [");
            for (int i = 0; i < nPrint; i++) {
                printf("%d ", matrix.A[i]);
            }
            printf(".... ");
            for (int i = matrix.nonzero - nPrint; i < matrix.nonzero; i++) {
                printf("%d ", matrix.A[i]);
            }
            printf("]\n");
        }

        printf("JA: [");

        for (int i = 0; i < nPrint; i++) {
            printf("%d ", matrix.JA[i]);
        }
        printf(".... ");
        for (int i = matrix.nonzero - nPrint; i < matrix.nonzero; i++) {
            printf("%d ", matrix.JA[i]);
        }
        printf("]\n");

        if (nPrint > matrix.size) {
            printf("IA: [");
            for (int i = 0; i <= matrix.size; i++) {
                printf("%d ", matrix.IA[i]);
            }
            printf("]\n");
        } else {
            printf("IA: [");
            for (int i = 0; i <= nPrint; i++) {
                printf("%d ", matrix.IA[i]);
            }
            printf(".... ");
            for (int i = matrix.size - nPrint; i <= matrix.size; i++) {
                printf("%d ", matrix.IA[i]);
            }
            printf("]\n");
        }

        printf("Matrix size: %d\n", matrix.size);
    } else {
        fprintf(stderr, "An error occurred while printing please check the limits you provided!!\n");
    }
}

/***
 * The createCSR function reads a matrix market file and creates the CSR vectors for the sparse matrix of the graph
 * represented in the file.
 *
 * NOTE: The files handled from this function must have %%MatrixMarket matrix coordinate pattern symmetric header.
 * Meaning that the graphs are unweighted and undirected. This means that the sparse matrix is square symmetrical and all
 * the non zero values are 1. If the file has 3 items per line (i j value) please modify the code (scanf function) to
 * ignore the value part.
 *
 * @param matrix   Pointer to the CSR struct object the csr matrix is saved. All vectors should be unallocated
 * @param fileName    Array of chars that stores the absolute path to the .mtx file (Read NOTE for more info about the file)
 * @return   The integer returned indicates the state of the operation
 *           Return value:
 *           0: creation was successful
 *           11: Could not read file
 *           12: Premature end of file (check file integrity and header)
 *           14: Could note read header
 *           15: Matrix is not supported
 */
int createCSR(CSR *matrix, char *fileName) {
    FILE *f;
    MM_typecode matrixCode;
    int M, N, nz;

    // Open the file ...
    if ((f = fopen(fileName, "r")) == NULL) {
        fprintf(stderr, "File %s not found\n", fileName);
        return MM_COULD_NOT_READ_FILE;
    }

    // ... read the information of the header ...
    if (mm_read_banner(f, &matrixCode) != 0) {
        printf("Could not process Matrix Market banner.\n");
        return MM_NO_HEADER;
    }

    // ... check if the matrix is supported. Note that this operation checks if the mmio.h library can support
    // the matrix type. For this function to work properly the matrix type should be as described on the functions
    // documentation
    if (mm_is_complex(matrixCode) && mm_is_matrix(matrixCode) && mm_is_sparse(matrixCode)) {
        printf("Sorry, this application does not support ");
        printf("Market Market type: [%s]\n", mm_typecode_to_str(matrixCode));
        return MM_UNSUPPORTED_TYPE;
    }

    // Finally, read the first line from the file that contains the size of the matrix and the number on nonzero elements.
    // Note that the files processed here contain information only fot the bottom triangle of the matrix so the real
    // number of nonzero elements is double
    if ((mm_read_mtx_crd_size(f, &M, &N, &nz)) != 0)
        return MM_PREMATURE_EOF;


    /*
     * lowerGraph stores all the nonzero elements found below the main diagonal organized in rows. The item found in
     * lowerGraph[i] is the first nonzero element of the ith row of the bottom triangular matrix. This nonzero element
     * is represented with an object from the RowCol struct and is the head of the linked list representing the row. If
     * a row does not have any nonzero elements the RowCol object has the init flag set to 0 and the column number
     * set to -1.
     *
     *   e.g.
     *
     *   lowerGraph = [ RowCol_0                     RowCol_1                     RowCol_i ]
     *                  ??? cr: number of column       ??? cr: -1 no nz               ??? cr: number of column
     *                  |     of the first nz        |      elements              |     of the first nz
     *                  |     element of 0 row       ??? init: 0                    |     element of the ith row
     *                  |                            |                            |
     *                  ??? nextItem                   ??? nextItem: NULL             ??? nextItem
     *                    ??? cr: number of column                                    ??? ...
     *                    |     of the first nz                                     ??? ...
     *                    |     element of 0 row
     *                    |
     *                    ??? nextItem
     *                       ??? ...
     *                       ??? nextItem: NULL
     */
    RowCol *lowerGraph = (RowCol *) malloc(N * sizeof(RowCol));

    /* Upper graph works the same way lower graph does with the only difference being that is stores all the nonzero
     * elements found above the main diagonal organized in rows.
     */
    RowCol *upperGraph = (RowCol *) malloc(M * sizeof(RowCol));

    // Set the default values for the uninitialized elements init = 0. cr = -1 and the nextItem pointer is NULL
    for (int i = 0; i < N; ++i) {
        lowerGraph[i].init = 0;
        lowerGraph[i].nextItem = NULL;
        lowerGraph[i].cr = -1;
    }

    for (int i = 0; i < M; ++i) {
        upperGraph[i].init = 0;
        upperGraph[i].nextItem = NULL;
        upperGraph[i].cr = -1;
    }

    // allocate memory for csr vectors
    matrix->JA = (int *) malloc(2 * nz * sizeof(int));  // The columns (j) of all the nonzero elements in the array
    matrix->A = NULL;  // Matrix A stores all ones for the initial read, so we do not need to allocate the memory


    // The ith element is the sum of the (i - 1)th element with the number of nonzero elements in the ith row.
    matrix->IA = (int *) malloc((M + 1) * sizeof(int));

    // Initialize the array to zeros
    for (int i = 0; i <= M; i++) {
        matrix->IA[i] = 0;
    }

    matrix->size = M;  // The size of the array
    matrix->nonzero = 2 * nz;  // The number of nonzero elements and consequently the length of the A and JA vectors

    int indexI = 0, indexJ = 0;  // The values read from the file

    // Start reading from the file...
    for (int i = 0; i < nz; i++) {
        fscanf(f, "%d %d\n", &indexI, &indexJ); // ...read the next line from the file

        // add 1 to the corresponding row counter for both symmetric nz elements
        matrix->IA[indexI]++;
        matrix->IA[indexJ]++;

        // Adjust from 1-base to 0-base
        indexJ--;
        indexI--;

        // Fill the upper graph
        if (upperGraph[indexJ].init != 1) {
            // If the head element of the row is not initialized it means this is the first nz element of the row
            upperGraph[indexJ].cr = indexI;  // Pass the column number...
            upperGraph[indexJ].init = 1;  // ...declare the head initialized

            //printf("IndexI: %d, IndexJ: %d, cr: %d\n", indexI, indexJ, upperGraph[indexJ].cr);  // debug uncomment

        } else {
            // If the first item is initialized it means this is not the first nonzero element of this row
            RowCol *next = &upperGraph[indexJ];

            while (true) {
                // Find the tail of the linked list...
                if (next->nextItem != NULL) {
                    next = next->nextItem;

                } else {
                    // ... and add the new item
                    RowCol *temp = (RowCol *) malloc(sizeof(RowCol));  // Allocate memory for the new item...
                    temp->cr = indexI;  // ...pass the column number...
                    temp->init = 1;  // ...declare the item initialized...
                    temp->nextItem = NULL;  // ...set the nextItem pointer to NULL as this is the new tail...
                    next->nextItem = temp;  // ...and finally set the "parent's" nextItem to point to the new tail

                    //printf("IndexI: %d, IndexJ: %d, cr: %d\n", indexI, indexJ, next->nextItem->cr);  // debug uncomment
                    break;
                }
            }
        }


        // With indexI and indexJ playing reverse role than before fill the lowerGraph as described above
        if (lowerGraph[indexI].init != 1) {

            lowerGraph[indexI].cr = indexJ;
            lowerGraph[indexI].init = 1;
            lowerGraph[indexI].nextItem = NULL;

            //printf("IndexI: %d, IndexJ: %d, cr: %d\n", indexI, indexJ, lowerGraph[indexI].cr);  // debug uncomment

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
                    //printf("IndexI: %d, IndexJ: %d, cr: %d\n", indexI, indexJ, next->nextItem->cr);  // debug uncomment
                    break;
                }
            }
        }
    }

    // Finalize the matrix that shows how many nz elements are there up to the ith row
    for (int i = 1; i <= matrix->size; i++) {
        matrix->IA[i] = matrix->IA[i] + matrix->IA[i - 1];
    }


    // Merge the two graphs to the CSR vectors and deallocate the previously allocated memory
    int aIndex = 0;
    RowCol *next;

    for (int i = 1; i < M; ++i) {

        next = &upperGraph[i - 1];
        while (next->init == 1) {
            matrix->JA[aIndex] = next->cr;
            aIndex++;

            if (next->nextItem == NULL) {
                break;
            } else {
                next = next->nextItem;
            }
        }

        next = &lowerGraph[i];
        while (next->init == 1) {
            matrix->JA[aIndex] = next->cr;
            aIndex++;

            if (next->nextItem == NULL) {
                break;
            } else {
                next = next->nextItem;
            }
        }
    }


    // Deallocate the linked list memory
    RowCol *temp;
    for (int i = 0; i < M; ++i) {
        next = upperGraph[i].nextItem;
        while (next != NULL) {
            temp = next->nextItem;
            free(next);
            next = temp;
        }

        next = lowerGraph[i].nextItem;
        while (next != NULL) {
            temp = next->nextItem;
            free(next);
            next = temp;
        }
    }

    // Free the remaining allocated memory.
    free(upperGraph);
    free(lowerGraph);

    fclose(f);

    return 0;
}

/**
 * Function for testing the creation of CSR matrices. There are two ways to get path:
 *      1. From the commandline arguments
 *      2. From the graph_path.txt file.
 *
 * If you want to use the file method to not provide any cmd line arguments and update the path of the file in the code.
 * The file must be a .txt file containing the absolute paths for the graph files (one at each line). Please note that
 * only the first 20 will be read and created. Read the code below for any adjustments.
 *
 * WARNING: Comment for release!!
 *
 * @param argc number of cmdline arguments
 * @param argv cmdline arguments
 * @return 0 if execution was successful
 */
//int main(int argc, char **argv) {
//    FILE *file = NULL;
//
//    // Holds the absolute paths for the .mtx files. The max length for each path is 256
//    char graphs[20][256];
//
//    // The CSR struct object that holds the resulted vectors and other information. See matrix_manipulation.h file
//    CSR testMatrix;
//
//    // Vars needed for execution time measurement
//    struct timeval begin, end;
//
//    // If no cmdline arguments are given read from the file
//    if (argc < 2) {
//
//        // Open the file
//        file = fopen(
//                "D:\\University\\AUTH\\Electrical_engineears\\7nth_semester\\Parallel_and_Distributed_Systems\\Assignment_1\\pds_assignment_1\\TriangleCalculator\\graph_paths.txt",
//                "r");
//
//        // If the file was found star reading
//        if (file != NULL) {
//            char buff[256];
//            int i;
//
//            // Get the first 20 lines
//            for (i = 0; i < 20; ++i) {
//                // If the EOF is not reached...
//                if (fgets(buff, 256, file) != NULL) {
//                    // ...remove new line char...
//                    buff[strcspn(buff, "\r")] = 0;
//                    // ...and add the path to the array
//                    strcpy(graphs[i], buff);
//
//                } else {
//                    // if EOF is reached fill the remaining array with "empty" string
//                    strcpy(graphs[i], "empty");
//                }
//            }
//
//            // close the file
//            fclose(file);
//
//            for (int j = 0; j < i; ++j) {
//                if (strcmp(graphs[j], "empty") == 0) {
//                    break;
//                }
//
//                printf("\n\nCreating Graph: %s\n", graphs[j]);
//
//                gettimeofday(&begin, 0);
//
//                createCSR(&testMatrix, graphs[j]);
//
//                gettimeofday(&end, 0);
//                long seconds = end.tv_sec - begin.tv_sec;
//                long microseconds = end.tv_usec - begin.tv_usec;
//                double elapsed = seconds + microseconds * 1e-6;
//
//                printf("Time for matrix creation: %.5f seconds.\n", elapsed);
//                printCSR(testMatrix, 20);
//
//                // debug create .csr files for the graph
//                char *name = graphs[j];
//                name[strlen(name) - 1] = 'r';
//                name[strlen(name) - 2] = 's';
//                name[strlen(name) - 3] = 'c';
//
//                writeToFile(name, &testMatrix);
//
//
//                free(testMatrix.IA);
//                free(testMatrix.JA);
//                free(testMatrix.A);
//            }
//
//        } else {
//            fprintf(stderr, "Usage: %s [martix-market-filename]\n", argv[0]);
//            exit(1);
//        }
//
//    } else {
//
//        gettimeofday(&begin, 0);
//
//        createCSR(&testMatrix, argv[1]);
//
//        gettimeofday(&end, 0);
//        long seconds = end.tv_sec - begin.tv_sec;
//        long microseconds = end.tv_usec - begin.tv_usec;
//        double elapsed = seconds + microseconds * 1e-6;
//
//        printf("Time for matrix creation: %.5f seconds.\n", elapsed);
//        printCSR(testMatrix, 20);
//
//        /* debug create .csr files for the graph
//        char *name = argv[1];
//        name[strlen(name) - 1] = 'r';
//        name[strlen(name) - 2] = 's';
//        name[strlen(name) - 3] = 'c';
//        writeToFile(name, &testMatrix);
//        */
//
//        free(testMatrix.IA);
//        free(testMatrix.JA);
//        free(testMatrix.A);
//    }
//
//    return 0;
//}
