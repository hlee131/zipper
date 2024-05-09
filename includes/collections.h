#ifndef COLLECTIONS_H 
#define COLLECTIONS_H
#define BUCKET_COUNT 10 

#include <stdint.h>

typedef enum BIT_ORDER_E { MSB, LSB } BIT_ORDER_T; 

typedef struct node_s node_t;  
// typedef struct hmap_s hmap_t; 
typedef struct list_s list_t; 
typedef struct huffman_node_s h_node_t; 
typedef struct bitstream_s {
    uint8_t* mem;
    int bytei;
    int biti; 
    size_t alloc_size; 
} bitstream_t; 

struct list_s {
    node_t* head; 
    node_t* tail; 
    size_t length; 
};
 
struct node_s {
    union {
        void* ptr;
        int raw; 
    } data ; 
    node_t* next; 
};

struct huffman_node_s {
    int freq;
    int symbol; 
    struct huffman_node_s* left; 
    struct huffman_node_s* right; 
};

void insert_ptr(list_t* list, void* ptr);
void insert_raw(list_t* list, int raw);
node_t* pop_head(list_t* list);
list_t* new_list(); 
list_t** new_chained_table();
void insert_table(list_t** table, uint8_t crc, int loc); 
bitstream_t* new_bitstream(size_t size); 
void send_bits(bitstream_t* bs, uint16_t data, size_t bitc, BIT_ORDER_T wo); 

// struct hmap_s {
//     list_t* buckets[BUCKET_COUNT]; 
// };

// hmap_t* new_hashmap(); 
// int get_index(uint32_t key);
// void insert(hmap_t* map, uint32_t key, int loc);
// list_t* get(hmap_t* map, uint32_t);
#endif 