
#ifndef HELPER_H
#define HELPER_H
#include <stdint.h>
#include <stdio.h> 
#include <time.h>
#include <sys/stat.h> 

#include "collections.h"
#include "datetime.h"

// TODO: size in bytes
#define len(ptr, type) (sizeof(*ptr) / sizeof(type))
#define MAX(x, y) ((x > y ? x : y))
#define MIN(x, y) ((x < y ? x : y))

enum COMPRESS_TYPE { NONE = 0, DEFLATE = 0x0008 };  

typedef struct file_hdr_s {
    uint32_t signature; 
    uint16_t min_version; 
    uint16_t bit_flag;
    uint16_t compression_method; 
    uint16_t mod_time;
    uint16_t mod_date; 
    uint32_t checksum; 
    uint32_t compress_size; 
    uint32_t uncompress_size; 
    uint16_t fn_size;
    uint16_t field_size;
    char* fn;
    char* extra_field; 
} file_hdr_t; 

typedef struct dir_hdr_s {
    uint32_t signature; 
    uint16_t made_by; 
    uint16_t min_version; 
    uint16_t bit_flag;
    uint16_t compression_method; 
    uint16_t mod_time;
    uint16_t mod_date; 
    uint32_t checksum; 
    uint32_t compress_size; 
    uint32_t uncompress_size; 
    uint16_t fn_size;
    uint16_t field_size;
    uint16_t comment_size;
    uint16_t disk_num;
    uint16_t intern_attr;
    uint32_t extern_attr;
    uint32_t offset; 
    char* fn;
    char* extra_field; 
    char* file_comment; 
} dir_hdr_t; 

typedef struct eocd_s {
    uint32_t signature;
    uint16_t disk_num; 
    uint16_t cdir_start;
    uint16_t cdir_amt; 
    uint16_t cdir_total; 
    uint32_t cdir_size; 
    uint32_t cdir_offset; 
    uint16_t comment_size;
    char* comment; 
} eocd_t; 

typedef struct file_s {
    char* fn; 
    uint8_t* bytes; 
    unsigned int size; 
} file_t; 

eocd_t* new_eocd(); 
dir_hdr_t* new_dir_hdr();
file_hdr_t* new_file_hdr(); 
void write_eocd(FILE* f, eocd_t* eocd);  
void write_dir_hdr(FILE* f, dir_hdr_t* dir_hdr);  
void write_file_hdr(FILE* f, file_hdr_t* file_hdr); 
// d_descriptor_t* new_descriptor(uint32_t crc, uint32_t csize, uint32_t usize); 
void* checked_malloc(size_t size); 

#endif 