#include "Heap.h"

char messages[256];

int HP_CreateFile(char* fileName, char attrType, char* attrName, int attrLength){

    BF_Init();

    if(BF_CreateFile(fileName) < 0){
        BF_PrintError(messages);
        return -1;
    }

    int fd = BF_OpenFile(fileName);
    if(fd < 0){
        BF_PrintError(messages);
        return -1;
    }

    if(BF_AllocateBlock(fd) < 0){
        BF_PrintError(messages);
        return -1;
    }

    // Header for new file.
    HP_info header = {.fileDesc = fd, .attrType = attrType,
    .attrLength = attrLength};
    header.attrName = malloc(sizeof(attrName) + 1);
    strcpy(header.attrName, attrName);

    void* block;

    if(BF_ReadBlock(fd, 0, &block)){
        BF_PrintError(messages);
        return -1;
    }

    memcpy(block, &header, sizeof(HP_info));

    if(BF_WriteBlock(fd, 0) < 0){
        BF_PrintError(messages);
        return -1;
    }

    return 0;
}


HP_info* HP_OpenFile(char* fileName){

    int fd = BF_OpenFile(fileName);
    if(fd < 0){
        BF_PrintError(messages);
        return NULL;
    }

    HP_info* header = malloc(sizeof(HP_info));
    void* block;

    if(BF_ReadBlock(fd, 0, &block)){
        BF_PrintError(messages);
        return NULL;
    }

    HP_info* temp = block;
    
    header->fileDesc = temp->fileDesc;
    header->attrType = temp->attrType;
    header->attrName = malloc(sizeof(temp->attrName));
    strcpy(header->attrName, temp->attrName);
    header->attrLength = temp->attrLength;

    return header;
}



int HP_CloseFile(HP_info* hp_info){


    if(BF_CloseFile(hp_info->fileDesc) < 0){
        BF_PrintError(messages);
        return -1;
    }

    free(hp_info->attrName);
    free(hp_info);

    return 0;
}


int HP_InsertEntry(HP_info hp, Record record){

    void* block;
    Block_info* curr_block;

    int num_of_blocks = BF_GetBlockCounter(hp.fileDesc);

    bool record_inserted = false;
    int i;
    for(i = 1 ; i < num_of_blocks ; i++){

        if(BF_ReadBlock(hp.fileDesc, i, &block) < 0){
            BF_PrintError(messages);
            return -1;
        }

        curr_block = block;
        

        if(curr_block->num_of_records == MAX_RECORDS){
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

        if(BF_AllocateBlock(hp.fileDesc) < 0){
            BF_PrintError(messages);
            return -1;
        }

        if(BF_ReadBlock(hp.fileDesc, i, &block)){
            BF_PrintError(messages);
            return -1;
        }

        curr_block = block;

        curr_block->index = i;
        curr_block->next = 0;
        curr_block->num_of_records = 1;
        memcpy(&curr_block->records[0], &record, sizeof(Record));

        memcpy(block, curr_block, sizeof(Block_info));


    }


    if(BF_WriteBlock(hp.fileDesc, i) < 0){
        BF_PrintError(messages);
        return -1;
    }


    Block_info_print(curr_block);
    

    return 0;
}



void Block_info_print(Block_info* b){

    printf("Index: %d\n", b->index);
    for(int i = 0 ; i < b->num_of_records ; i++){
        printf("ID: %d\n", b->records[i].id);
        printf("Name: %s\n", b->records[i].name);
        printf("Surname: %s\n", b->records[i].surname);
        printf("Address: %s\n", b->records[i].address);
    }
    printf("Num of records: %d\n\n\n", b->num_of_records);

}



void HP_info_print(HP_info* hp){

    printf("File descriptor: %d\n",hp->fileDesc);
    printf("Attribute type: %c\n",hp->attrType);
    printf("Attribute name: %s\n",hp->attrName);
    printf("Attribute length: %d\n",hp->attrLength);

}



