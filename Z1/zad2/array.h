#ifndef _ARRAY_H
#define _ARRAY_H

#define DIM_X_ARRAY 10000
#define DIM_Y_ARRAY 500

struct Node{
    char *data;
    int n; //size of single data block
    int sum;
};

struct Node* create_node(char* data, int n);
int calculateSum(char* array, int n);
void free_node(struct Node* node);

struct CharArray {
    struct Node** array;
    int n; //size of array of blocks
};

struct CharArray* create_char_arr(int n);
void free_char_array(struct CharArray* charArray);
void addNodeToCharArray(struct CharArray* charArray, char* data, int n, int i);

/* add block of given index to array */
void add_block(int i, struct CharArray* charArray, struct Node* node);

/* remove block of given index from array */
void remove_block(int i, struct CharArray* charArray);

/* find node which sum is closest to given num */
int search_node(struct CharArray* charArray, int num);

char static_arr[DIM_X_ARRAY][DIM_Y_ARRAY];

void initalize_array();
void clear_array();

void add_block_to_static_array(struct Node* node, int i);
void remove_block_from_static_array(int i);
int search_node_within_arr(int num);

#endif
