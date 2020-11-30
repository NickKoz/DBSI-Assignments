#ifndef RECORD
#define RECORD

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <assert.h>
#include <stdbool.h>

#define BUFFER_SIZE 256


extern int num_of_records; 

typedef struct{
    
    int id;
    char name[15];
    char surname[25];
    char address[50];

}Record;


void Record_print(Record*);
bool Record_is_empty(Record*);

Record* Record_extract(char*);
void Record_delete(Record*);

#endif