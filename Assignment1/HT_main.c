#include "HashTable.h"


static int generate_rand_number(){
    return rand() % (num_of_records + 1);
}


int main(void){

    char* HT_file = "HT1";
    char* record_file = "record_examples/records15K.txt";

    int returned = HT_CreateIndex(HT_file, 'i', "id", sizeof(int), 3000);

    HT_info* temp_header = HT_OpenIndex(HT_file);
    if(temp_header == NULL)
        return -1;


    Record* records = Record_extract(record_file);
    if(records == NULL){
        return -1;
    }


    // Inserts all records from file.
    int status;
    for(int i = 0 ; i < num_of_records ; i++){
        status = HT_InsertEntry(*temp_header, records[i]);
        if(status < 0){
            printf("Insertion failed for record with ID: %d\n", records[i].id);
        }
    }

    // Prints all records.
    status = HT_GetAllEntries(*temp_header, NULL);
    printf("Blocks iterated: %d\n", status);


    srand(time(NULL));
    int num_for_deletion = 5;

    int to_be_deleted[num_for_deletion];

    for(int i = 0 ; i < num_for_deletion ; i++){
        to_be_deleted[i] = generate_rand_number();
    }
    printf("\n##############################\n");
    printf("#       Before deletion     #\n");
    printf("#############################\n\n");
    for(int i = 0 ; i < num_for_deletion ; i++){
        status = HT_GetAllEntries(*temp_header, &to_be_deleted[i]);
        if(status < 0){
            printf("Getting failed for record with ID: %d\n", to_be_deleted[i]);
        }
    }

    // Deletes 5 random records. 
    for(int i = 0 ; i < num_for_deletion ; i++){
        status = HT_DeleteEntry(*temp_header, &to_be_deleted[i]);
        if(status < 0){
            printf("Deletion failed for record with ID: %d\n", to_be_deleted[i]);
        }
        else{
            printf("Successfull deletion for record with ID: %d\n", to_be_deleted[i]);
        }
    }

    printf("\n#############################\n");
    printf("#       After deletion      #\n");
    printf("#############################\n\n");
    for(int i = 0 ; i < num_for_deletion ; i++){
        status = HT_GetAllEntries(*temp_header, &to_be_deleted[i]);
        if(status < 0){
            printf("Cannot find record with ID: %d\n", to_be_deleted[i]);
        }
    }

    // Calls hash statistics.
    HashStatistics(HT_file);

    HT_CloseIndex(temp_header);

    Record_delete(records);
    
    return 0;
}
