#ifndef DEFLATE_H
#define DEFLATE_H
#include <stdint.h> 

#define LL_ALPHABET_SIZE 286 
#define DIST_ALPHABET_SIZE 30 

typedef enum ENC_TYPE_E { CODE_LENGTHS, LITERALS } ENC_TYPE_T; 

uint8_t* deflate(uint8_t* data, size_t len, int* compressed_size); 
void deflate_block_hdr(int* ll_cl, int* dist_cl, bitstream_t* bs);
void get_cl_freqs(int* cl, size_t cl_count, int* sym_freq);
int get_unused_tail(int* cl, size_t n); 
int* lit_dist_stream(int* ll_cl, int* dist_cl, int ll_unused, int dist_unused, int* stream_size); 
// void send_enc_bits(bitstream_t* bs, list_t* stream, int* cl_cl, int* canon); 
void send_enc_bits(bitstream_t* bs, unsigned int* raw, size_t raw_size, int* canon, int* cl, ENC_TYPE_T et, ...); 

#endif 