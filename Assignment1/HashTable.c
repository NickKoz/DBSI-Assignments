#include "HashTable.h"
#include "Heap.h"

char error_mess[256];

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

    for(int i = 0 ; i < buckets ; i++){

        if(BF_AllocateBlock(fd) < 0){
            BF_PrintError(error_mess);
            return -1;
        }

        if(BF_ReadBlock(fd, i+1, &block) < 0){
            BF_PrintError(error_mess);
            return -1;
        }

        curr_block = block;

        curr_block->index = i;
        curr_block->next = -1;  // Next is NULL
        curr_block->num_of_records = 0;

        memcpy(block, curr_block, sizeof(Block_info));

        if(BF_WriteBlock(fd, i+1) < 0){
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



int hash_string(char* key, int buckets){

    int hash=0, a=33;
    char* temp = key;

    for(; *temp!='\0' ; temp++)
        hash = (a*hash + *temp) % buckets;

    return hash;
}


int hash_int(int key, int buckets){
    return key % buckets;
}


int HT_InsertEntry(HT_info ht, Record record){

    void* block;
    Block_info* curr_block;
    int h;

    if(!strcmp(ht.attrName, "id")){
        h = hash_int(record.id, ht.numBuckets);
    }
    else if(!strcmp(ht.attrName, "name")){
        h = hash_string(record.name, ht.numBuckets);
    }
    else if(!strcmp(ht.attrName, "surname")){
        h = hash_string(record.surname, ht.numBuckets);
    }
    else if(!strcmp(ht.attrName, "address")){
        h = hash_string(record.surname, ht.numBuckets);
    }

    bool record_inserted = false;
    int i = h + 1;
    while(true){

        if(BF_ReadBlock(ht.fileDesc, i, &block) < 0){
            BF_PrintError(error_mess);
            return -1;
        }

        curr_block = block;
        

        if(curr_block->num_of_records == MAX_RECORDS){
            if(curr_block->next == -1){
                break;
            }
            i = curr_block->next;
            continue;
        }

        // Checking if there is available position in the records in block i. 
        int j;
        for(j = 0 ; j < MAX_RECORDS ; j++){
            if(!strcmp(curr_block->records[j].name, "")){
                break;
            }
        }
        
        // If we have available space in block, we insert the given record.

        memcpy(&curr_block->records[j], &record, sizeof(Record));
        curr_block->num_of_records++;
        
        memcpy(block, curr_block, sizeof(Block_info));

        record_inserted = true;

        break;

    }


    if(!record_inserted){

        curr_block->next = BF_GetBlockCounter(ht.fileDesc);

        // memcpy ??

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

        curr_block->index = BF_GetBlockCounter(ht.fileDesc) - 2;
        curr_block->next = -1;
        curr_block->num_of_records = 1;
        memcpy(&curr_block->records[0], &record, sizeof(Record));

        memcpy(block, curr_block, sizeof(Block_info));

        i = BF_GetBlockCounter(ht.fileDesc) - 1;
    }

    if(BF_WriteBlock(ht.fileDesc, i) < 0){
        BF_PrintError(error_mess);
        return -1;
    }

    Block_info_print(curr_block);

}




void HT_info_print(HT_info* hp){

    printf("File descriptor: %d\n",hp->fileDesc);
    printf("Attribute type: %c\n",hp->attrType);
    printf("Attribute name: %s\n",hp->attrName);
    printf("Attribute length: %d\n",hp->attrLength);
    printf("Num of buckets: %ld\n",hp->numBuckets);

}
