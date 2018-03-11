#include "array.h"
#include <stdlib.h>
#include <stdio.h>
#include <time.h>

void test_dynamic_library(int n, int sizeofBlock, int array[], int numofOperations);
void test_static_library(int n, int sizeofBlock, int array[], int numofOperations);
char* generate_data(int sizeofBlock);
char generate_random_char();
struct CharArray* initialize_dynamic_array(int n, int sizeofBlock);
int generate_random_num(int maxval);
void execute_operations_dynamic(int array[], int numofOperations, struct CharArray* charArray);
void realloc_blocks_sequentially(struct CharArray* charArray);
void realloc_blocks_alternately(struct CharArray* charArray);
int* getRange(struct CharArray* charArray);

/* 1 - static memory allocation, 0 - dynamic memory allocation,
n - numbers of blocks, sizeofBlock - size of single block, array - list of operations,
numofOperations - numbers of operations in array */

/* 1 - initializing array, 2 - find element, 3 - remove and add blocks, 4 - remove and add alternately
.. */
void test_library(int n, int sizeofBlock, int memAllocation, int array[], int numofOperations) {
    if(memAllocation) {
        test_dynamic_library(n, sizeofBlock, array, numofOperations);
    } else {
        test_static_library(n, sizeofBlock, array, numofOperations);
    }
}

void test_dynamic_library(int n, int sizeofBlock, int array[], int numofOperations) {
    if(array[0] == 1) {
        CharArray* charArray = initialize_dynamic_array(n, sizeofBlock);
        execute_operations_dynamic(array, numofOperations, charArray);
    } else {
        printf("You have to initalize an array first.");
    }
}

void execute_operations_dynamic(int array[], int numofOperations, struct CharArray* charArray) {
    int i;
    for(i = 0; i < numofOperations; i++) {
        switch (array[i]) {
            case 2:
                search_node(charArray, generate_random_num(100));
                break;
            case 3:
                realloc_blocks_sequentially(charArray);
                break;
            case 4:
                realloc_blocks_alternately(charArray);
                break;
        }
    }
}

void realloc_blocks_alternately(struct CharArray* charArray) {
    int* range = getRange(charArray);
    int size_of_single_block = charArray->array[0]->n;
    int i;
    for(i = range[0]; i < range[1]; i++) {
        remove_block(i, charArray);
        add_block(i, charArray, create_node(generate_data(size_of_single_block), size_of_single_block));
    }
}

int* getRange(struct CharArray* charArray) {
    int starting_block = generate_random_num(charArray->n);
    int num_of_blocks;
    do {
        num_of_blocks = generate_random_num(charArray->n - starting_block);
    } while(num_of_blocks == 0);
    int* array = (int*) calloc(2, sizeof(int));
    array[0] = starting_block;
    array[1] = starting_block + num_of_blocks;
    return array;
}

void realloc_blocks_sequentially(struct CharArray* charArray) {
    int* range = getRange(charArray);
    int size_of_single_block = charArray->array[0]->n;
    int i;
    for(i = range[0]; i < range[1]; i++) {
        remove_block(i, charArray);
    }
    for(i = range[0]; i < range[1]; i++) {
        add_block(i, charArray, create_node(generate_data(size_of_single_block), size_of_single_block));
    }
}

CharArray* initialize_dynamic_array(int n, int sizeofBlock) {
    struct CharArray* charArray = create_char_arr(n);
    int i;
    for(i = 0; i < n; i++) {
        char* data = generate_data(sizeofBlock);
        addNodeToCharArray(charArray, data, sizeofBlock, i);
    }
    return charArray;
}

char* generate_data(int sizeofBlock) {
    char* array = (char*) calloc(sizeofBlock, sizeof(char));
    int i;
    for(i = 0; i < sizeofBlock; i++) {
        array[i] = generate_random_char();
    }
    return array;
}

char generate_random_char() {
    int r = rand() % 95 + 33;
    return r;
}

int generate_random_num(int maxval) {
    int r = rand() % maxval;
    return r;
}

int main(void) {
    srand(time(NULL));

}
