#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <pthread.h>
#include <sys/times.h>
#include <math.h>

#define MAX(x, y) (((x) > (y)) ? (x) : (y))
#define MIN(x, y) (((x) < (y)) ? (x) : (y))

int numberOfThreads;
int H, W, C, M, P;
unsigned char **I; //picture
float **K; //filter

pthread_t *threads; //threads array
unsigned char **J; //picture with filter

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

void *filter_function(void *args) {
    int tmp = (int) ceil(C / 2);
    int whoIAm = *(int *) args;
    double s;

    int start = H * whoIAm / numberOfThreads;
    int end = H * (whoIAm + 1) / numberOfThreads;

    for(int i = start; i < end; i++) {
        for(int j = 0; j < H; j++) {
            s = 0;
            for (int h = 0; h < C; ++h) {
                for (int w = 0; w < C; ++w) {
                    int a = MIN((H - 1), MAX(0, i - tmp + h));
                    int b = MIN((W - 1), MAX(0, j - tmp + w));
                    s += I[a][b] * K[h][w];
                }
            }
            J[i][j] = (unsigned char) MAX(0, MIN(round(s), 255));
        }
    }
    return NULL;
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

    J = (unsigned char**) malloc(W * sizeof(unsigned char *));
    for (int i = 0; i < H; ++i)
        J[i] = (unsigned char*) malloc(H * sizeof(unsigned char *));

    for (int i = 0; i < numberOfThreads; ++i) {
        int *arg = (int*) malloc(sizeof(int));
        *arg = i;
        pthread_create(&threads[i], NULL, filter_function, arg);
    }
    for (int i = 0; i < numberOfThreads; ++i) {
        void *x;
        pthread_join(threads[i], &x);
    }

/*
    while (fgets(line, sizeof(line), file)) {
        //printf("%s", line);
    }
    fclose(file);
*/

}
