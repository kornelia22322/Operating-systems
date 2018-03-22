#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <unistd.h>
#include <time.h>

enum Op_type{
    generate_type,
    copy_type,
    sort_type
};

enum In_type{
    sys,
    lib
};

struct Input_set{
    enum Op_type op_type;
    enum In_type in_type;
    char * filename;
    int records_number;
    int records_width;
};

int sort(char *filename, int number, int width, enum In_type type);
int raiseError(char* fileName, unsigned char* row_a, unsigned char* row_b, int file);


int parse_input(struct Input_set* input_set, int argc, char* argv[]){
    int i = 1;
    if(argc == i){
        printf("Brak %d argumentu!(typ operacji)\n",i);
        return 1;
    }
    else{
        if(strcmp(argv[i],"generate") == 0){
            input_set->op_type = generate_type;
        }
        else if(strcmp(argv[i],"copy") == 0){
            input_set->op_type = copy_type;
        }
        else if(strcmp(argv[i],"sort") == 0){
            input_set->op_type = sort_type;
        }
        else{
            printf(" %d argumentem powinien byc typ operacji generate/copy/sort\n", i);
            return 1;
        }
        i++;
    }
    if(input_set->op_type != generate_type){
        if(argc == i){
            printf("Brak %d argumentu!(typ funkcji)\n",i);
            return 1;
        }
        if(strcmp(argv[i],"sys") == 0){
            input_set->in_type = sys;
        }
        else if(strcmp(argv[i],"lib") == 0){
            input_set->in_type = lib;
        }
        else{
            printf("%d argumentem powinien byc typ funkcji sys/lib\n",i);
            return 1;
        }
        i++;
    }
    if(argc == i){
        printf("Brak %d argumentu(sciezka do pliku)!\n",i);
        return 1;
    }
    else{
        input_set->filename = argv[i];
        i++;
    }
    if(input_set->op_type!=copy_type) {
        if(argc == i){
            printf("Brak %d argumentu(wielkosc rekordu)!\n",i);
            return 1;
        }
        else{
            input_set->records_width = atoi(argv[i]);
            if(input_set->records_width <= 0){
                printf("Wartosc %d argumentu musi byc wieksza od zera\n",i);
                return 1;
            }
            i++;
        }
        if(argc == i){
            printf("Brak %d argumentu(ilosc rekordow)!\n",i);
            return 1;
        }
        else{
            input_set->records_number = atoi(argv[i]);
            if(input_set->records_number <= 0){
                printf("Wartosc %d argumentu musi byc wieksza od zera\n",i);
                return 1;
            }
            i++;
        }
    }
    return 0;
}

int generate(char * filename,int records_number,int records_width){
    int randomData = open("/dev/random", O_RDONLY);
    if(randomData == -1){
        perror("/dev/random");
        return 1;
    }
    int out = open(filename,O_WRONLY|O_CREAT|O_TRUNC, S_IRUSR|S_IWUSR);
    if(out == -1){
        close(randomData);
        perror(filename);
        return 1;
    }
    char *myRandomData = (char*) calloc(records_width, sizeof (char));
    for(int i = 0; i < records_number; i++){
        ssize_t result = read(randomData, myRandomData, records_width);
        if (result < 0)
        {
            close(randomData);
            close(out);
            free(myRandomData);
            perror("/dev/random");
            return 1;
        }
        result = write(out,myRandomData,records_width);
        if (result < 0)
        {
            close(randomData);
            close(out);
            free(myRandomData);
            perror(filename);
            return 1;
        }
    }
    close(randomData);
    close(out);
    free(myRandomData);
    return 0;
}

char* generate_random_char_arr(int records_width) {
    char *myRandomData = (char*) calloc(records_width, sizeof (char));
    int i;
    for(i = 0; i < records_width; i++) {
        char r = rand()%30+97;
        myRandomData[i] = r;
    }
    return myRandomData;
}

int generate_simple_random(char * filename, int records_number, int records_width) {
    int out = open(filename,O_WRONLY|O_CREAT|O_TRUNC, S_IRUSR|S_IWUSR);
    if(out == -1){
        perror(filename);
        return 1;
    }
    for(int i = 0; i < records_number; i++){
        char* data = generate_random_char_arr(records_width);
        int result = write(out,data,records_width);
        if (result < 0)
        {
            close(out);
            free(data);
            perror(filename);
            return 1;
        }

        free(data);
    }
    close(out);
    return 0;
}

int sys_insertion_sort(char * filename,int records_number,int records_width) {
    int file = open(filename,O_RDWR);
    if(file == -1){
        perror(filename);
        return 1;
    }
    unsigned char *row_a = (unsigned char*) calloc(records_width,sizeof (unsigned char));
    unsigned char *row_b = (unsigned char*) calloc(records_width,sizeof (unsigned char));

    int i;
    for(i = 1; i < records_number; i++) {
        int j = i - 1;
        lseek(file,i*records_width,SEEK_SET);
        ssize_t result = read(file, row_a, records_width);
        if (result < 0) {
            raiseError(filename, row_a, row_b, file);
            return 1;
        }
        lseek(file,j*records_width,SEEK_SET);
        result = read(file, row_b, records_width);
        if (result < 0) {
            raiseError(filename, row_a, row_b, file);
            return 1;
        }

        while(row_b[0] > row_a[0] && j>=0){
            lseek(file,(j+1)*records_width,SEEK_SET);
            result = write(file, row_b, records_width);
            if (result < 0) {
                raiseError(filename, row_a, row_b, file);
                return 1;
            }
            j--;
            if(j>=0) {
                lseek(file,j*records_width,SEEK_SET);
                result = read(file, row_b, records_width);
            }
            if (result < 0) {
                raiseError(filename, row_a, row_b, file);
                return 1;
            }
        }


        lseek(file,(j+1)*records_width,SEEK_SET);
        result = write(file, row_a, records_width);
    }
    return 0;
}

int copy_file_sys_func(char* filename) {
    char block[1024];
    int liczyt;
    int we, wy;
    we=open(filename, O_RDONLY);
    if(we < 0) {
        close(we);
        return 1;
    }
    wy=open("copied_data_sys",O_WRONLY|O_CREAT,S_IRUSR|S_IWUSR);
    if(we < 0) {
        close(wy);
        return 1;
    }
    while((liczyt=read(we,block,sizeof(block)))>0)
        write(wy,block,liczyt);

    return 0;
}

void copy_file_lib_func() {
    char blok[1024];
    int liczyt;

    FILE* in_file = fopen("we_lib", "r");
    FILE* out_file = fopen("wy_lib", "rb+");
    if(out_file == NULL) //if file does not exist, create it
    {
        out_file = fopen("wy_lib", "wb");
    }
    while((liczyt=fread(blok,sizeof(char),sizeof(blok),in_file))>0)
        fwrite(blok,sizeof(char),liczyt,out_file);
}

int raiseError(char* fileName, unsigned char* row_a, unsigned char* row_b, int file) {
    perror(fileName);
    close(file);
    free(row_a);
    free(row_b);
    return 1;
}

int sort(char *filename, int number, int width, enum In_type type) {
    if(type == sys) return sys_insertion_sort(filename,number,width);
    return 0;
}

int copy(char *filename, enum In_type type) {
    if(type == sys) return copy_file_sys_func(filename);
    return 0;
}


int main(int argc,char * argv[]){

    srand(time(NULL));

    struct Input_set* input_set = (struct Input_set*) malloc(sizeof(struct Input_set));
    int result = parse_input(input_set,argc,argv);
    if(result == 0){
        switch (input_set->op_type){
            case generate_type:
                //result = generate(input_set->filename,input_set->records_number,input_set->records_width);
                result = generate_simple_random(input_set->filename,input_set->records_number,input_set->records_width);
                break;
            case sort_type:
                result = sort(input_set->filename,input_set->records_number,input_set->records_width,input_set->in_type);
                break;
            case copy_type:
                result = sort(input_set->filename,input_set->records_number,input_set->records_width,input_set->in_type);
                break;

        }
    }
    free(input_set);
    return result;
}
