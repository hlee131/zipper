#ifndef WRITER_H
#include <stdio.h>
#define WRITER_H

typedef enum WRITE_TYPE_E { FILE_HDR, DIR_HDR, EOCD } WRITE_TYPE_T;

void write_to_file(FILE* fptr, void* to_write, WRITE_TYPE_T wt);
#endif 