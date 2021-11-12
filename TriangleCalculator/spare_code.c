/**
 * calculates the common elements of two arrays
 * @param array1
 * @param array2
 * @param size1 of array1
 * @param size2 of array2
 * @return the number of the commons
 */
int commonElements(int *array1, int *array2, int size1, int size2){
    int counter = 0;
    int start = 0;

    if(size1 <= size2) {
        for (int i = 0; i < size1; i++) {
            for (int j = start; j < size2; j++) {
                if (array1[i] == array2[j]) {
                    ++counter;
                    start = j + 1;
                    break;
                }
                if (array2[j] > array1[i]) {
                    start = j - 1;
                    break;
                }
            }
        }
    } else {
        for (int i = 0; i < size2; i++) {
            for (int j = start; j < size1; j++) {
                if (array1[j] == array2[i]) {
                    start = j + 1;
                    ++counter;
                    break;
                }
                if (array1[j] > array2[i]){
                    start = j - 1;
                    break;
                }
            }
        }
    }
    return counter;
}


/**
 * calcualtes csr-hadamard result
 * @return a structure that concludes the three vectors
 */
void product(CSR *input, CSR *output){
    int counter = 0;
    int nnzRows = 0;
    int nnzColumns = 0;

    output->A = (int*) malloc(input->nonzero * sizeof (int));
    output->JA = (int*) malloc(input->nonzero * sizeof (int));
    output->IA = (int*) calloc(input->size + 1, sizeof (int));
    output->nonzero = input->nonzero;
    output->size = input->size;

    for(int i = 0; i < input->size; i++) {
        nnzRows = input->IA[i + 1] - input->IA[i];  // find how many non-zero elements in i-th row
        int *holdTheColumns = (int *) malloc(nnzRows * sizeof(int));



        for (int k = 0; k < nnzRows; k++) {
            holdTheColumns[k] = input->JA[input->IA[i] + k];  // find the position of them
        }

        for (int j = 0; j < nnzRows; j++) {
            nnzColumns = input->IA[holdTheColumns[j] + 1] - input->IA[holdTheColumns[j]];
            int *holdTheRows = (int *) malloc(nnzColumns * sizeof(int));

            for (int k = 0; k < nnzColumns; k++) {
                holdTheRows[k] = input->JA[input->IA[holdTheColumns[j]] + k];  // find the position of them
            }
            output->A[counter] = commonElements(holdTheColumns, holdTheRows, nnzRows, nnzColumns);
            output->JA[counter] = holdTheColumns[j];
            output->IA[i + 1]++;
            ++counter;

            free(holdTheRows);
        }
        free(holdTheColumns);
    }
    for (int i = 1; i <= output->size; i++) {
        output->IA[i] = output->IA[i] + output->IA[i - 1];
    }
}


/* C = C + AB, where A is m x n, B is n x p, and C is m x p, in
 row-major order. Actually, the physical size of A, B, and C
 are m x fdA, n x fdB, and m x fdC, but only the first n/p/p
 columns are used, respectively. */
void add_matmul_rec(int *A, int *B, int *C, int m, int n, int p, int fdA, int fdB, int fdC){
    if (m+n+p <= 48) { /* <= 16x16 matrices "on average" */
        int i, j, k;
        for (i = 0; i < m; ++i)
            for (k = 0; k < p; ++k) {
                int sum = 0;
                for (j = 0; j < n; ++j)
                    sum += A[i*fdA +j] * B[j*fdB + k];
                C[i*fdC + k] += sum;
            }
    }
    else { /* divide and conquer */
        int m2 = m/2, n2 = n/2, p2 = p/2;

        add_matmul_rec(A, B, C, m2, n2, p2, fdA, fdB, fdC);
        add_matmul_rec(A+n2, B+n2*fdB, C, m2, n-n2, p2, fdA, fdB, fdC);

        add_matmul_rec(A, B+p2, C+p2, m2, n2, p-p2, fdA, fdB, fdC);
        add_matmul_rec(A+n2, B+p2+n2*fdB, C+p2, m2, n-n2, p-p2, fdA, fdB, fdC);

        add_matmul_rec(A+m2*fdA, B, C+m2*fdC, m-m2, n2, p2, fdA, fdB, fdC);
        add_matmul_rec(A+m2*fdA+n2, B+n2*fdB, C+m2*fdC, m-m2, n-n2, p2, fdA, fdB, fdC);

        add_matmul_rec(A+m2*fdA, B+p2, C+m2*fdC+p2, m-m2, n2, p-p2, fdA, fdB, fdC);
        add_matmul_rec(A+m2*fdA+n2, B+p2+n2*fdB, C+m2*fdC+p2, m-m2, n-n2, p-p2, fdA, fdB, fdC);
    }
}


void simpleMultiplication(int **first, int **second, int **multiply, int size){
    int sum = 0;

    for (int c = 0; c < size; c++) {
        for (int d = 0; d < size; d++) {
            for (int k = 0; k < size; k++) {
                sum = sum + first[c][k]*second[k][d];
            }

            multiply[c][d] = sum;
            sum = 0;
        }
    }
}





#define SIZE 2000
int main(){
    struct timeval begin, end;

    int **D = (int**) malloc(SIZE * sizeof (int*));
    for (int i = 0; i < SIZE; ++i) {
        D[i] = (int*) malloc(SIZE * sizeof (int));
    }

    int **E = (int**) malloc(SIZE * sizeof (int*));
    for (int i = 0; i < SIZE; ++i) {
        E[i] = (int*)calloc(SIZE, sizeof (int));
    }

    for (int i = 0; i < SIZE; ++i) {
        for (int j = 0; j < SIZE; ++j) {
            D[i][j] = 1;
        }
    }

    gettimeofday(&begin, 0);

//    simpleMultiplication(D, D, E, SIZE);

//    for (int c = 0; c < SIZE; c++) {
//        for (int d = 0; d < SIZE; d++)
//            printf("%d\t", E[c][d]);
//
//        printf("\n");
//    }

    gettimeofday(&end, 0);
    long seconds = end.tv_sec - begin.tv_sec;
    long microseconds = end.tv_usec - begin.tv_usec;
    double elapsed = seconds + microseconds * 1e-6;
    printf("Time for stupid multiplication: %.5f seconds.\n", elapsed);

    for (int i = 0; i < SIZE; ++i) {
        free(D[i]);
        free(E[i]);
    }

    free(D);
    free(E);

    int *A;
    int *C;

    A = (int *) malloc(SIZE * SIZE * sizeof (int));
    C = (int *) calloc(SIZE * SIZE, sizeof (int));
    for (int i = 0; i < SIZE * SIZE; ++i) {
        A[i] = 1;
    }

//    for (int i = 0; i < SIZE; i++) {
//        for (int j = 0; j < SIZE; ++j) {
//            printf("%d ", A[i * SIZE + j]);
//        }
//        printf("\n");
//    }

    gettimeofday(&begin, 0);

    CSR matrix;

    matrix.size = 5;
    matrix.nonzero = 14;
    matrix.JA = (int*) malloc(matrix.nonzero * sizeof(int));
    matrix.IA = (int*) malloc((matrix.size + 1) * sizeof(int));

    matrix.JA[0] = 1;
    matrix.JA[1] = 2;
    matrix.JA[2] = 3;
    matrix.JA[3] = 0;
    matrix.JA[4] = 3;
    matrix.JA[5] = 0;
    matrix.JA[6] = 3;
    matrix.JA[7] = 4;
    matrix.JA[8] = 0;
    matrix.JA[9] = 1;
    matrix.JA[10] = 2;
    matrix.JA[11] = 4;
    matrix.JA[12] = 2;
    matrix.JA[13] = 3;

    matrix.IA[0] = 0;
    matrix.IA[1] = 3;
    matrix.IA[2] = 5;
    matrix.IA[3] = 8;
    matrix.IA[4] = 12;
    matrix.IA[5] = 14;


    CSR result;

    result.size = 5;
    result.nonzero = 14;
    result.JA = (int*) calloc(matrix.nonzero, sizeof(int));
    result.IA = (int*) calloc(matrix.size + 1, sizeof(int));

    add_matmul_rec(A, A, C, SIZE, SIZE, SIZE, SIZE, SIZE, SIZE);

    gettimeofday(&end, 0);
    seconds = end.tv_sec - begin.tv_sec;
    microseconds = end.tv_usec - begin.tv_usec;
    elapsed = seconds + microseconds * 1e-6;
    printf("Time for good multiplication: %.5f seconds.\n", elapsed);

//    for (int i = 0; i < SIZE; i++) {
//        for (int j = 0; j < SIZE; ++j) {
//            printf("%d ", C[i * SIZE + j]);
//        }
//        printf("\n");
//    }

    free(A);
    free(C);


    return 0;
}

// serial main
{
gettimeofday(&begin, 0);

product(&input, &output);

gettimeofday(&end, 0);
seconds = end.tv_sec - begin.tv_sec;
microseconds = end.tv_usec - begin.tv_usec;
elapsed = seconds + microseconds * 1e-6;
printf("Time for triangle calculation (product): %.5f seconds.\n", elapsed);

printf("Number of triangles is: %d", measureTriangles(output));
}



// Parallel 1

void *colRowThread(void *args) {
    while(1) {
        Params param;

        if (queueEmpty && finished)
            break;

        pthread_mutex_lock(&mutexQueue);
        while (queueEmpty) {
//            printf("Waiting for task...\n");  // debug comment
            pthread_cond_wait(&condNotEmpty, &mutexQueue);
        }

//        printf("Consuming task...\n");  // debug comment
        param = paramQueue[queueEnd];
        if (queueEnd == TASK_SIZE - 1) {
            queueEnd = 0;
        } else {
            queueEnd++;
        }

        if (queueEnd == queueStart) {
            queueEmpty = true;
        }

        queueFull = false;

        pthread_mutex_unlock(&mutexQueue);
        pthread_cond_signal(&condNotFull);

        calculateColRowProduct(&param);
    }

    pthread_exit(NULL);
}

void productParallel(CSR *input, CSR *output, int numberOfThreads) {
    pthread_t *threads;
    pthread_attr_t pthread_custom_attr;
    int firstRow;
    int lastRow;
    int firstCol;
    int lastCol;
    int nnzInRow;
    int nnzInCol;

    threads = (pthread_t *) malloc(numberOfThreads * sizeof(pthread_t));
    pthread_attr_init(&pthread_custom_attr);
    pthread_mutex_init(&mutexQueue, NULL);
    pthread_mutex_init(&mutexOutput, NULL);
    pthread_cond_init(&condNotEmpty, NULL);
    pthread_cond_init(&condNotFull, NULL);

    printf("Creating threads...\n");
    for (int i = 0; i < numberOfThreads; i++) {
        if (pthread_create(&threads[i], NULL, colRowThread, NULL) != 0) {
            fprintf(stderr, "Could not create thread %d\n", i);
            free(threads);
            return;
        }
    }

    printf("Allocate output...\n");
    output->A = (int *) malloc(input->nonzero * sizeof(int));
    output->JA = (int *) malloc(input->nonzero * sizeof(int));
    output->IA = (int *) calloc(input->size + 1, sizeof(int));
    output->nonzero = input->nonzero;
    output->size = input->size;


    for (int row = 0; row < input->size; row++) {
        nnzInRow = input->IA[row + 1] - input->IA[row];  // find how many non-zero JA in i-th row
        firstRow = input->IA[row];
        lastRow = input->IA[row] + nnzInRow - 1;

        // We want from JA[input->IA[row]] to JA[input->IA[row] + nnzInRow]
        // JA[firstRow + colNumber] gives me the index of the colNumber th column
        for (int colNumber = 0; colNumber < nnzInRow; ++colNumber) {
            nnzInCol = input->IA[input->JA[firstRow + colNumber] + 1] - input->IA[input->JA[firstRow + colNumber]];
            firstCol = input->IA[input->JA[firstRow + colNumber]];
            lastCol = input->IA[input->JA[firstRow + colNumber]] + nnzInCol - 1;

            Params params = {
                    .JA = input->JA,
                    .rowStart = firstRow,
                    .rowEnd = lastRow,
                    .colStart = firstCol,
                    .colEnd = lastCol,
                    .columnNumber = colNumber,
                    .currentRow = row,
                    .output = output
            };

            pthread_mutex_lock(&mutexQueue);
            while (queueFull) {
                pthread_cond_wait(&condNotFull, &mutexQueue);
            }
//            printf("Creating task...\n");  // debug comment
            paramQueue[queueStart] = params;
            if(queueStart == TASK_SIZE - 1)
                queueStart = 0;
            else
                queueStart++;

            if (queueEnd == queueStart) {
                queueFull = true;
            }
            queueEmpty = false;
            pthread_mutex_unlock(&mutexQueue);
            pthread_cond_signal(&condNotEmpty);
        }
    }

    finished = true;

    while (!queueEmpty){
        usleep(1000);
    }

    printf("Signaling threads...\n");
    for (int i = 0; i <= numberOfThreads; ++i) {
        pthread_cond_signal(&condNotEmpty);
    }

//    for (int i = 0; i < numberOfThreads; i++) {
//        if (pthread_join(threads[i], NULL) != 0) {
//            fprintf(stderr, "Failed to join the thread %d\n", i);
//        }
//    }

    for (int i = 1; i <= output->size; i++) {
        output->IA[i] += output->IA[i - 1];
    }

    pthread_mutex_destroy(&mutexQueue);
    pthread_mutex_destroy(&mutexOutput);
    pthread_cond_destroy(&condNotFull);
    pthread_cond_destroy(&condNotEmpty);
}



// ################## parallel 2

void *colRowThreadImproved(void *args) {
    while(1) {
        Params param;

        if (queueEmpty && finished)
            break;

        pthread_mutex_lock(&mutexQueue);
        while (queueEmpty) {
//            printf("Waiting for task...\n");  // debug comment
            pthread_cond_wait(&condNotEmpty, &mutexQueue);
        }

//        printf("Consuming task...\n");  // debug comment
        param = paramQueue[queueEnd];
        if (queueEnd == TASK_SIZE - 1) {
            queueEnd = 0;
        } else {
            queueEnd++;
        }

        if (queueEnd == queueStart) {
            queueEmpty = true;
        }

        queueFull = false;

        pthread_mutex_unlock(&mutexQueue);
        pthread_cond_signal(&condNotFull);

        int nnzInCol;
        int firstCol;
        int lastCol;

        // We want from JA[input->IA[row]] to JA[input->IA[row] + nnzInRow]
        // JA[firstRow + colNumber] gives me the index of the colNumber th column
        for (int colNumber = 0; colNumber < param.nnzInRow; ++colNumber) {
            nnzInCol = param.IA[param.JA[param.rowStart + colNumber] + 1] - param.IA[param.JA[param.rowStart + colNumber]];
            firstCol = param.IA[param.JA[param.rowStart + colNumber]];
            lastCol = param.IA[param.JA[param.rowStart + colNumber]] + nnzInCol - 1;

            param.columnNumber = colNumber;
            param.colStart = firstCol;
            param.colEnd = lastCol;

            calculateColRowProduct(&param);
        }
    }

    pthread_exit(NULL);
}

void productParallelImproved(CSR *input, CSR *output, int numberOfThreads){
    pthread_t *threads;
    pthread_attr_t pthread_custom_attr;
    int firstRow;
    int lastRow;
    int nnzInRow;


    threads = (pthread_t *) malloc(numberOfThreads * sizeof(pthread_t));
    pthread_attr_init(&pthread_custom_attr);
    pthread_mutex_init(&mutexQueue, NULL);
    pthread_mutex_init(&mutexOutput, NULL);
    pthread_cond_init(&condNotEmpty, NULL);
    pthread_cond_init(&condNotFull, NULL);

    printf("Creating threads...\n");
    for (int i = 0; i < numberOfThreads; i++) {
        if (pthread_create(&threads[i], NULL, colRowThreadImproved, NULL) != 0) {
            fprintf(stderr, "Could not create thread %d\n", i);
            free(threads);
            return;
        }
    }

    printf("Allocate output...\n");
    output->A = (int *) malloc(input->nonzero * sizeof(int));
    output->JA = (int *) malloc(input->nonzero * sizeof(int));
    output->IA = (int *) calloc(input->size + 1, sizeof(int));
    output->nonzero = input->nonzero;
    output->size = input->size;


    for (int row = 0; row < input->size; row++) {
        nnzInRow = input->IA[row + 1] - input->IA[row];  // find how many non-zero JA in i-th row
        firstRow = input->IA[row];
        lastRow = input->IA[row] + nnzInRow - 1;

        Params params = {
                .JA = input->JA,
                .IA = input->IA,
                .rowStart = firstRow,
                .rowEnd = lastRow,
                .currentRow = row,
                .output = output,
                .nnzInRow = nnzInRow
        };

        pthread_mutex_lock(&mutexQueue);
        while (queueFull) {
            pthread_cond_wait(&condNotFull, &mutexQueue);
        }
//            printf("Creating task...\n");  // debug comment
        paramQueue[queueStart] = params;
        if(queueStart == TASK_SIZE - 1)
            queueStart = 0;
        else
            queueStart++;

        if (queueEnd == queueStart) {
            queueFull = true;
        }
        queueEmpty = false;
        pthread_mutex_unlock(&mutexQueue);
        pthread_cond_signal(&condNotEmpty);
    }

    finished = true;

    while (!queueEmpty){
        usleep(1000);
    }

    printf("Signaling threads...\n");
    for (int i = 0; i <= numberOfThreads; ++i) {
        pthread_cond_signal(&condNotEmpty);
    }

//    for (int i = 0; i < numberOfThreads; i++) {
//        if (pthread_join(threads[i], NULL) != 0) {
//            fprintf(stderr, "Failed to join the thread %d\n", i);
//        }
//    }

    for (int i = 1; i <= output->size; i++) {
        output->IA[i] += output->IA[i - 1];
    }

    pthread_mutex_destroy(&mutexQueue);
    pthread_mutex_destroy(&mutexOutput);
    pthread_cond_destroy(&condNotFull);
    pthread_cond_destroy(&condNotEmpty);
}

// ######## noclue

void calculateColRowProduct(Params *params){
    int res = 0;
    int offset = 0;

    if (params->colEnd - params->colStart > params->colEnd - params->rowStart) {
        for (int i = 0; i <= params->rowEnd - params->rowStart; ++i) {
            if (offset > params->colEnd - params->colStart)
                break;


            if (params->JA[params->rowStart + i] < params->JA[params->colStart + offset]) {
                continue;

            } else if (params->JA[params->rowStart + i] > params->JA[params->colStart + offset]) {
                i--;
                offset++;
                continue;
            } else {
                res++;
                offset++;
                continue;
            }
        }
    } else {
        for (int i = 0; i <= params->colEnd - params->colStart; ++i) {
            if (offset > params->rowEnd - params->rowStart)
                break;

            if (params->JA[params->rowStart + offset] < params->JA[params->colStart + i]) {
                i--;
                offset++;
                continue;

            } else if (params->JA[params->rowStart + offset] > params->JA[params->colStart + i]) {
                continue;
            } else {
                res++;
                offset++;
                continue;
            }
        }
    }

    params->output->A[params->rowStart + params->columnNumber] = res;
    params->output->JA[params->rowStart + params->columnNumber] = params->JA[params->rowStart +
                                                                             params->columnNumber];

//    pthread_mutex_lock(&mutexOutput);
//    params->output->IA[params->currentRow + 1]++;
//    pthread_mutex_unlock(&mutexOutput);
}

#define TASK_SIZE 256

Params paramQueue[TASK_SIZE];
bool queueFull = false;
bool queueEmpty = true;
int queueStart, queueEnd = 0;


pthread_mutex_t mutexQueue, mutexOutput;
pthread_cond_t condNotEmpty;
pthread_cond_t condNotFull;

bool finished = false;  // no tasks left