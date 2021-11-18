#include <stdio.h>
#include <sys/time.h>
#include <stdlib.h>
#include <string.h>

#include "csr_matrix.h"
#include "matrix_manipulation.h"
#include "openmp_parallel.h"
#include "pthreads_parallel.h"
#include "serial_mult.h"

#define REPS 2

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
    int nthreads  = atoi(argv[2]);
    printf("Number of threads: %d\n\n", nthreads);

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
         serialProduct(&input, &output);
         gettimeofday(&end, 0);

         serialTimes[i] = measureTime(begin, end);

//         printf("Time for %d serial triangle calculation: %.5f seconds.\n", i, serialTimes[i]);
//         printf("Number of triangles (serial) is: %d\n", output.triangles);

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


    // =========================================== Pthread Test ===========================================
    double **pthreadTimes;
    pthreadTimes = (double**)malloc(nthreads/4 * sizeof (double *));
    for (int i = 0; i < nthreads/4; ++i) {
        pthreadTimes[i] = (double *)malloc(REPS * sizeof (double));
    }

    int indexI = 0;
    for (int i = 4; i <= nthreads; i+=4) {
        for (int j = 0; j < REPS; ++j) {
            // Allocate memory for the output vectors
            output.A = (int *) malloc(input.nonzero * sizeof(int));
            output.JA = (int *) malloc(input.nonzero * sizeof(int));
            output.IA = (int *) calloc(input.size + 1, sizeof(int));
            output.nonzero = input.nonzero;
            output.size = input.size;
            output.triangles = 0;

            gettimeofday(&begin, 0);
            pthreadProduct(&input, &output, i - 2);
            gettimeofday(&end, 0);

            pthreadTimes[indexI][j] = measureTime(begin, end);

//            printf("NThreads: %d. Time for %d time triangle calculation (pthread): %.5f seconds.\n", i, j, pthreadTimes[indexI][j]);
//            printf("Number of triangles (pthread) is: %d\n", output.triangles);

            // Reset the output matrix
            free(output.A);
            free(output.JA);
            free(output.IA);
        }
        indexI++;
    }

    // TODO update python project
    fprintf(file, "Pthread_times:\n");

    for (int i = 0; i < nthreads/4; ++i) {
        fprintf(file, "Threads %d: ", 4 * i + 2);
        for (int j = 0; j < REPS; ++j) {
            fprintf(file, "%.5f ", pthreadTimes[i][j]);
        }
        fprintf(file, "\n");
    }
    fprintf(file, "\n");


    // =========================================== OpenMP Test ===========================================

    double **openMPTimes;

    openMPTimes = (double**)malloc(nthreads/4 * sizeof (double *));
    for (int i = 0; i < nthreads/4; ++i) {
        openMPTimes[i] = (double *)malloc(REPS * sizeof (double));
    }

    indexI = 0;
    for (int i = 4; i <= nthreads; i+=4) {
        for (int j = 0; j < REPS; ++j) {
            // Allocate memory for the output vectors
            output.A = (int *) malloc(input.nonzero * sizeof(int));
            output.JA = (int *) malloc(input.nonzero * sizeof(int));
            output.IA = (int *) calloc(input.size + 1, sizeof(int));
            output.nonzero = input.nonzero;
            output.size = input.size;
            output.triangles = 0;

            gettimeofday(&begin, 0);
            openmpProduct(&input, &output, i - 2);
            gettimeofday(&end, 0);

            openMPTimes[indexI][j] = measureTime(begin, end);

//            printf("NThreads: %d. Time for %d time triangle calculation (openMP): %.5f seconds.\n", i, j, openMPTimes[indexI][j]);
//            printf("Number of triangles (openMP) is: %d\n", output.triangles);

            // Reset the output matrix
            free(output.A);
            free(output.JA);
            free(output.IA);
        }
        indexI++;
    }

    fprintf(file, "Openmp_times:\n");

    for (int i = 0; i < nthreads/4; ++i) {
        fprintf(file, "Threads %d: ", 4 * i + 2);
        for (int j = 0; j < REPS; ++j) {
            fprintf(file, "%.5f ", openMPTimes[i][j]);
        }
        fprintf(file, "\n");
    }
    fprintf(file, "\n");

    fclose(file);

    for (int i = 0; i < nthreads - 1; ++i) {
        free(openMPTimes[i]);
        free(pthreadTimes[i]);
    }

    free(openMPTimes);
   free(pthreadTimes);

    return 0;
}