#include <stdio.h>
#include <sys/time.h>
#include <stdlib.h>
#include <string.h>
#include "csr_matrix.h"
#include "matrix_manipulation.h"
#include "m_man_openmp.h"

#define REPS 10

/**
 * Calculates the elapse time
 *
 * @param begin the starting timestamp
 * @param end the ending timestamp
 * @return elapsed time in seconds
 */
double measureTime(struct timeval begin, struct timeval end) {
    long seconds;
    long microseconds;
    double elapsed;

    seconds = end.tv_sec - begin.tv_sec;
    microseconds = end.tv_usec - begin.tv_usec;
    elapsed = seconds + microseconds * 1e-6;

    return elapsed;
}


int main(int argc, char **argv) {
    if (argc != 3) {
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

    FILE *file = NULL;

    char *name = argv[1];
    name[strlen(name) - 1] = 't';
    name[strlen(name) - 2] = 'x';
    name[strlen(name) - 3] = 't';

    file = fopen(name, "w");
    if (file == NULL){
        fprintf(stderr, "Could not open file\n");
        exit(1);
    }

    // ============================================== Serial Test ==============================================
    double serialTimes[REPS] = {0};

    for (int i = 0; i < REPS; ++i) {
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

        serialTimes[i] = measureTime(begin, end);

        printf("Time for %d serial triangle calculation: %.5f seconds.\n", i, serialTimes[i]);
        printf("Number of triangles (serial) is: %d\n", output.triangles);

        // Reset the output matrix
        free(output.A);
        free(output.JA);
        free(output.IA);
    }

    fprintf(file, "Serial_times:\n");
    for (int i = 0; i < REPS; ++i) {
        fprintf(file, "%.5f ", serialTimes[i]);
    }

    fprintf(file, "\n");

    if (1){
        fclose(file);

        free(output.A);
        free(output.JA);
        free(output.IA);
        return 0;
    }

    double **parallelTimes;

    parallelTimes = (double**)malloc((atoi(argv[2]) - 1) * sizeof (double *));
    for (int i = 0; i < atoi(argv[2]); ++i) {
        parallelTimes[i] = (double *)malloc(REPS * sizeof (double));
    }

    for (int i = 2; i <= atoi(argv[2]); i++) {
        for (int j = 0; j < REPS; ++j) {
            // Allocate memory for the output vectors
            output.A = (int *) malloc(input.nonzero * sizeof(int));
            output.JA = (int *) malloc(input.nonzero * sizeof(int));
            output.IA = (int *) calloc(input.size + 1, sizeof(int));
            output.nonzero = input.nonzero;
            output.size = input.size;
            output.triangles = 0;

            gettimeofday(&begin, 0);
            productParallel(&input, &output, i);
            gettimeofday(&end, 0);

            parallelTimes[i - 2][j] = measureTime(begin, end);

//            printf("NThreads: %d. Time for %d time triangle calculation (parallel): %.5f seconds.\n", i, j, parallelTimes[i - 2][j]);
//            printf("Number of triangles (parallel) is: %d\n", output.triangles);

            // Reset the output matrix
            free(output.A);
            free(output.JA);
            free(output.IA);
        }
    }

    fprintf(file, "Parallel_times:\n");

    for (int i = 0; i < atoi(argv[2]) - 1; ++i) {
        fprintf(file, "Threads %d: ", i + 2);
        for (int j = 0; j < REPS; ++j) {
            fprintf(file, "%.5f ", parallelTimes[i][j]);
        }
        fprintf(file, "\n");
    }
    fprintf(file, "\n");


    double **openMPTimes;

    openMPTimes = (double**)malloc((atoi(argv[2]) - 1) * sizeof (double *));
    for (int i = 0; i < atoi(argv[2]); ++i) {
        openMPTimes[i] = (double *)malloc(REPS * sizeof (double));
    }

    for (int i = 2; i <= atoi(argv[2]); i++) {
        for (int j = 0; j < REPS; ++j) {
            // Allocate memory for the output vectors
            output.A = (int *) malloc(input.nonzero * sizeof(int));
            output.JA = (int *) malloc(input.nonzero * sizeof(int));
            output.IA = (int *) calloc(input.size + 1, sizeof(int));
            output.nonzero = input.nonzero;
            output.size = input.size;
            output.triangles = 0;

            gettimeofday(&begin, 0);
            productOpenmp(&input, &output, i);
            gettimeofday(&end, 0);

            openMPTimes[i - 2][j] = measureTime(begin, end);

//            printf("NThreads: %d. Time for %d time triangle calculation (openMP): %.5f seconds.\n", i, j, parallelTimes[i - 2][j]);
//            printf("Number of triangles (openMP) is: %d\n", output.triangles);

            // Reset the output matrix
            free(output.A);
            free(output.JA);
            free(output.IA);
        }
    }

    fprintf(file, "Openmp_times:\n");

    for (int i = 0; i < atoi(argv[2]) - 1; ++i) {
        fprintf(file, "Threads %d: ", i + 2);
        for (int j = 0; j < REPS; ++j) {
            fprintf(file, "%.5f ", openMPTimes[i][j]);
        }
        fprintf(file, "\n");
    }
    fprintf(file, "\n");

    fclose(file);

    for (int i = 0; i < atoi(argv[2]) - 1; ++i) {
        free(openMPTimes[i]);
        free(parallelTimes[i]);
    }

    free(openMPTimes);
    free(parallelTimes);
    free(output.A);
    free(output.JA);
    free(output.IA);

    return 0;
}