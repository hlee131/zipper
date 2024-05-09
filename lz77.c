#include <stdlib.h>

#include "includes/helper.h"
#include "includes/crc.h"
#include "includes/lz77.h"
#include "includes/collections.h"

unsigned int* lz77(uint8_t* data, size_t len, int* length_literal_freq, int* dist_freq, int* final_size) {
    unsigned int* compressed = checked_malloc(sizeof(unsigned int) * len); 
    list_t** chained_tab = new_chained_table(); 
    int cindex = 0; 
    
    for (int i = 0; i < len; i++) {
        int match = 0, loc = 0, length = 0; 
        // hash next 3 bytes 
        // checking for substrings with chained hash table 
        if (i > len - 3) {
            // get byte by dereferencing pointer and increment pointer 
            compressed[cindex++] = data[i];  
            length_literal_freq[data[i]]++; 
            continue; 
        }

        // check if table has list 
        uint8_t hash = crc8(data + i, 3);
        if (chained_tab[hash] != NULL) {
            int longest_index = 0, longest_length = 0;
            // iterate over each hash
            for (node_t* curr = chained_tab[hash]->head; curr; curr = curr->next) { 
                int nloc = curr->data.raw;
                // sliding window is only 32 KiB 
                if (i - nloc > 32768) break;

                int working_index = 0, curr_length = 0; 
                // iterate over each byte 
                while (1) { 
                    if (data[nloc + working_index] == 
                        *(data + i + working_index) && 
                        curr_length < 258) {
                        curr_length++; working_index++; 
                    } else break; 
                }
                if (curr_length > longest_length) { 
                    longest_index = nloc; 
                    longest_length = curr_length; 
                }
            }
            loc = longest_index; length = longest_length; 
        }

        // check for match
        if (length >= 3) { 
            int leb; int deb; 
            int length_code = get_length_code(length, &leb);
            int dist_code = get_dist_code(i - loc, &deb); 
            compressed[cindex++] = length_code; 
            compressed[cindex++] = leb; 
            compressed[cindex++] = dist_code;
            compressed[cindex++] = deb; 
            
            length_literal_freq[length_code]++;
            dist_freq[dist_code]++; 
            insert_table(chained_tab, hash, i); 
            i += length - 1; 
        } else {
            compressed[cindex++] = data[i]; 
            insert_table(chained_tab, hash, i); 
            length_literal_freq[data[i]]++; 
        }
    } 

    // add end of block 
    compressed[cindex++] = 0x100; 
    length_literal_freq[0x100]++; 
    *(final_size) = cindex; 

    return compressed; 
}

// varargs for return extra bits, and bit count
int get_length_code(int length, int* eb_val) {
    if (length >= 3 && length <= 10) return 254 + length;
    else if (length == 258) return 285; 
    else {
        int base = 11; 
        for (int i = 265; i < 285; i += 4) {
            int eb = ((i - 265) / 4) + 1; 
            int max = 0x00; 
            for (int j = 0; j < eb; j++) max = (max << 1) + 1;
            for (int j = i; j <= i + 3; j++) {
                if (length >= base && length <= base + max) { 
                    *eb_val = length - base; 
                    return j; 
                } else base += max + 1; 
            }
        }
    }
    // no length code found 
    return -1; 
}

// varargs for return extra bits, and bit count
int get_dist_code(int dist, int* eb_val) {
    if (dist >= 1 && dist <= 4) return dist - 1;
    else {
        int base = 5; 
        for (int i = 4; i < 29; i += 2) {
            int eb = ((i - 4) / 2) + 1; 
            int max = 0x00; 
            for (int j = 0; j < eb; j++) max = (max << 1) + 1; 
            for (int j = i; j < i + 2; j++) {
                if (dist >= base && dist <= base + max) {
                    *eb_val = dist - base; 
                    return j; 
                } else base += max + 1; 
            }
        }
    }
    // no dist code found 
    return -1;
}