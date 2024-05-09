#ifndef HUFFMAN_H
#define HUFFMAN_H
#define MAX_BITS 15

#include "collections.h"

int* huffman(int freq_table[], size_t len, int* max); 
int get_depths(int lengths[], h_node_t* node, int iteration); 
int* generate_canonical(int lengths[], size_t n, int max); 
#endif 