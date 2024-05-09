#ifndef LZ77_H
#define LZ77_H
/*
min: 9 bits 
max: 27 bits 
*/
typedef struct lz77_token_s {
    // 0 : raw 
    // 1 : back ref
    unsigned int type : 1; 
    union {
        uint16_t raw_byte : 9;
        struct {
            unsigned int dist : 17;
            unsigned int length : 9; 
        } match; 
    } bits; 
} lz77_tok_t; 

int get_length_code(int length, int* eb_val); 
unsigned int* lz77(uint8_t* data, size_t len, int* length_literal_freq, int* dist_freq, int* final_size); 
int get_dist_code(int dist, int* eb_val);  
#endif 