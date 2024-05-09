#include <sys/stat.h> 
#include <time.h>
#include <stdio.h>
#include <stdlib.h>
#include <errno.h> 

#include "includes/datetime.h"

/*
Docs for reference: 
https://docs.microsoft.com/en-us/windows/win32/api/winbase/nf-winbase-dosdatetimetofiletime
https://man7.org/linux/man-pages/man2/lstat.2.html
https://www.gnu.org/software/libc/manual/html_node/Time-Types.html
https://www.gnu.org/software/libc/manual/html_node/Broken_002ddown-Time.html
*/ 

/* 
ZIP files use MSDOS date format (first 16 bits)
Bits:    Description:  
0-4      Day of month 
5-8      Month 
9-15     Year from 1980

ZIP files use MSDOS time format (second 16 bits)
Bits:    Description:  
0-4      Seconds divided by 2 
5-10     Minute 
11-15    Hour 
*/
uint16_t get_mod_date(char* fn) {
    uint16_t date = 0; 
    struct stat attributes; 
    struct tm broken_down; 
    
    if (stat(fn, &attributes)) {
        perror("stat function failed");
        exit(EXIT_FAILURE); 
    }
    
    gmtime_r(&(attributes.st_mtim.tv_sec), &broken_down);

    date = broken_down.tm_mday; 
    date += broken_down.tm_mon + 1 << 5; 
    date += broken_down.tm_year - 80 << 4;  

    return date; 
}

uint16_t get_mod_time(char* fn) {
    uint16_t time = 0; 
    struct stat attributes; 
    struct tm broken_down; 
    
    if (stat(fn, &attributes)) {
        perror("stat function failed");
        exit(EXIT_FAILURE); 
    }
    
    gmtime_r(&(attributes.st_mtim.tv_sec), &broken_down);

    time = broken_down.tm_sec / 2; 
    time += broken_down.tm_min << 5; 
    time += broken_down.tm_hour << 6;  

    return time;
}

