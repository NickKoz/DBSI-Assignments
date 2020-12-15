#include "HashTable.h"
#include "Heap.h"

char error_mess[BUFFER_SIZE];

int HT_CreateIndex(char* fileName, char attrType, char* attrName, int attrLength, int buckets){

    BF_Init();

    if(BF_CreateFile(fileName) < 0){
        BF_PrintError(error_mess);
        return -1;
    }

    int fd = BF_OpenFile(fileName);
    if(fd < 0){
        BF_PrintError(error_mess);
        return -1;
    }

    if(BF_AllocateBlock(fd) < 0){
        BF_PrintError(error_mess);
        return -1;
    }

    // Header for new file.
    HT_info header = {.fileDesc = fd, .attrType = attrType,
    .attrLength = attrLength, .numBuckets = buckets};
    header.attrName = malloc(sizeof(attrName) + 1);
    strcpy(header.attrName, attrName);

    void* block;

    if(BF_ReadBlock(fd, 0, &block) < 0){
        BF_PrintError(error_mess);
        return -1;
    }

    memcpy(block, &header, sizeof(HT_info));

    if(BF_WriteBlock(fd, 0) < 0){
        BF_PrintError(error_mess);
        return -1;
    }

    Block_info* curr_block;

    // Creates 1 block for each bucket
    for(int i = 1 ; i <= buckets ; i++){

        if(BF_AllocateBlock(fd) < 0){
            BF_PrintError(error_mess);
            return -1;
        }

        if(BF_ReadBlock(fd, i, &block) < 0){
            BF_PrintError(error_mess);
            return -1;
        }

        curr_block = block;

        curr_block->index = i;
        curr_block->next = FINAL_BLOCK;  // Next is NULL
        curr_block->num_of_records = 0;

        memcpy(block, curr_block, sizeof(Block_info));

        if(BF_WriteBlock(fd, i) < 0){
            BF_PrintError(error_mess);
            return -1;
        }

    }


    return 0;
}


HT_info* HT_OpenIndex(char* fileName){

    int fd = BF_OpenFile(fileName);
    if(fd < 0){
        BF_PrintError(error_mess);
        return NULL;
    }

    HT_info* header = malloc(sizeof(HT_info));
    void* block;

    if(BF_ReadBlock(fd, 0, &block) < 0){
        BF_PrintError(error_mess);
        return NULL;
    }

    HT_info* temp = block;
    
    header->fileDesc = fd;
    header->attrType = temp->attrType;
    header->attrName = malloc(sizeof(temp->attrName));
    strcpy(header->attrName, temp->attrName);
    header->attrLength = temp->attrLength;
    header->numBuckets = temp->numBuckets;

    // Writes header to block number 0.
    memcpy(block, header, sizeof(Block_info));
    if(BF_WriteBlock(fd, 0) < 0){
        BF_PrintError(error_mess);
        return NULL;
    }

    return header;
}


int HT_CloseIndex(HT_info* ht){

    if(BF_CloseFile(ht->fileDesc) < 0){
        BF_PrintError(error_mess);
        return -1;
    }

    free(ht->attrName);
    free(ht);

    return 0;
}



// Hash function for integers.
int hash_int(int key, int buckets){
    return key % buckets;
}



static int overwrite_index_structure(HT_info* ht){
    if(ht->numBuckets == 1){
        if(BF_GetBlockCounter(ht->fileDesc) == 22 || BF_GetBlockCounter(ht->fileDesc) == 23){
            void* block;
            for(int i = 1 ; i < BF_GetBlockCounter(ht->fileDesc) ; i++){
                if(BF_ReadBlock(ht->fileDesc, i, &block) < 0){
                    BF_PrintError(error_mess);
                    return -1;
                }
            }
        }
        return 1;
    }
    return 1;
}


static bool check_for_duplicates(HT_info* ht, int id){

    void* block;
    Block_info* curr_block;
    int h = hash_int(id, ht->numBuckets);

    // i is index to corresponding bucket's first block.
    int i = h + 1;  // Number 0 block is the header block.

    // Iterating each block.
    while(true){

        if(BF_ReadBlock(ht->fileDesc, i, &block) < 0){
            BF_PrintError(error_mess);
            return false;
        }

        curr_block = block;

        // Searching for record with given id in block.
        for(int j = 0 ; j < MAX_RECORDS ; j++){

            if(Record_is_empty(&curr_block->records[j])){
                continue;
            }

            if(curr_block->records[j].id == id){
                return true;
            }
        }

        i = curr_block->next;

        if(i == FINAL_BLOCK){
            break;
        }
    }

    return false;

}


int HT_InsertEntry(HT_info ht, Record record){

    if(check_for_duplicates(&ht, record.id)){
        printf("Record already exists!\n");
        return -1;
    }

    void* block;
    Block_info* curr_block;
    int h;

    overwrite_index_structure(&ht);

    h = hash_int(record.id, ht.numBuckets);
    
    bool record_inserted = false;

    // i is index to corresponding bucket's first block.
    int i = h + 1;  // Number 0 block is the header block.
  
    //Search for available position for the new record
    while(true){

        if(BF_ReadBlock(ht.fileDesc, i, &block) < 0){
            BF_PrintError(error_mess);
            return -1;
        }

        curr_block = block;

        //if this block is full, go to next block
        if(curr_block->num_of_records == MAX_RECORDS){
            if(curr_block->next == -1){
                break;
            }
            i = curr_block->next;
            continue;
        }

        //Searching for an available position in the records in block. 
        int j;
        for(j = 0 ; j < MAX_RECORDS ; j++){
            if(!strcmp(curr_block->records[j].name, "")){
                break;
            }
        }
        
        // If a position was found, insert the new record in it

        memcpy(&curr_block->records[j], &record, sizeof(Record));
        curr_block->num_of_records++;
        
        memcpy(block, curr_block, sizeof(Block_info));

        record_inserted = true;   

        break;

    }

    //If no available position was found, a new block has to be created
    if(!record_inserted){

        //setting the next "pointer" for the previous block
        curr_block->next = BF_GetBlockCounter(ht.fileDesc);

        memcpy(block, curr_block, sizeof(Block_info));

        if(BF_WriteBlock(ht.fileDesc, i) < 0){
            BF_PrintError(error_mess);
            return -1;
        }

        if(BF_AllocateBlock(ht.fileDesc) < 0){
            BF_PrintError(error_mess);
            return -1;
        }

        if(BF_ReadBlock(ht.fileDesc, BF_GetBlockCounter(ht.fileDesc) - 1, &block) < 0){
            BF_PrintError(error_mess);
            return -1;
        }

        curr_block = block;

        curr_block->index = BF_GetBlockCounter(ht.fileDesc) - 1;
        curr_block->next = FINAL_BLOCK;
        curr_block->num_of_records = 1;
        memcpy(&curr_block->records[0], &record, sizeof(Record));

        memcpy(block, curr_block, sizeof(Block_info));

        i = BF_GetBlockCounter(ht.fileDesc) - 1;  //i is the index of the new block
    }

    if(BF_WriteBlock(ht.fileDesc, i) < 0){
        BF_PrintError(error_mess);
        return -1;
    }

    // Block_info_print(curr_block);
    return i;
}



int HT_GetAllEntries(HT_info ht, void* value){

    void* block;
    Block_info* curr_block;
    int h = hash_int(*(int*)value, ht.numBuckets);

    int block_counter = -1, blocks = 0;

    // i is index to corresponding bucket's first block.
    int i = h + 1;  // Number 0 block is the header block.
    while(true){

        if(BF_ReadBlock(ht.fileDesc, i, &block) < 0){
            BF_PrintError(error_mess);
            return -1;
        }

        curr_block = block;

        // Searching every record in the block.
        for(int j = 0 ; j < MAX_RECORDS ; j++){

            if(Record_is_empty(&curr_block->records[j])){
                continue;
            }

            if(value != NULL){
                if(curr_block->records[j].id == *(int*)value){
                    block_counter = blocks + 1;
                    Record_print(&curr_block->records[j]);
                }
            }
            else{
                block_counter = blocks + 1;
                Record_print(&curr_block->records[j]);
            }

        }

        blocks++;

        i = curr_block->next;

        if(i == FINAL_BLOCK){
            break;
        }
    }

    return block_counter;

}


int HT_DeleteEntry(HT_info header_info, void* value){

    void* block;
    Block_info* curr_block;
    int h = hash_int(*(int*)value, header_info.numBuckets);
    
    bool is_found;
    // i is index to corresponding bucket's first block.
    int i = h + 1;  // Number 0 block is the header block.
    while(true){

        if(BF_ReadBlock(header_info.fileDesc, i, &block) < 0){
            BF_PrintError(error_mess);
            return -1;
        }

        curr_block = block;
        
        // Searching every record in the block.
        for(int j = 0 ; j < MAX_RECORDS ; j++){

            is_found = false;

            if(Record_is_empty(&curr_block->records[j])){
                continue;
            }

            if(curr_block->records[j].id == *(int*)value){
                is_found = true;
            }

            // If record was found, deletes the record.
            if(is_found){
                // Deleting record.
                memset(&curr_block->records[j], 0, sizeof(Record));

                curr_block->num_of_records--;
                memcpy(block, curr_block, sizeof(Block_info));
                if(BF_WriteBlock(header_info.fileDesc, i ) < 0){
                    BF_PrintError(error_mess);
                    return -1;
                }
            }
        }

        i = curr_block->next;

        // End of bucket.
        if(i == FINAL_BLOCK){
            break;
        }
    }

    if(!is_found)
        return -1;

    return 0;
}


int HashStatistics(char* filename){

    HT_info* ht = HT_OpenIndex(filename);

    printf("############################\n");
    printf("#      Hash Statistics     #\n");
    printf("############################\n\n");


    printf("Blocks: %d\n", BF_GetBlockCounter(ht->fileDesc));

    int buckets = ht->numBuckets;

    int records[buckets];
    int blocks[buckets];
    for(int i = 0 ; i < buckets ; i++){
        records[i] = 0;
        blocks[i] = 0;
    }


    int j;
    void* block;
    Block_info* curr_block;
    for(int i = 1 ; i <= buckets ; i++){
        
        j = i;
        while(true){

            if(BF_ReadBlock(ht->fileDesc, j, &block) < 0){
                BF_PrintError(error_mess);
                return -1;
            }
            
            curr_block = block;

            records[i-1] += curr_block->num_of_records;
            blocks[i-1]++;

            j = curr_block->next;

            if(j == FINAL_BLOCK){
                break;
            }
        }
        
    }

    int min = records[0];
    int max = records[0];
    int sum_records = 0;
    int sum_blocks = 0;

    for(int i = 0 ; i < buckets ; i++){

        if(records[i] > max){
            max = records[i];
        }

        if(records[i] < min){
            min = records[i];
        }

        sum_records += records[i];

        sum_blocks += blocks[i];
    }

    printf("Max number of records: %d\n", max);
    printf("Min number of records: %d\n", min);
    printf("Average number of records: %d\n\n", (int)sum_records/buckets);

    printf("Average number of blocks: %d\n\n", (int)sum_blocks/buckets);


    int overflowed_buckets = 0;
    for(int i = 0 ; i < buckets ; i++){
        if(blocks[i] > 1){
            printf("Bucket %d: %d\n", i, blocks[i] - 1);
            overflowed_buckets++;
        }
    }

    printf("\nOverflowed buckets: %d\n", overflowed_buckets);


    HT_CloseIndex(ht);
}





void HT_info_print(HT_info* hp){

    printf("File descriptor: %d\n",hp->fileDesc);
    printf("Attribute type: %c\n",hp->attrType);
    printf("Attribute name: %s\n",hp->attrName);
    printf("Attribute length: %d\n",hp->attrLength);
    printf("Num of buckets: %ld\n",hp->numBuckets);

}
