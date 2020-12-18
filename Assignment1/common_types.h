#ifndef COMMON
#define COMMON

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <assert.h>
#include <stdbool.h>
#include <unistd.h>
#include <time.h>
#include "BF.h"

#include "Record.h"

// floor(BLOCK_SIZE/sizeof(Record))
#define MAX_RECORDS (int)(BLOCK_SIZE/sizeof(Record))

#define BUFFER_SIZE 256

#define FINAL_BLOCK -1




// Structure that is used to cast the fetched block.
typedef struct{

    int index;
    Record records[MAX_RECORDS];
    int num_of_records;
    int next;
    
}Block_info;




#endif