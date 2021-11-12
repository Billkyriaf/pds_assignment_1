#include <stdio.h>
#include <stdlib.h>

void read_words (FILE *f) {
    char x[1024];
    int number;
    /* assumes no word exceeds length of 1023 */
    while (fscanf(f, " %1023s", x) == 1) {
        number = atoi(x);
        printf("%d", number);
    }
}

int main(int argc, char **argv){
    FILE *file;

    if (argc != 2){
        fprintf(stderr, "Wrong number of arguments\n");

    } else {
        file = fopen(argv[1], "r");

        read_words(file);
    }

    return 0;
}

