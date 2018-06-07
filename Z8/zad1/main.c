#include <stdio.h>
#include <pthread.h>
#include <stdlib.h>
#include <memory.h>
#include <math.h>
#include <sys/resource.h>
#include <sys/time.h>


#define max(a,b) \
   ({ __typeof__ (a) _a = (a); \
       __typeof__ (b) _b = (b); \
     _a > _b ? _a : _b; })

#define min(j,l) \
   ({ __typeof__ (j) _j = (j); \
       __typeof__ (l) _l = (l); \
     _j < _l ? _j : _l; })


int numberOfThreads;
int H, W, C, M, P;
int **I; //picture
float **K; //filter

pthread_t *threads; //threads array
int **J; //picture with filter

void load_picture(char *file_path) {
    FILE *file;
    if ((file = fopen(file_path, "r")) == NULL) {
        printf("Opening input file failed");
        exit(2);
    }
    fscanf(file, "P%d\n", &P);
    fscanf(file, "%d", &W);
    fscanf(file, "%d", &H);
    fscanf(file, "%d", &M);

    I = (int**) malloc(W * sizeof(int**));
    for (int i = 0; i < W; ++i)
        I[i] = (int*) malloc(H * sizeof(int));

    for (int i = 0; i < W; ++i)
        for (int j = 0; j < H; ++j)
            fscanf(file, "%d", &I[i][j]);

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

struct counted_times{
    struct timeval system_start_time, system_end_time, system_timediff,
            user_start_time, user_end_time, user_timediff,
            real_start_time, real_end_time, real_timediff;
};


void set_timediff(struct timeval* start_time, struct timeval* end_time, struct timeval* result){

    if (start_time->tv_sec > end_time->tv_sec)
        timersub(start_time, end_time, result);
    else if (start_time->tv_sec < end_time->tv_sec)
        timersub(end_time, start_time, result);
    else  // start_time.tv_sec == end_time.tv_sec
    {
        if (start_time->tv_usec >= end_time->tv_usec)
            timersub(start_time, end_time, result);
        else
            timersub(end_time, start_time, result);
    }
}

void count_difference(struct counted_times *times){

    set_timediff(&times->system_start_time, &times->system_end_time, &times->system_timediff);
    set_timediff(&times->user_start_time, &times->user_end_time, &times->user_timediff);
    set_timediff(&times->real_start_time, &times->real_end_time, &times->real_timediff);

}

void print_times(struct counted_times times){

    printf("\nREAL: %fs\t", (double) times.real_timediff.tv_sec  + ((double) times.real_timediff.tv_usec) / 1000000);
    printf("USER: %fs\t",(double) times.user_timediff.tv_sec  + ((double) times.user_timediff.tv_usec) / 1000000);
    printf("SYSTEM: %fs\n",(double) times.system_timediff.tv_sec  + ((double) times.system_timediff.tv_usec) / 1000000);
}

void set_times(struct rusage res_start_time, struct rusage res_end_time, struct counted_times* times){

    times->system_start_time = res_start_time.ru_stime;
    times->user_start_time = res_start_time.ru_utime;
    times->system_end_time = res_end_time.ru_stime;
    times->user_end_time = res_end_time.ru_utime;
}

void to_file(struct counted_times times, FILE *sp){

    fprintf(sp, "\nREAL: %fs\t", (double) times.real_timediff.tv_sec  + ((double) times.real_timediff.tv_usec) / 1000000);
    fprintf(sp, "USER: %fs\t", (double) times.user_timediff.tv_sec  + ((double) times.user_timediff.tv_usec) / 1000000);
    fprintf(sp, "SYSTEM: %fs\n",(double) times.system_timediff.tv_sec  + ((double) times.system_timediff.tv_usec) / 1000000);
}

void save_result(char *file_path) {
    FILE *file;
    if ((file = fopen(file_path, "w+")) == NULL) {
        printf("Cannot open output file...\n");
        exit(2);
    }
    fprintf(file, "P2\n%d %d\n%d\n", W, H, M);
    for (int i = 0; i < H; ++i) {
        for (int j = 0; j < W; ++j) {
            fprintf(file, "%d ", J[i][j]);
            if(j==W-1) {
                fprintf(file, " \n");
            }
        }
    }
    fclose(file);
}

void* filter(void* args){

    long tid = (long) args;
    int start_width = (int) ((tid * W) / numberOfThreads);
    int end_width = (int) (((tid + 1) * W) / numberOfThreads);
    int c_ceil = (int) ceil(C/2);

    for(int x = start_width; x < end_width; x++){
        for(int y = 0; y < H; y++){
            double s_xy = 0;
            for(int i = 0; i < C; i++){
                int ix = min((W-1), max(0, x - c_ceil + i));
                for(int j = 0; j < C; j++){
                    int iy = min((H-1), max(0, y - c_ceil + j));

                    s_xy += I[ix][iy] * K[i][j];
                }
            }
            J[x][y] = (int) round(s_xy);
        }
    }
    pthread_exit(NULL);
}



int main(int argc, char** argv) {

    numberOfThreads = (int) strtol(argv[1], NULL, 10);
    char* originFileName = argv[2];
    char *filter_file_path = argv[3];
    char *result_file_path = argv[4];

    load_picture(originFileName);
    load_filter(filter_file_path);

    struct rusage res_start_time;
    struct rusage res_end_time;
    struct counted_times times;


    FILE* result_file = fopen(result_file_path, "w+");

    J = (int**) malloc(W * sizeof(int *));
    for (int i = 0; i < H; ++i)
        J[i] = (int*) malloc(H * sizeof(int));

    pthread_t* threads = (pthread_t*) calloc((size_t) numberOfThreads, sizeof(pthread_t));

    gettimeofday(&times.real_start_time, NULL);
    getrusage(RUSAGE_SELF, &res_start_time);

    for(long i = 0; i < numberOfThreads; i++) {
        pthread_create(&threads[i], NULL, filter, (void *) i);
    }
    for(long i = 0; i < numberOfThreads; i++) {
        pthread_join(threads[i], NULL);
    }

    gettimeofday(&times.real_end_time, NULL);
    getrusage(RUSAGE_SELF, &res_end_time);

    set_times(res_start_time, res_end_time, &times);
    count_difference(&times);

    FILE* res;
    res = fopen("times.txt", "a+");
    fprintf(res, "Filtering with filter of size : %d, with usage of %d threads time:\t\n", C, numberOfThreads);
    to_file(times, res);


    save_result(result_file_path);
    fclose(result_file);
    fclose(res);

    //The pthread_exit() function terminates the calling thread and returns
    //a value via retval that (if the thread is joinable) is available to
    //another thread in the same process that calls pthread_join(3).
    pthread_exit(NULL);
}
