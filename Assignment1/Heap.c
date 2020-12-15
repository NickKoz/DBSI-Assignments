#include "Heap.h"

char error_mess[BUFFER_SIZE];

int HP_CreateFile(char* fileName, char attrType, char* attrName, int attrLength){

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
    HP_info header = {.fileDesc = fd, .attrType = attrType,
    .attrLength = attrLength};
    header.attrName = strdup(attrName);

    void* block;

    if(BF_ReadBlock(fd, 0, &block) < 0){
        BF_PrintError(error_mess);
        return -1;
    }

    memcpy(block, &header, sizeof(HP_info));

    if(BF_WriteBlock(fd, 0) < 0){
        BF_PrintError(error_mess);
        return -1;
    }

    free(header.attrName);

    return 0;
}


HP_info* HP_OpenFile(char* fileName){

    int fd = BF_OpenFile(fileName);
    if(fd < 0){
        BF_PrintError(error_mess);
        return NULL;
    }

    HP_info* header = malloc(sizeof(HP_info));
    void* block;

    if(BF_ReadBlock(fd, 0, &block) < 0){
        BF_PrintError(error_mess);
        return NULL;
    }

    HP_info* temp = block;
    
    header->fileDesc = fd;
    header->attrType = temp->attrType;
    header->attrName = strdup(temp->attrName);
    header->attrLength = temp->attrLength;

    memcpy(block, header, sizeof(Block_info));

    if(BF_WriteBlock(fd, 0) < 0){
        BF_PrintError(error_mess);
        return NULL;
    }

    return header;
}



int HP_CloseFile(HP_info* hp_info){


    if(BF_CloseFile(hp_info->fileDesc) < 0){
        BF_PrintError(error_mess);
        return -1;
    }

    free(hp_info->attrName);
    free(hp_info);

    return 0;
}


static int overwrite_index_structure(HP_info* hp){

    if(BF_GetBlockCounter(hp->fileDesc) == 22 || BF_GetBlockCounter(hp->fileDesc) == 23){
        void* block;
        for(int i = 1 ; i < BF_GetBlockCounter(hp->fileDesc) ; i++){
            if(BF_ReadBlock(hp->fileDesc, i, &block) < 0){
                BF_PrintError(error_mess);
                return -1;
            }
        }
    }
    return 1;
}


static bool check_for_duplicates(HP_info* hp, int id){

    void* block;
    Block_info* curr_block;

    overwrite_index_structure(hp);

    int num_of_blocks = BF_GetBlockCounter(hp->fileDesc);

    // Iterating each block.
    for(int i = 0 ; i < num_of_blocks ; i++){

        if(BF_ReadBlock(hp->fileDesc, i, &block) < 0){
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

    }

    return false;
}



int HP_InsertEntry(HP_info hp, Record record){

    // if(check_for_duplicates(&hp, record.id)){
    //     printf("Record already exists!\n");
    //     return -1;
    // }


    void* block;
    Block_info* curr_block;

    int num_of_blocks = BF_GetBlockCounter(hp.fileDesc);

    overwrite_index_structure(&hp);

    

    int block_ID;

    // Iterating every block in the file.
    int i;
    for(i = 1 ; i < num_of_blocks ; i++){

        if(BF_ReadBlock(hp.fileDesc, i, &block) < 0){
            BF_PrintError(error_mess);
            return -1;
        }

        curr_block = block;

        // If block is full, goes to next one.
        if(curr_block->num_of_records == MAX_RECORDS){
            continue;
        }

        // Finding available position for the record in block i. 
        int j;
        for(j = 0 ; j < MAX_RECORDS ; j++){
            if(!strcmp(curr_block->records[j].name, "")){
                break;
            }
        }
        
        // Inserting the given record.

        memcpy(&curr_block->records[j], &record, sizeof(Record));
        curr_block->num_of_records++;

        
        if(BF_WriteBlock(hp.fileDesc, i) < 0){
            BF_PrintError(error_mess);
            return -1;
        }


        block_ID = i;

        return block_ID;

    }
    

    // If there is no position in the existing blocks, creates a new block.

    //sets next pointer for the previous block
    if(num_of_blocks > 1){
        curr_block->next = BF_GetBlockCounter(hp.fileDesc);

        memcpy(block, curr_block, sizeof(Block_info));

        if(BF_WriteBlock(hp.fileDesc, num_of_blocks - 1) < 0){
            BF_PrintError(error_mess);
            return -1;
        }
    }
    

    if(BF_AllocateBlock(hp.fileDesc) < 0){
        BF_PrintError(error_mess);
        return -1;
    }

    num_of_blocks = BF_GetBlockCounter(hp.fileDesc);


    if(BF_ReadBlock(hp.fileDesc, num_of_blocks - 1, &block) < 0){
        BF_PrintError(error_mess);
        return -1;
    }

    curr_block = block;

    curr_block->index = num_of_blocks - 1;
    curr_block->next = FINAL_BLOCK;
    curr_block->num_of_records = 1;
    memcpy(&curr_block->records[0], &record, sizeof(Record));


    block_ID = curr_block->index;



    if(BF_WriteBlock(hp.fileDesc, num_of_blocks - 1) < 0){
        BF_PrintError(error_mess);
        return -1;
    }


    return block_ID;
}


int HP_DeleteEntry(HP_info hp, void* value){

    void* block;
    Block_info* curr_block;

    int num_of_blocks = BF_GetBlockCounter(hp.fileDesc);

    // Iterating every block in the file.
    for(int i = 1 ; i < num_of_blocks ; i ++){

        if(BF_ReadBlock(hp.fileDesc, i, &block) < 0){
            BF_PrintError(error_mess);
            return -1;
        }

        curr_block = block;
        for(int j = 0 ; j < MAX_RECORDS ; j++){

            bool is_found = false;

            if(Record_is_empty(&curr_block->records[j])){
                continue;
            }


            if(curr_block->records[j].id == *(int*)value){
                is_found = true;
            }
            
            // If record is found, puts zeros in record's position.
            if(is_found){
                memset(&curr_block->records[j], 0, sizeof(Record));
                curr_block->num_of_records--;
                if(BF_WriteBlock(hp.fileDesc, i) < 0){
                    BF_PrintError(error_mess);
                    return -1;
                }
                return 0;
            }

        }
    }

    // If deleting doesn't happen, we return error code -1.
    return -1;

}




int HP_GetAllEntries(HP_info hp, void* value){

    void* block;
    Block_info* curr_block;
    int return_value = -1;

    overwrite_index_structure(&hp);

    int num_of_blocks = BF_GetBlockCounter(hp.fileDesc);

    // Iterating each block.
    for(int i = 1 ; i < num_of_blocks ; i ++){

        if(BF_ReadBlock(hp.fileDesc, i, &block) < 0){
            BF_PrintError(error_mess);
            return -1;
        }


        curr_block = block;


        for(int j = 0 ; j < MAX_RECORDS ; j++){

            if(Record_is_empty(&curr_block->records[j])){
                continue;
            }

            if(value != NULL){
                // If record is found, prints it.
                if(curr_block->records[j].id == *(int*)value){
                    Record_print(&curr_block->records[j]);
                    return i;
                }
            }
            else{
                Record_print(&curr_block->records[j]);
                return_value = i;
            }
   
        }
    }

    return return_value;
}







void Block_info_print(Block_info* b){


    printf("Index: %d\n", b->index);
    printf("Next: %d\n", b->next);
    // for(int i = 0 ; i < MAX_RECORDS ; i++){
    //     // if(!strcmp(b->records[i].name, "")){
    //     //     printf("NULL\n");
    //     //     continue;
    //     // }
    //     printf("ID: %d\n", b->records[i].id);
    //     printf("Name: %s\n", b->records[i].name);
    //     printf("Surname: %s\n", b->records[i].surname);
    //     printf("Address: %s\n", b->records[i].address);
    // }
    printf("Num of records: %d\n\n\n", b->num_of_records);

}



void HP_info_print(HP_info* hp){

    printf("File descriptor: %d\n",hp->fileDesc);
    printf("Attribute type: %c\n",hp->attrType);
    printf("Attribute name: %s\n",hp->attrName);
    printf("Attribute length: %d\n",hp->attrLength);

}



