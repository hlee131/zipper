#include <stdlib.h>
#include <limits.h>
#include <string.h> 
#include <assert.h> 

#include "includes/huffman.h"
#include "includes/collections.h"
#include "includes/helper.h"

// parameter: int[] frequency table for each symbol
//            size_t length of frequency table 
// returns: int[] code lengths for each symbol in frequency table
int* huffman(int* freq_table, size_t len, int* max) {

    // declare table to maintain lengths of each symbol
    int* lengths = (int*) calloc(len, sizeof(int)); 
    h_node_t* root; 
    
    // zip freq table into tuple with symbols
    // { symbol, frequency }
    int zipped[len][2]; 
    for (int i = 0; i < len; i++) {
        zipped[i][0] = i;
        zipped[i][1] = freq_table[i];
    }
    
    // sort tuples into priority queue using selection sort 
    for (int i = 0; i < len; i++) {
        int min = zipped[i][1]; 
        int mindex = i; 
        for (int j = i + 1; j < len; j++) {
            if (zipped[j][1] < min) {
                min = zipped[j][1]; 
                mindex = j; 
            }
        }
        // tmp 
        int symbol = zipped[i][0];
        int freq = zipped[i][1]; 
        memcpy(zipped[i], zipped[mindex], sizeof(int) * 2);
        zipped[mindex][0] = symbol;
        zipped[mindex][1] = freq; 
    }
    
    // list of huffman nodes in queue 
    list_t* queue = new_list(); 
    for (int i = 0; i < len; i++) {
        // ignore unused symbol
        if (zipped[i][1] == 0) continue; 
        h_node_t* n = checked_malloc(sizeof(h_node_t)); 
        n->left = n->right = NULL; 
        n->symbol = zipped[i][0];
        n->freq = zipped[i][1]; 
        insert_ptr(queue, n); 
    }
    
    // at each lowest tuple, compare to combination and/or increment length 
    while (queue->head != NULL) {
        h_node_t* n; 
        h_node_t* left = pop_head(queue)->data.ptr; 
        h_node_t* right; 
        
        if (queue->head == NULL) {
            root = left; break; 
        } else {
            right = pop_head(queue)->data.ptr; 
            n = checked_malloc(sizeof(h_node_t));
            n->left = left;
            n->right = right; 
            n->symbol = -1; 
            n->freq = left->freq + right->freq; 
            root = n; 
        }

        // insert n back into queue with one node lookahead 
        for (node_t* lnode = queue->head; lnode; lnode = lnode->next) {
            h_node_t* curr = lnode->data.ptr; 
            if (lnode->next == NULL) break; 
            int next_freq = ((h_node_t*) lnode->next->data.ptr)->freq; 
            if (curr->freq <= n->freq && n->freq <= next_freq) {
                node_t* insert = checked_malloc(sizeof(node_t)); 
                insert->next = lnode->next; 
                insert->data.ptr = n; 
                lnode->next = insert; 
                goto exit; 
            }
        }
        
        insert_ptr(queue, n);
        exit: 
        continue;
    }
    assert(queue->head == NULL); 
    (*max) = get_depths(lengths, root, 0); 

    // return length table 
    return lengths; 
}

// returns the maximum depth 
int get_depths(int* lengths, h_node_t* node, int iteration) {
    if (node->left == NULL && node->right == NULL) {
        lengths[node->symbol] = iteration; 
        return iteration; 
    } else if (node->left != NULL && node->right != NULL) {
        int i1 = get_depths(lengths, node->left, iteration + 1);
        int i2 = get_depths(lengths, node->right, iteration + 1);   
        return i1 > i2 ? i1 : i2; 
    } else if (node->left != NULL) return get_depths(lengths, node->left, iteration + 1);
    else return get_depths(lengths, node->right, iteration + 1); 
}

// generates canonical huffman codes to encode data with 
// code is derived from DEFLATE specification: 
// https://datatracker.ietf.org/doc/html/rfc1951#page-13
int* generate_canonical(int* lengths, size_t n, int max) {
    // zero initialize counts
    int length_count[max + 1]; 
    int next_codes[max + 1]; 
    memset( length_count, 0, (max+1)*sizeof(int) );
    memset( next_codes, 0, (max+1)*sizeof(int) );
    int* canonical = calloc(n, sizeof(int)); 

    int code = 0; 
    for (int i = 0; i < n; i++) {
        if (lengths[i] != 0) { 
            assert(lengths[i] < max + 1 && lengths[i] >= 0); 
            length_count[lengths[i]]++;   
        }
    } 
    
    // populate next codes with base 
    /*     
     * All codes of a given bit length have lexicographically
     * consecutive values, in the same order as the symbols
     * they represent;

     * Shorter codes lexicographically precede longer codes.
    */ 
    for (int bit_length = 1; bit_length <= max; bit_length++) {
        // the + length_count[bits-2] is because "Shorter codes lexicographically precede longer codes."
        // shift bits because one code cannot be the prefix of another code
        assert(bit_length < max + 1 && bit_length >= 0); 
        assert(bit_length - 1 < max + 1 && bit_length - 1 >= 0);
        code 
            = next_codes[bit_length] 
            = (code + length_count[bit_length - 1]) << 1; 
    }
    
    // generate canonical symbols 
    for (int i = 0; i < n; i++) {
        if (lengths[i] != 0) {
            assert(i >= 0 && i < n); 
            assert(lengths[i] >= 0 && lengths[i] < max + 1);
            canonical[i] = (next_codes[lengths[i]])++;
        }
    }
    
    return canonical; 
}