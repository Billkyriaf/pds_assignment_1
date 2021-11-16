#include <stdio.h>
#include <sys/time.h>
#include <stdlib.h>
#include "csr_matrix.h"
#include "matrix_manipulation.h"
#include "m_man_openmp.h"


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
 * Main function runs the load function creates a graph and the calculates the number of triangles both with the serial
 * algorithm and the parallel algorithm. Finally displays the execution time and the result for every algorithm
 *
 *
 * @param argc  number of arguments
 * @param argv  path to file, number of threads
 * @return  0 if everything was ok
 */
int main(int argc, char **argv) {

    if (argc != 3){
        fprintf(stderr, "Wrong number of arguments\n");
        exit(1);
    }

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



    // Calculate the product A .* (A * A) with the serial algorithm
    gettimeofday(&begin, 0);
    productSerial(&input, &output);
    gettimeofday(&end, 0);

    printf("Time for serial triangle calculation: %.5f seconds.\n", measureTime(begin, end));

    //printCSR(output, 10);  // debug comment

    printf("Number of triangles (serial) is: %d\n", output.triangles);

    // Reset the output matrix
    free(output.A);
    free(output.JA);
    free(output.IA);

    // Allocate new memory
    output.A = (int *) malloc(input.nonzero * sizeof(int));
    output.JA = (int *) malloc(input.nonzero * sizeof(int));
    output.IA = (int *) calloc(input.size + 1, sizeof(int));
    output.triangles = 0;

    gettimeofday(&begin, 0);
    productParallel(&input, &output, atoi(argv[2]));
    gettimeofday(&end, 0);

    printf("Time for triangle calculation (parallel): %.5f seconds.\n", measureTime(begin, end));

    //printCSR(output, 10);  // debug comment

    printf("Number of triangles (parallel) is: %d\n", output.triangles);

    // Reset the output matrix
    free(output.A);
    free(output.JA);
    free(output.IA);

    // Allocate new memory
    output.A = (int *) malloc(input.nonzero * sizeof(int));
    output.JA = (int *) malloc(input.nonzero * sizeof(int));
    output.IA = (int *) calloc(input.size + 1, sizeof(int));
    output.triangles = 0;

    gettimeofday(&begin, 0);
    productOpenmp(&input, &output, atoi(argv[2]));
    gettimeofday(&end, 0);

    printf("Time for triangle calculation (openmp): %.5f seconds.\n", measureTime(begin, end));

    //printCSR(output, 10);  // debug comment

    printf("Number of triangles (openmp) is: %d\n", output.triangles);

    return 0;
}


