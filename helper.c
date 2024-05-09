#include <stdlib.h>
#include <string.h>

#include "includes/crc.h"
#include "includes/helper.h"

void* checked_malloc(size_t size) {
    void* ptr = malloc(size); 
    if (ptr) return ptr; 
    exit(1); 
}

eocd_t* new_eocd() {
    eocd_t* ptr = checked_malloc(sizeof(eocd_t));
    ptr->signature = 0x06054b50;
    ptr->disk_num = 0; 
    ptr->cdir_start = 0; 
    return ptr; 
}

dir_hdr_t* new_dir_hdr() {
    dir_hdr_t* ptr = checked_malloc(sizeof(dir_hdr_t));
    ptr->signature = 0x02014b50; 
    ptr->min_version = 2;
    ptr->compression_method = DEFLATE; 
    ptr->made_by = 0x31e; 
    ptr->bit_flag = 2048; 
    return ptr; 
}

file_hdr_t* new_file_hdr() {
    file_hdr_t* ptr = checked_malloc(sizeof(file_hdr_t));
    ptr->signature = 0x04034b50;
    ptr->min_version = 2; 
    ptr->compression_method = DEFLATE;
    ptr->bit_flag = 2048; 
    return ptr; 
}