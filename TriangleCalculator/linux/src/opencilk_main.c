#include <stdio.h>
#include <sys/time.h>
#include <stdlib.h>
#include <string.h>

#include "csr_matrix.h"
#include "cilk_parallel.h"


/**
 * Calculates the elapse time
 *
 * @param begin the starting timestamp
 * @param end the ending timestamp
 * @return elapsed time in seconds
 */
double measureTime(struct timeval begin, struct timeval end){
    long seconds;
    long microseconds;
    double elapsed;

    seconds = end.tv_sec - begin.tv_sec;
    microseconds = end.tv_usec - begin.tv_usec;
    elapsed = seconds + microseconds * 1e-6;

    return elapsed;
}


/**
 * Runs the OpenCilk implementation of the algorithm. Function takes two arguments the path to the .mtx file and the number of threads. 
 * 
 * @param argc Number of argumnets
 * @param argv Arguments
 * @return 0 if execution was successful 
 */
int main(int argc, char **argv) {

    if (argc != 3) {
        fprintf(stderr, "Wrong number of arguments\n");
        exit(1);
    }
    int nthreads  = atoi(argv[2]);

    // Vars needed for execution time measurement
    struct timeval begin, end;

    CSR input;  /// The input CSR matrix
    CSR output;  /// The result of A .* (A * A) in CSR format


    // Create the CSR matrix from the file path on argv[1]
    gettimeofday(&begin, 0);
    createCSR(&input, argv[1]);
    gettimeofday(&end, 0);

    printf("Time for matrix read: %.5f seconds.\n", measureTime(begin, end));

    //printCSR(input, 50);  // debug comment


    
    // Allocate memory for the output vectors
    output.A = (int *) malloc(input.nonzero * sizeof(int));
    output.JA = (int *) malloc(input.nonzero * sizeof(int));
    output.IA = (int *) calloc(input.size + 1, sizeof(int));
    output.nonzero = input.nonzero;
    output.size = input.size;
    output.triangles = 0;

    gettimeofday(&begin, 0);
    cilkProduct(&input, &output, nthreads);
    gettimeofday(&end, 0);

    printf("NThreads: %d. Time for triangle calculation (OpenCilk): %.5f seconds.\n", nthreads, measureTime(begin, end));
    printf("Number of triangles (OpenCilk algorithm) is: %d\n", output.triangles);

    // Free the output matrix
    free(output.A);
    free(output.JA);
    free(output.IA);
        

    return 0;
}


