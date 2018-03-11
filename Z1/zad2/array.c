#include "array.h"
#include <stdlib.h>
#include <limits.h>

struct Node* create_node(char* data, int n){
    struct Node* node = (struct Node*) calloc(1, sizeof(struct Node));
    node->data = data;
    node->n = n;
    node->sum = calculateSum(data, n);
    return node;
}

struct CharArray* create_char_arr(int n){
    struct CharArray* charArray = (struct CharArray*) calloc(1, sizeof(struct CharArray));
    charArray->n=n;
    charArray->array= (struct Node**) calloc(n, sizeof(struct Node*));
    return charArray;
}

void addNodeToCharArray(struct CharArray* charArray, char* data, int n, int i){
    struct Node* node = create_node(data, n);
    charArray->array[i] = node;
}

int calculateSum(char* array, int n) {
    int i;
    int sum = 0;
    for(i = 0; i < n; i++) {
        sum+=array[i];
    }
    return sum;
}

void free_node(struct Node* node){
    free(node->data);
    free(node);
}

void free_char_array(struct CharArray* charArray) {
    int i;
    for(i = 0; i < charArray->n; i++){
        free_node(charArray->array[i]);
    }
    free(charArray);
}

void remove_block(int i, struct CharArray* charArray) {
    free_node(charArray->array[i]);
    charArray->array[i] = NULL;
}

void add_block(int i, struct CharArray* charArray, struct Node* node) {
    if(charArray->array[i] != NULL){ //in case there is already something
        free_node(charArray->array[i]);
        charArray->array[i] = NULL;
    }
    charArray->array[i] = node;
}

int search_node(struct CharArray* charArray, int num) {
    int diff;
    int ind;
    if(charArray->array[0] != NULL){
        diff = charArray->array[0]->sum-num;
        ind = 0;
    }
    int i;
    for(i = 1; i < charArray->n; i++){
        if(charArray->array[i]->sum-num < diff){
            diff = charArray->array[i]->sum;
            ind = i;
        }
    }
    return ind;
}

void initalize_array() {
    int i, j;
    for(i = 0; i < DIM_X_ARRAY; i++) {
        for(j = 0; j < DIM_Y_ARRAY; j++){
            static_arr[i][j] = 0;
        }
    }
}

void clear_array() {
    int i, j;
    for(i = 0; i < DIM_X_ARRAY; i++) {
        for(j = 0; j < DIM_Y_ARRAY; j++){
            static_arr[i][j] = 0;
        }
    }
}

void add_block_to_static_array(struct Node* node, int i){
    int j;
    for(j = 0; j < node->n; j++){
        static_arr[i][j] = node->data[j];
    }
}

void remove_block_from_static_array(int i){
    int j;
    for(j = 0; j < DIM_Y_ARRAY; j++){
        static_arr[i][j] = 0;
    }
}

int search_node_within_arr(int num) {
    int diff = INT_MAX;
    int ind_sum;
    int i, j;
    for(i = 0; i < DIM_X_ARRAY; i++) {
        if(static_arr[i][0]!=0) {
            int sum = 0;
            for(j = 0; j < DIM_Y_ARRAY; j++){
                sum += static_arr[i][j];
            }
            if(sum < diff) {
                diff = sum;
                ind_sum = 0;
            }
        }
    }
    if(diff != INT_MAX) {
        return ind_sum;
    } else {
        return -1;
    }
}
