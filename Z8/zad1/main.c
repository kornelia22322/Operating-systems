#include <stdio.h>
#include <stdlib.h>
#include <string.h>

int numberOfThreads;
int H, W, C, M, P;
unsigned char **I; //picture
float **K; //filter

pthread_t *threads; //threads array

void load_picture(char *file_path) {
    FILE *file;
    if ((file = fopen(file_path, "r")) == NULL) {
        printf("Opening input file failed");
        exit(2);
    }

    printf("I am here");
    fscanf(file, "P%d\n", &P);
//    printf("%d\n", P);
    fscanf(file, "%d", &W);
    fscanf(file, "%d", &H);
    fscanf(file, "%d", &M);
    I = (unsigned char**) malloc(W * sizeof(unsigned char **));
    for (int i = 0; i < W; ++i)
        I[i] = (unsigned char*) malloc(H * sizeof(unsigned char));

    for (int i = 0; i < W; ++i)
        for (int j = 0; j < H; ++j)
            fscanf(file, "%d", &I[i][j]);


    for (int i = 0; i < W; ++i) {
        for (int j = 0; j < H; ++j) {
            printf("%d ", I[i][j]);
        }
        printf("\n");
    }
    fclose(file);
}

void load_filter(char *file_path) {
    FILE *file;
    file = fopen(file_path, "r");
    fscanf(file, "%d", &C);
    K = (float**) malloc(C * sizeof(float **));
    for (int i = 0; i < C; ++i)
        K[i] = (float*) malloc(C * sizeof(float *));

    for (int i = 0; i < C; ++i)
        for (int j = 0; j < C; ++j)
            fscanf(file, "%f", &K[i][j]);

    fclose(file);
}

/*
char** readPGMFileToArray(FILE* file) {
    int lineNumber = 0;
    char line[256];
    while (fgets(line, sizeof line, file) != NULL && lineNumber != 1) {
        lineNumber++;
    }
    char * tmp;
    tmp = strtok (line," ");
    int counter = 0;
    int maxCounter = 2;
    //output Array - 0: width (number of colums), 1: height (number of rows)
    int output[maxCounter];

    while (tmp != NULL && counter != maxCounter) {
        output[counter] = (int) strtol(tmp, NULL, 10);
        tmp = strtok (NULL, " ");
        counter++;
    }
//    printf("%d\n", output[0]);
//    printf("%d\n", output[1]);

    char **imageArray = (char**) malloc(sizeof *imageArray * output[0]);
    if (imageArray) {
        for (int i = 0; i < output[1]; i++) {
            imageArray[i] = (char*) malloc(sizeof *imageArray[i] * output[1]);
        }
    }

    lineNumber = 0;
    int iterator = 0;
    char* newline = (char*) malloc(sizeof *newline * output[0]);
    while (fgets(newline, sizeof newline*output[0], file) != NULL) {
        if(lineNumber < 3) {
            lineNumber++;
            printf("%s\n", newline);
        } else {
            printf("%s\n", newline);
        }
    }


    for(int i = 0; i < output[0]; i++) {
        for(int j = 0; j < output[1]; j++) {
            printf("%s ", imageArray[i][j]);
        }
        printf("\n");
    }




    fclose(file);
    return NULL;

}

*/

//./a.out 10 "./mountain.pgm"
int main(int argc, char* argv[]) {
    numberOfThreads = (int) strtol(argv[1], NULL, 10);
    printf("%d\n", numberOfThreads);
    char* originFileName = argv[2];
    char *filter_file_path = argv[3];
    char *result_file_path = argv[4];
    load_picture(originFileName);
    load_filter(filter_file_path);

    threads = (pthread_t *) malloc(numberOfThreads * sizeof(pthread_t));



/*
    while (fgets(line, sizeof(line), file)) {
        //printf("%s", line);
    }
    fclose(file);
*/

}
