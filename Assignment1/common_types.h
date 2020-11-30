#ifndef COMMON
#define COMMON

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <assert.h>
#include <stdbool.h>
#include "BF.h"

// floor(BLOCK_SIZE/sizeof(Record))
#define MAX_RECORDS 5

#define BUFFER_SIZE 256

#include "Record.h"


typedef struct{

    int index;
    Record records[MAX_RECORDS];
    int num_of_records;
    int next;
    
}Block_info;




#endif