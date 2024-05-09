#ifndef DATETIME_H
#define DATETIME_H
#include <stdint.h>

typedef struct date_s{
    unsigned int day : 5; 
    unsigned int month : 4;
    unsigned int year : 7; 
} DATE_T; 

typedef struct time_s {
    unsigned int second : 5;
    unsigned int min : 6; 
    unsigned int hour : 5; 
} TIME_T; 

uint16_t get_mod_date(char* fn);
uint16_t get_mod_time(char* fn);
#endif 