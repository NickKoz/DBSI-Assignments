#ifndef RECORD
#define RECORD

#include <stdio.h>
#include <string.h>
#include <stdbool.h>

typedef struct{
    
    int id;
    char name[15];
    char surname[25];
    char address[50];

}Record;


void Record_print(Record*);
bool Record_is_empty(Record*);


#endif