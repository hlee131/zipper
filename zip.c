#include <stdio.h> 
#include <stdlib.h> 
#include <stdint.h> 
#include <string.h> 
#include <sys/stat.h> 
#include <dirent.h> 

#include "includes/helper.h"
#include "includes/deflate.h"
#include "includes/collections.h"
#include "includes/crc.h"
#include "includes/datetime.h"
#include "includes/zip.h"
#include "includes/writer.h"

int main(int argc, char* argv[]) {
    if (argc != 2) {
        puts("error: expected one file to compress"); 
        return 1; 
    }

    list_t* cdirs = new_list(); 
    eocd_t* eocd = new_eocd(); 
    
    // open out file
    FILE* outptr = fopen("out.zip", "wb");
    if (outptr == NULL) exit(1);

    // zip the file 
    zip_file(argv[1], outptr, cdirs); 

    // populate eocd 
    eocd->cdir_amt = eocd->cdir_total = cdirs->length;
    eocd->cdir_offset = ftell(outptr); 
    
    write_cdirs(outptr, cdirs); 
    eocd->cdir_size = ftell(outptr) - eocd->cdir_offset; 
    write_to_file(outptr, eocd, EOCD); 

    // // close out file stream
    fclose(outptr); 
    
}

void zip_file(char* fn, FILE* outptr, list_t* cdir) {
    // check if file is a directory 
    struct stat s; 
    if (stat(fn, &s) == 0 && S_ISDIR(s.st_mode)) {
        // iterate over files and recursively call zip_file 
        // get file name and concatenate to current file 
        DIR *dir;
        struct dirent *entry;
        if (dir = opendir(fn)) {
            while ((entry = readdir(dir)) != NULL) {
                char* new_fn = checked_malloc(strlen(fn) + strlen(entry->d_name) + 2);
                if (strcmp(".", entry->d_name) == 0 || strcmp("..", entry->d_name) == 0) 
                    continue; 
                strcpy(new_fn, fn); 
                strcat(new_fn, "/"); 
                strcat(new_fn, entry->d_name); 
                zip_file(new_fn, outptr, cdir); 
            }   
        }
        closedir(dir);
    } else {
        // read file into buffer
        FILE* inptr = fopen(fn, "rb");
        if (inptr == NULL) exit(EXIT_FAILURE);
        fseek(inptr, 0, SEEK_END); 
        long length = ftell(inptr);
        rewind(inptr); 
        uint8_t* raw_bytes = (uint8_t*) checked_malloc(sizeof(uint8_t) * length); 
        size_t file_size = length; 
        fread(raw_bytes, file_size, 1, inptr);
        fclose(inptr); 
        
        // initialize headers
        file_hdr_t* hdr = new_file_hdr();  
        dir_hdr_t* dir = new_dir_hdr(); 
        int compressed_size; 
    
        // populate fields 
        hdr->uncompress_size = dir->uncompress_size = file_size;  
        uint8_t* compressed = deflate(raw_bytes, hdr->uncompress_size, &compressed_size);
        hdr->compress_size = dir->compress_size = compressed_size; 
        hdr->checksum = dir->checksum = crc32(raw_bytes, hdr->uncompress_size);
        hdr->fn_size = dir->fn_size = strlen(fn);
        hdr->field_size = dir->field_size = 0;
        hdr->fn = dir->fn = fn;  
        
        // store last modified into local header and central directory  
        hdr->mod_date = dir->mod_date = get_mod_date(hdr->fn); 
        hdr->mod_time = dir->mod_time = get_mod_time(hdr->fn); 
        
        // write header and compressed data to outfile 
        uint32_t offset = ftell(outptr); 
    
        // finish populating central directory 
        dir->comment_size = 0; 
        dir->disk_num = 0; 
        dir->intern_attr = dir->extern_attr = 0; 
        dir->offset = offset; 

        // write into file 
        write_to_file(outptr, hdr, FILE_HDR); 
        fwrite(compressed, sizeof(uint8_t), compressed_size, outptr); 
        printf("compressed %s by %.3f%%\n", fn, (((file_size - compressed_size) / (double) file_size) * 100.0)); 
        
        // free dynamically allocated memory 
        free(hdr); 
        free(raw_bytes);
        free(compressed);
        
        // add central directory to list 
        insert_ptr(cdir, dir); 
    }
}

void write_cdirs(FILE* outfile, list_t* cdir) {
    node_t* next = NULL; 
    node_t* curr = cdir->head; 
    while (curr) { 
        dir_hdr_t* dir = (dir_hdr_t*) curr->data.ptr;
        next = curr->next; 
        // free node 
        free(curr); 
        curr = next; 
        // write dir
        write_to_file(outfile, dir, DIR_HDR); 
        // free central directory 
        free(dir); 
    }
    // free linked list 
    free(cdir); 
}

