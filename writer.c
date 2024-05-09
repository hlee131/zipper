#include <byteswap.h> 
#include <stddef.h> 
#include <string.h> 

#include "includes/writer.h"
#include "includes/helper.h"

#define FH_LAST_OFFSET offsetof(file_hdr_t, fn) 
#define DIR_LAST_OFFSET offsetof(dir_hdr_t, fn)
#define EOCD_LAST_OFFSET offsetof(eocd_t, comment_size) 

static int file_hdr_offsets[] = {
    offsetof(file_hdr_t, signature), 4, 
    offsetof(file_hdr_t, min_version), 2,
    offsetof(file_hdr_t, bit_flag), 2,
    offsetof(file_hdr_t, compression_method), 2, 
    offsetof(file_hdr_t, mod_time), 2,
    offsetof(file_hdr_t, mod_date), 2,
    offsetof(file_hdr_t, checksum), 4,
    offsetof(file_hdr_t, compress_size), 4,
    offsetof(file_hdr_t, uncompress_size), 4,
    offsetof(file_hdr_t, fn_size), 2,
    offsetof(file_hdr_t, field_size), 2, 
    FH_LAST_OFFSET
}; 

static int dir_hdr_offsets[] = {
    offsetof(dir_hdr_t, signature), 4,
    offsetof(dir_hdr_t, made_by), 2,
    offsetof(dir_hdr_t, min_version), 2, 
    offsetof(dir_hdr_t, bit_flag), 2, 
    offsetof(dir_hdr_t, compression_method), 2, 
    offsetof(dir_hdr_t, mod_time), 2, 
    offsetof(dir_hdr_t, mod_date), 2,
    offsetof(dir_hdr_t, checksum), 4, 
    offsetof(dir_hdr_t, compress_size), 4, 
    offsetof(dir_hdr_t, uncompress_size), 4,
    offsetof(dir_hdr_t, fn_size), 2,
    offsetof(dir_hdr_t, field_size), 2,
    offsetof(dir_hdr_t, comment_size), 2,
    offsetof(dir_hdr_t, disk_num), 2,
    offsetof(dir_hdr_t, intern_attr), 2,
    offsetof(dir_hdr_t, extern_attr), 4, 
    offsetof(dir_hdr_t, offset), 4, 
    DIR_LAST_OFFSET
};

static int eocd_offsets[] = {
    offsetof(eocd_t, signature), 4,
    offsetof(eocd_t, disk_num), 2,
    offsetof(eocd_t, cdir_start), 2,
    offsetof(eocd_t, cdir_amt), 2,
    offsetof(eocd_t, cdir_total), 2,
    offsetof(eocd_t, cdir_size), 4, 
    offsetof(eocd_t, cdir_offset), 4, 
    EOCD_LAST_OFFSET
};

void write_to_file(FILE* fptr, void* to_write, WRITE_TYPE_T wt) {
    int* offsets;
    int offset_counts; 

    switch (wt) {
        case FILE_HDR: 
            offsets = file_hdr_offsets;
            offset_counts = 20;
            break; 
        case DIR_HDR:
            offsets = dir_hdr_offsets;
            offset_counts = 32; 
            break; 
        case EOCD: 
            offsets = eocd_offsets;
            offset_counts = 14;
            break; 
        default: break; 
    }
    
    for (int i = 0; i <= offset_counts; i++) {
        switch (offsets[++i]) {
            case 2: {
                uint16_t swapped = (*((uint16_t*) (to_write + offsets[i - 1]))); 
                fwrite(&swapped, sizeof(uint16_t), 1, fptr); 
                break;
            }
            case 4: {
                uint32_t swapped = (*((uint32_t*) (to_write + offsets[i - 1])));
                fwrite(&swapped, sizeof(uint32_t), 1, fptr); 
                break;
            }
            default: continue; 
        }
    }

    // write last field 
    if (wt != EOCD) {
        char* str = * ((char**) (to_write + (wt == FILE_HDR ? FH_LAST_OFFSET : DIR_LAST_OFFSET )));
        fwrite(str, sizeof(char), strlen(str), fptr);   
    } else fwrite(to_write + EOCD_LAST_OFFSET, sizeof(uint16_t), 1, fptr); 
}

