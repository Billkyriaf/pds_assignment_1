#include <stdio.h>
#include "write_csr_to_file.h"


void writeToFile(char *filename, CSR *matrix){
    FILE *file = NULL;

    printf("File name %s", filename);

    file = fopen(filename, "w");

    if (file == NULL){
        fprintf(stderr, "File %s could not be created", filename);
    } else{
        fprintf(stdout, "File %s created", filename);
    }

    // Write the header of the file
    fprintf(file, "##CSR File format\n");
    fprintf(file, "%d %d\n", matrix->size, matrix->nonzero);

    // Write the array IA
    for (int i = 0; i <= matrix->size; ++i) {
        fprintf(file, "%d ", matrix->A[i]);
    }
    fprintf(file, "\n");

    // Write the array JA
    for (int i = 0; i < matrix->nonzero; ++i) {
        fprintf(file, "%d ", matrix->JA[i] + 1);
    }
    fprintf(file, "\n");

    // Write the array IA
    for (int i = 0; i <= matrix->size; ++i) {
        fprintf(file, "%d ", matrix->IA[i]);
    }
    fprintf(file, "\n");

    fclose(file);
}