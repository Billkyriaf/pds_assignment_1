#include <stdio.h>
#include <stdlib.h>
#include "mmio.h"


/*void createSparseArray(int *I, int *J, int m, int n, int nz){
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
}*/


int main(int argc, char *argv[]){
    MM_typecode matrixCode;
    FILE *f;
    int M, N, nz;
    int i, *I, *J;

    if (argc < 2){
        fprintf(stderr, "Usage: %s [martix-market-filename]\n", argv[0]);
        exit(1);
    }
    else{
        if ((f = fopen(argv[1], "r")) == NULL){
            fprintf(stderr, "File %s not found\n", argv[1]);
            exit(1);
        }
    }

    if (mm_read_banner(f, &matrixCode) != 0){
        printf("Could not process Matrix Market banner.\n");
        exit(1);
    }

    /*  This is how one can screen matrix types if their application */
    /*  only supports a subset of the Matrix Market data types.      */
    if (mm_is_complex(matrixCode) && mm_is_matrix(matrixCode) && mm_is_sparse(matrixCode) ){
        printf("Sorry, this application does not support ");
        printf("Market Market type: [%s]\n", mm_typecode_to_str(matrixCode));
        exit(1);
    }

    /* find out size of sparse matrix .... */
    if ((mm_read_mtx_crd_size(f, &M, &N, &nz)) !=0)
        exit(1);

    /* reserve memory for matrices */
    I = (int *) malloc(nz * sizeof(int));
    J = (int *) malloc(nz * sizeof(int));


    /* NOTE: when reading in doubles, ANSI C requires the use of the "l"  */
    /*   specifier as in "%lg", "%lf", "%le", otherwise errors will occur */
    /*  (ANSI C X3.159-1989, Sec. 4.9.6.2, p. 136 lines 13-15)            */

    for (i=0; i<nz; i++){
        fscanf(f, "%d %d\n", &I[i], &J[i]);
        I[i]--;  /* adjust from 1-based to 0-based */
        J[i]--;
    }

    if (f !=stdin)
        fclose(f);

    /* now write out the matrix */
    mm_write_banner(stdout, matrixCode);
    mm_write_mtx_crd_size(stdout, M, N, nz);
    for (i=0; i<nz; i++)
        fprintf(stdout, "%d %d\n", I[i]+1, J[i]+1);

    //createSparseArray(I, J, M, N, nz);

    return 0;
}
