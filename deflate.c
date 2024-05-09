#include <stdlib.h>
#include <assert.h> 
#include <string.h>
#include <stdio.h> 
#include <stdarg.h> 

#include "includes/helper.h"
#include "includes/deflate.h" 
#include "includes/huffman.h"
#include "includes/lz77.h"
#include "includes/collections.h"

uint8_t* deflate(uint8_t* data, size_t len, int* compressed_size) {
    bitstream_t* bs = new_bitstream(len); 
    // send deflate block header
    // first bit: BFINAL: set 
    send_bits(bs, 1, 1, LSB); 
    // next 2 bits: BTYPE: 10 for dynamic huffman codes
    send_bits(bs, 2, 2, LSB); 
    
    int ll_freq[LL_ALPHABET_SIZE] = {};
    int dist_freq[DIST_ALPHABET_SIZE] ={}; 

    int backreffed_size;
    unsigned int* backreffed = lz77(data, len, ll_freq, dist_freq, &backreffed_size);

    int ll_max; int dist_max; 
    
    int* ll_cl = huffman(ll_freq, LL_ALPHABET_SIZE, &ll_max); 
    int* dist_cl = huffman(dist_freq, DIST_ALPHABET_SIZE, &dist_max); 

    assert(ll_max <= 15); 
    assert(dist_max <= 15); 

    int* ll_canon = generate_canonical(ll_cl, LL_ALPHABET_SIZE, ll_max);
    int* dist_canon = generate_canonical(dist_cl, DIST_ALPHABET_SIZE, dist_max);

    // deflate block header 
    deflate_block_hdr(ll_cl, dist_cl, bs); 

    send_enc_bits(bs, backreffed, backreffed_size, ll_canon, ll_cl, LITERALS, dist_canon, dist_cl); 
    
    exit: {
    // free extra space created by compression and free bitstream 
    uint8_t* truncated = realloc(bs->mem, (bs->bytei + 1) * sizeof(uint8_t)); 
    if (truncated == NULL) {
        puts("error: truncation failed"); 
        free(bs->mem); 
        free(bs); 
        exit(1);  
    } 
        
    (*compressed_size) = bs->bytei + 1;  
    free(bs); 
    return truncated; 
    }
}

// describes the alphabet used to deflate
// writes directly into the bitstream; 
void deflate_block_hdr(int* ll_cl, int* dist_cl, bitstream_t* bs) {
    
    // 0 - 18 symbol frequencies, zero initialized
    int sym_freq[19] = {};
    int HLIT, HDIST, HCLEN;  
    int stream_size; 
    
    int ll_unused = get_unused_tail(ll_cl, LL_ALPHABET_SIZE);
    int dist_unused = get_unused_tail(dist_cl, DIST_ALPHABET_SIZE); 
    ll_unused = MIN(ll_unused, 29);
    dist_unused = MIN(dist_unused, 29);
    int* stream = lit_dist_stream(ll_cl, dist_cl, ll_unused, dist_unused, &stream_size);

    HLIT = LL_ALPHABET_SIZE - ll_unused - 257; 
    HDIST = DIST_ALPHABET_SIZE - dist_unused - 1; 

    // get freqs
    for (int i = 0; i < stream_size; i++) sym_freq[stream[(stream[i] > 15 ? i++ : i)]]++; 
        
    // get huffman codes 
    int cl_cl_max; 
    int* cl_cl = huffman(sym_freq, 19, &cl_cl_max); 
    int* canon_codes = generate_canonical(cl_cl, 19, cl_cl_max); 
    int canon_ordered[19] = {}; 
    
    int mapping[] = {16, 17, 18, 0, 8, 7, 9, 6, 10, 5, 11, 4, 12, 3, 13, 2, 14, 1, 15}; 
    for (int i = 0; i < 19; i++) {
        canon_ordered[i] = cl_cl[mapping[i]]; 
    }

    int cl_cl_unused = get_unused_tail(canon_ordered, 19); 
    cl_cl_unused = MIN(cl_cl_unused, 14); 
    HCLEN = 19 - cl_cl_unused - 4; 
    
    assert(cl_cl_max <= 15);  
    
    /*
    5 Bits: HLIT, # of Literal/Length codes - 257 (257 - 286)
    5 Bits: HDIST, # of Distance codes - 1        (1 - 32)
    4 Bits: HCLEN, # of Code Length codes - 4     (4 - 19)
    */
    send_bits(bs, HLIT, 5, LSB); 
    send_bits(bs, HDIST, 5, LSB);
    send_bits(bs, HCLEN, 4, LSB);

    /*
    (HCLEN + 4) x 3 bits: code lengths for the code length
      alphabet given just above, in the order: 16, 17, 18,
      0, 8, 7, 9, 6, 10, 5, 11, 4, 12, 3, 13, 2, 14, 1, 15
    */
    for (int i = 0; i < HCLEN + 4; i++) send_bits(bs, cl_cl[mapping[i]], 3, LSB);

    /*
    HLIT + 257 code lengths for the literal/length alphabet,
        encoded using the code length Huffman code
    HDIST + 1 code lengths for the distance alphabet,
        encoded using the code length Huffman code
    */ 
    send_enc_bits(bs, stream, stream_size, canon_codes, cl_cl, CODE_LENGTHS); 
}

int get_unused_tail(int* cl, size_t n) {
    int unused_tail_amt = 0; 
    for (int i = n - 1; i > 0; i--) {
        if (cl[i] != 0) break; 
        else unused_tail_amt++; 
    }
    return unused_tail_amt; 
}

// TODO: join the actual streams 
int* lit_dist_stream(int* ll_cl, int* dist_cl, int ll_unused, int dist_unused, int* stream_size) {
    // initialize stream to maximum size
    int* stream = calloc((LL_ALPHABET_SIZE + DIST_ALPHABET_SIZE - ll_unused - dist_unused), sizeof(int)); 
    int stream_index = 0; 

    for (int j = 0; j < 2; j++) {
        int* curr_cl = (j == 0 ? ll_cl : dist_cl);
        for (int i = 0; 
            i < (j == 0 ? LL_ALPHABET_SIZE - ll_unused : 
                          DIST_ALPHABET_SIZE - dist_unused); 
            i++) {
            int second_ptr = i + 1; 
            while (curr_cl[second_ptr] == curr_cl[i]) second_ptr++; 
            int dist = second_ptr - i;
            if (curr_cl[i] == 0 && dist >= 3) {
                if (dist >= 3 && dist <= 10) {
                    stream[stream_index++] = 17;
                    stream[stream_index++] = dist - 3;    
                } else if (dist >= 11) {
                    dist = MIN(dist, 138); 
                    stream[stream_index++] = 18;
                    stream[stream_index++] = dist - 11;  
                } 
                i += dist - 1;
            } else if (dist > 3) {
                dist = MIN(dist, 7);
                stream[stream_index++] = curr_cl[i];
                stream[stream_index++] = 16;
                stream[stream_index++] = dist - 4;
                i += dist - 1;
            } else {
                assert(curr_cl[i] <= 15); 
                stream[stream_index++] = curr_cl[i];
            }
        }
    }

    int* truncated = realloc(stream, stream_index * sizeof(int)); 
    if (truncated == NULL) {
        puts("error: truncation failed"); 
        exit(1);  
    }
    *stream_size = stream_index; 
    return stream; 
}

void send_enc_bits(bitstream_t* bs, unsigned int* raw, size_t raw_size, int* canon, int* cl, ENC_TYPE_T et, ...) {
    va_list argp;
    va_start(argp, et);
    int* dist_canon; int* dist_cl;
    
    if (et == LITERALS) {
        dist_canon = va_arg(argp, int*);
        dist_cl = va_arg(argp, int*);
    }

    va_end(argp); 
    
    for (int i = 0; i < raw_size; i++) {
        unsigned int ele = raw[i];
        if (ele >= 16 && ele <= 18) {
            send_bits(bs, canon[ele], cl[ele], MSB); 
            // send extra bits if needed
            if (et == CODE_LENGTHS) {
                switch (ele) {
                    case 16: send_bits(bs, raw[++i], 2, LSB); break; 
                    case 17: send_bits(bs, raw[++i], 3, LSB); break; 
                    case 18: send_bits(bs, raw[++i], 7, LSB); break; 
                    default: continue; 
                }
            }
        } else {
            // write raw 
            send_bits(bs, canon[ele], cl[ele], MSB); 
            // check if any eb and dist codes to handle 
            if (ele == 256) return; 
            else if (ele > 256) {
                // send length extra bits 
                int eb_val = raw[++i]; 
                int bitc = (ele < 265 || ele == 285) ? 0 : ((ele - 265) / 4) + 1; 
                send_bits(bs, eb_val, bitc, LSB); 

                // send distance code and extra bits 
                ele = raw[++i]; 
                eb_val = raw[++i];
                bitc = MAX(0, (int) (ele / 2) - 1); 
                send_bits(bs, dist_canon[ele], dist_cl[ele], MSB);
                send_bits(bs, eb_val, bitc, LSB);
            }
        }
    } 
}