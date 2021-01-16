#include "HashTable.h"
#include "SHashTable.h"


static int generate_rand_number(){
    return rand() % (num_of_records + 1);
}


int main(void){

    char* HT_file = "HT1";
    char* SHT_file = "SHT1";
    char* record_file = "record_examples/records5K.txt";

    int returned = HT_CreateIndex(HT_file, 'i', "id", sizeof(int), 3000);
    if(returned == -1){
        return 1;
    }

    returned = SHT_CreateSecondaryIndex(SHT_file, "surname", SURNAME_LEN, 1000, HT_file);
    if(returned == -1){
        return 1;
    }

    HT_info* ht_header = HT_OpenIndex(HT_file);
    if(ht_header == NULL)
        return -1;


    SHT_info* sht_header = SHT_OpenSecondaryIndex(SHT_file);
    if(sht_header == NULL)
        return -1;

    Record* records = Record_extract(record_file);
    if(records == NULL){
        return -1;
    }


    // Inserts all records from file.
    int blockID;
    SecondaryRecord srecord;
    for(int i = 0 ; i < num_of_records ; i++){
        blockID = HT_InsertEntry(*ht_header, records[i]);
        if(blockID < 0){
            printf("Insertion failed for record with ID: %d\n", records[i].id);
            continue;
        }
        
        SRecord_create(&srecord, records[i], blockID);

        SHT_SecondaryInsertEntry(*sht_header, srecord);

    }

    srand(time(NULL));
    int num_of_searches = 5;

    int to_be_searched[num_of_searches];

    for(int i = 0 ; i < num_of_searches ; i++){
        to_be_searched[i] = generate_rand_number();
    }

    int status;
    for(int i = 0 ; i < num_of_searches ; i++){
        status = HT_GetAllEntries(*ht_header, &to_be_searched[i]);
        if(status < 0){
            printf("Cannot find record with ID: %d\n", to_be_searched[i]);
        }
    }

    printf("------------------------\n");

    char* temp_surname;
    for(int i = 0 ; i < num_of_searches ; i++){

        temp_surname = records[to_be_searched[i]].surname;
        status = SHT_SecondaryGetAllEntries(*sht_header, *ht_header, temp_surname);
        if(status < 0){
            printf("Cannot find record with ID: %d\n", to_be_searched[i]);
        }
    }


    SHT_HashStatistics(SHT_file);


    HT_CloseIndex(ht_header);
    SHT_CloseSecondaryIndex(sht_header);

    Record_delete(records);
    
    return 0;
}
