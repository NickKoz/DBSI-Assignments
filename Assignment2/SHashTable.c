#include "SHashTable.h"

char error_mess[BUFFER_SIZE];

int SHT_CreateSecondaryIndex(char* sfileName, char* attrName, int attrLength, int buckets, char* fileName){

    if(BF_CreateFile(sfileName) < 0){
        BF_PrintError(error_mess);
        return -1;
    }
    
    int fd = BF_OpenFile(sfileName);
    if(fd < 0){
        BF_PrintError(error_mess);
        return -1;
    }

    if(BF_AllocateBlock(fd) < 0){
        BF_PrintError(error_mess);
        return -1;
    }

    // Header for new file.
    SHT_info header = {.fileDesc = fd, .attrLength = attrLength,
    .numBuckets = buckets};
    header.attrName = malloc(sizeof(attrName) + 1);
    strcpy(header.attrName, attrName);
    header.fileName = malloc(sizeof(fileName) + 1);
    strcpy(header.fileName, fileName);

    void* block;

    if(BF_ReadBlock(fd, 0, &block) < 0){
        BF_PrintError(error_mess);
        return -1;
    }

    memcpy(block, &header, sizeof(SHT_info));

    if(BF_WriteBlock(fd, 0) < 0){
        BF_PrintError(error_mess);
        return -1;
    }


    SHT_Block_info* curr_block;

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
        curr_block->num_of_entries = 0;

        memcpy(block, curr_block, sizeof(SHT_Block_info));

        if(BF_WriteBlock(fd, i) < 0){
            BF_PrintError(error_mess);
            return -1;
        }

    }

    if(BF_CloseFile(fd) < 0){
        BF_PrintError(error_mess);
        return -1;
    }

}


SHT_info* SHT_OpenSecondaryIndex(char* sfileName){

    int fd = BF_OpenFile(sfileName);
    if(fd < 0){
        BF_PrintError(error_mess);
        return NULL;
    }

    SHT_info* header = malloc(sizeof(SHT_info));
    void* block;

    if(BF_ReadBlock(fd, 0, &block) < 0){
        BF_PrintError(error_mess);
        return NULL;
    }

    SHT_info* temp = block;
    
    header->fileDesc = fd;
    header->attrName = strdup(temp->attrName);
    header->attrLength = temp->attrLength;
    header->numBuckets = temp->numBuckets;
    header->fileName = strdup(temp->fileName);

    // Writes header to block number 0.
    memcpy(block, header, sizeof(SHT_Block_info));
    if(BF_WriteBlock(fd, 0) < 0){
        BF_PrintError(error_mess);
        return NULL;
    }

    return header;

}


int SHT_CloseSecondaryIndex(SHT_info* header_info){

    if(BF_CloseFile(header_info->fileDesc) < 0){
        BF_PrintError(error_mess);
        return -1;
    }

    free(header_info->attrName);
    free(header_info->fileName);
    free(header_info);

    return 0;
}





int hash_string(char* K, long int hash_size){
	
	int h=0,a=33;
	
	for(; *K!='\0' ; K++)
		h=(a*h + *K)%hash_size;
	
	return h;
	
}



int SHT_SecondaryInsertEntry(SHT_info header_info, SecondaryRecord srecord){

    void* block;
    SHT_Block_info* curr_block;

    SHT_entry temp_entry = {0};
    strcpy(temp_entry.surname, srecord.record.surname);
    temp_entry.blockId = srecord.blockId;

    int h = hash_string(srecord.record.surname, header_info.numBuckets);
    
    bool record_inserted = false;

    // i is index to corresponding bucket's first block.
    int i = h + 1;  // Number 0 block is the header block.
  
    //Search for available position for the new record
    while(true){

        if(BF_ReadBlock(header_info.fileDesc, i, &block) < 0){
            BF_PrintError(error_mess);
            return -1;
        }

        curr_block = block;

        //if this block is full, go to next block
        if(curr_block->num_of_entries == MAX_ENTRIES){
            if(curr_block->next == FINAL_BLOCK){
                break;
            }
            i = curr_block->next;
            continue;
        }

        //Searching for an available position in the records in block. 
        int j;
        for(j = 0 ; j < MAX_ENTRIES ; j++){
            if(!strcmp(curr_block->entries[j].surname, "")){
                break;
            }
        }
        
        // If a position was found, insert the new record in it
        memcpy(&curr_block->entries[j], &temp_entry, sizeof(SHT_entry));

        curr_block->num_of_entries++;
        
        memcpy(block, curr_block, sizeof(SHT_Block_info));

        record_inserted = true;   

        break;

    }

    //If no available position was found, a new block has to be created
    if(!record_inserted){

        //setting the next "pointer" for the previous block
        curr_block->next = BF_GetBlockCounter(header_info.fileDesc);

        memcpy(block, curr_block, sizeof(SHT_Block_info));

        if(BF_WriteBlock(header_info.fileDesc, i) < 0){
            BF_PrintError(error_mess);
            return -1;
        }

        if(BF_AllocateBlock(header_info.fileDesc) < 0){
            BF_PrintError(error_mess);
            return -1;
        }

        if(BF_ReadBlock(header_info.fileDesc, BF_GetBlockCounter(header_info.fileDesc) - 1, &block) < 0){
            BF_PrintError(error_mess);
            return -1;
        }

        curr_block = block;

        curr_block->index = BF_GetBlockCounter(header_info.fileDesc) - 1;
        curr_block->next = FINAL_BLOCK;
        curr_block->num_of_entries = 1;

        memcpy(&curr_block->entries[0], &temp_entry, sizeof(SHT_entry));

        memcpy(block, curr_block, sizeof(Block_info));

        i = BF_GetBlockCounter(header_info.fileDesc) - 1;  //i is the index of the new block
    }

    if(BF_WriteBlock(header_info.fileDesc, i) < 0){
        BF_PrintError(error_mess);
        return -1;
    }


    return 0;

}



int SHT_SecondaryGetAllEntries(SHT_info header_info_sht, HT_info header_info_ht, void* value){

    void* block;
    SHT_Block_info* curr_block;
    Block_info* ht_block;
    int h,i,j;
    int block_number;

    char* given_surname = value;

    h = hash_string(given_surname, header_info_sht.numBuckets);
    // i is index to corresponding bucket's first block.
    i = h + 1;  // Number 0 block is the header block.

    int block_counter = -1, blocks = 0;

    while(true){

        if(BF_ReadBlock(header_info_sht.fileDesc, i, &block) < 0){
            BF_PrintError(error_mess);
            return -1;
        }

        curr_block = block;

        // Searching every record in the block.
        for(j = 0 ; j < MAX_ENTRIES ; j++){

            if(!strcmp(curr_block->entries[j].surname, "")){
                continue;
            }

            if(!strcmp(curr_block->entries[j].surname, given_surname)){

                block_counter = blocks + 1;
                
                block_number = curr_block->entries[j].blockId;

                if(BF_ReadBlock(header_info_ht.fileDesc, block_number, &block)){
                    BF_PrintError(error_mess);
                    return -1;
                }

                ht_block = block;

                // Searching every record in the block.
                for(int j = 0 ; j < MAX_RECORDS ; j++){

                    if(Record_is_empty(&ht_block->records[j])){
                        continue;
                    }

                    if(!strcmp(ht_block->records[j].surname, given_surname)){
                        Record_print(&ht_block->records[j]);
                    }
                }
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



int SHT_HashStatistics(char* filename){

    SHT_info* ht = SHT_OpenSecondaryIndex(filename);

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
    SHT_Block_info* curr_block;
    for(int i = 1 ; i <= buckets ; i++){
        
        j = i;
        while(true){

            if(BF_ReadBlock(ht->fileDesc, j, &block) < 0){
                BF_PrintError(error_mess);
                return -1;
            }
            
            curr_block = block;
            records[i-1] += curr_block->num_of_entries;
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
    printf("Average number of records: %.4f\n\n", (float)sum_records/buckets);

    printf("Average number of blocks: %.4f\n\n", (float)sum_blocks/buckets);


    int overflowed_buckets = 0;
    for(int i = 0 ; i < buckets ; i++){
        if(blocks[i] > 1){
            printf("Bucket %d: %d\n", i, blocks[i] - 1);
            overflowed_buckets++;
        }
    }

    printf("\nOverflowed buckets: %d\n", overflowed_buckets);


    SHT_CloseSecondaryIndex(ht);
}




