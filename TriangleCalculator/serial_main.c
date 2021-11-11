#include <stdio.h>
#include <sys/time.h>
#include <stdlib.h>
#include "csr_matrix.h"
#include "matrix_manipulation.h"
#include "write_csr_to_file.h"


int main(int argc, char **argv) {
    CSR input;
    CSR output;

    // Vars needed for execution time measurement
    struct timeval begin, end;

    gettimeofday(&begin, 0);

    createCSR(&input, argv[1]);

    gettimeofday(&end, 0);
    long seconds = end.tv_sec - begin.tv_sec;
    long microseconds = end.tv_usec - begin.tv_usec;
    double elapsed = seconds + microseconds * 1e-6;
    printf("Time for matrix creation: %.5f seconds.\n", elapsed);

    //printCSR(input, 50);  // debug comment

    gettimeofday(&begin, 0);

    productImproved(&input, &output);

    gettimeofday(&end, 0);
    seconds = end.tv_sec - begin.tv_sec;
    microseconds = end.tv_usec - begin.tv_usec;
    elapsed = seconds + microseconds * 1e-6;
    printf("Time for triangle calculation (product improved): %.5f seconds.\n", elapsed);

    printCSR(output, 10);  // debug comment

    writeToFile("youtube.csr", &output);

    printf("Number of triangles is: %d\n", measureTriangles(output));

    return 0;
}


