#include <stdlib.h>
#include <pthread.h>
#include <stdio.h>
#include "pthreads_parallel.h"

/**
 * Arguments for the threads
 */
typedef struct args {
    CSR *input;
    CSR *output;
    int start;  // Starting row
    int stop;  // Ending row (inclusive)
    int counted;  // The partial (per thread) triangles count
} ARGS;


/**
 * The runnable function that every thread runs. The work of every thread is independent from all the other threads so
 * that no blocking between them occurs and slows them down.
 *
 * The work load for every thread is assigned in the pthreadProduct() (see more info there)
 *
 * @param args  Void pointer of the ARGS struct object
 * @return  void
 */
void *runnableProduct(void* args){
    ARGS *arguments = (ARGS *) args;

    int rowStart;  // The starting index of the row
    int rowEnd;  // The ending index of the row
    int colStart;  // The starting index of the column
    int colEnd;  // The ending index of the column
    int nnzInRow;  // The number of the nonzero elements in the row
    int nnzInCol;  // The number of the nonzero elements in the column
    int res;

    for (int row = arguments->start; row <= arguments->stop; row++) {
        nnzInRow = arguments->input->IA[row + 1] - arguments->input->IA[row];  // find how many non-zero JA in i-th row
        rowStart = arguments->input->IA[row];  // The starting index of the row in the JA vector
        rowEnd = rowStart + nnzInRow - 1;  // The ending index of the row in the JA vector

        // We want the elements from JA[input->IA[row]] to JA[input->IA[row] + nnzInRow]
        // JA[rowStart + colNumber] gives the index of the colNumber th column
        // So for every column found in the row...
        for (int col = rowStart; col <= rowEnd; ++col) {
            // ...find how many nonzero elements in ith col column from the IA vector
            nnzInCol = arguments->input->IA[arguments->input->JA[col] + 1] - arguments->input->IA[arguments->input->JA[col]];
            colStart = arguments->input->IA[arguments->input->JA[col]];  // The starting index of the column in the JA vector
            colEnd = colStart + nnzInCol - 1;  // The ending index of the column in the JA vector

            // Find the common elements in the row and the column and pass the values to the output JA vector
            res = colRowProduct(arguments->input->JA, colStart, colEnd, rowStart, rowEnd);
            arguments->counted += res;  // Count triangles as they are calculated
            arguments->output->A[col] = res;
            arguments->output->JA[col] = arguments->input->JA[col];
        }
    }

    //printf("Thread exiting...\n");  // debug comment
    pthread_exit(NULL);
}


/**
 * This functions initializes the threads and their parameters distributes the work load depending on the number of
 * threads and waits for all the threads to finish execution before exiting.
 *
 * Every thread is responsible for calculating the product for a range of rows in the matrix. It is done in this way so,
 * all the threads can be independent. The result is that no locking is needed and that benefits performance greatly
 *
 * @param input   The CSR object of the input matrix
 * @param output  The CSR object for the resulting matrix (must be initialized prior to calling this function)
 * @param numberOfThreads   The number of threads to divide the work load
 */
void pthreadProduct(CSR *input, CSR *output, int numberOfThreads){

    // Initialize the thread attributes
    pthread_attr_t pthread_custom_attr;
    pthread_attr_init(&pthread_custom_attr);

    // If the requested matrix is too small we calculate it with only one thread
    if (input->size < numberOfThreads){

        // Initialize the arguments
        ARGS argument = {
                .input = input,
                .output = output,
                .start = 0,
                .stop = input->size - 1,
                .counted = 0
        };

        // Create start and join the thread
        pthread_t thread;
        pthread_create(&thread, &pthread_custom_attr, runnableProduct, (void *) &argument);
        pthread_join(thread, NULL);

        input->triangles = argument.counted / 6;
        output->triangles = argument.counted / 6;

    } else { // else if the matrix is big enough...

        pthread_t *threads;  // Array of all the threads
        threads = (pthread_t *)malloc(numberOfThreads * sizeof(pthread_t));

        ARGS *args = (ARGS *) malloc(numberOfThreads * sizeof (ARGS));  // Array of the arguments of every thread

        int blockSize = input->size/numberOfThreads;  // Find how many rows each thread will compute

        // Create all the threads with a given range of rows
        for (int i = 1; i <= numberOfThreads; ++i) {
            args[i - 1].input = input;
            args[i - 1].output = output;
            args[i - 1].start = blockSize * i - blockSize;
            args[i - 1].counted = 0;

            if (i == numberOfThreads)
                args[i - 1].stop = input->size - 1;

            else
                args[i - 1].stop = args[i - 1].start + blockSize - 1;

            // Create the thread
            pthread_create(&threads[i - 1], &pthread_custom_attr, runnableProduct, (void *) &args[i - 1]);
        }

        // Join all the threads
        for (int i = 0; i < numberOfThreads; ++i) {
            pthread_join(threads[i], NULL);
        }

        int triangles = 0;
        // Sum the triangles form all the threads
        for (int i = 0; i < numberOfThreads; ++i) {
            triangles += args[i].counted;
        }

        output->triangles = triangles/6;
        input->triangles = output->triangles;

        // After execution free the allocated memory
        free(threads);
        free(args);
    }

    // IA vector is the same, so we just copy the values. Not really ideal for performance but it only happens once and
    // is very simple. We want input and output to be independent that's why we don't just point output->IA to the same
    // memory address as the input->IA
    for (int i = 1; i <= output->size; i++) {
        output->IA[i] = input->IA[i];
    }

    // Delete the attributes
    pthread_attr_destroy(&pthread_custom_attr);
}
