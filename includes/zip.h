#ifndef ZIP_H
#define ZIP_H

void zip_file(char* fn, FILE* outptr, list_t* cdir);
void write_cdirs(FILE* outfile, list_t* cdir);

#endif 