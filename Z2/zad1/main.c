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



int main(int argc,char * argv[]){

    struct Input_set* input_set = (struct Input_set*) malloc(sizeof(struct Input_set));
    int result = parse_input(input_set,argc,argv);
    if(result == 0){
        switch (input_set->op_type){
            case generate_type:
                result = generate(input_set->filename,input_set->records_number,input_set->records_width);
                break;
        }
    }
    free(input_set);
    return result;
}
