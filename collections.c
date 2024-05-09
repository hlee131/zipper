#include <string.h> 
#include <stdio.h>
#include <stdlib.h>

#include "includes/collections.h"
#include "includes/helper.h"

list_t* new_list() {
    list_t* l = (list_t*) checked_malloc(sizeof(list_t)); 
    l->head = l->tail = NULL;
    l->length = 0; 
    return l; 
}

node_t* pop_head(list_t* list) {
    node_t* ret = list->head; 
    list->head = list->head->next; 
    list->length--; 
    return ret; 
}

bitstream_t* new_bitstream(size_t size) {
    bitstream_t* bs = checked_malloc(sizeof(bitstream_t));
    bs->mem = calloc(size, sizeof(uint8_t));
    bs->bytei = bs->biti = 0; 
    bs->alloc_size = size; 
    return bs; 
}

void send_bits(bitstream_t* bs, uint16_t data, size_t bitc, BIT_ORDER_T wo) {
    for (int i = (wo == LSB ? 0 : bitc - 1); 
        (wo == LSB ? i < bitc : i >= 0); 
        i += (wo == LSB ? 1 : -1)) {
        // get lsb of data byte 
        // shift lsb into the bit index
        bs->mem[bs->bytei] |= ((data & (0x01 << i)) >> i) << (bs->biti);  
        if (++(bs->biti) > 7) {
            bs->biti = 0;
            if (bs->alloc_size <= bs->bytei - 1) {
                bs->alloc_size = (int) (bs->alloc_size * 1.3); 
                bs->mem = realloc(bs->mem, bs->alloc_size * sizeof(uint8_t)); 
                if (bs->mem == NULL) {
                    puts("error: allocation error");
                    exit(1); 
                } 
                // TODO: check realloc 
            }
            bs->bytei++; 
        }
    }
    // printf("Written %d amount of bits, value: %x\n", ((bs->bytei) * 8) + bs->biti, data); 
}

void insert_ptr(list_t* list, void* ptr) {
    node_t* new_node = checked_malloc(sizeof(node_t));
    new_node->data.ptr = ptr; 
    new_node->next = NULL; 
    
    if (list->head == NULL) {
        // if empty, assign new node as head and tail 
        list->head = list->tail = new_node;  
    } else {
        // append new_node to the tail 
        list->tail->next = new_node; 
        // set new tail as new node 
        list->tail = new_node; 
    }

    list->length++; 
}

void insert_raw(list_t* list, int raw) {
    node_t* new_node = checked_malloc(sizeof(node_t));
    new_node->data.raw = raw; 
    new_node->next = NULL; 
    
    if (list->head == NULL) {
        // if empty, assign new node as head and tail 
        list->head = list->tail = new_node;  
    } else {
        // append new_node to the tail 
        list->tail->next = new_node; 
        // set new tail as new node 
        list->tail = new_node; 
    }

    list->length++; 
}

list_t** new_chained_table() {
    return (list_t**) (calloc(256, sizeof(list_t*))); 
}

void insert_table(list_t** table, uint8_t crc, int loc) {
    if (table[crc] == NULL) table[crc] = new_list(); 
    insert_raw(table[crc], loc); 
}