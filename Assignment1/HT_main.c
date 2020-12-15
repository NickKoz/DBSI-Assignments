#include "Record.h"
#include "HashTable.h"
#include "Heap.h"

int main(void){

    char* HT_file = "HT1";
    char* record_file = "record_examples/records1K.txt";

    int returned = HT_CreateIndex(HT_file, 'i', "id", sizeof(int), 10);

    HT_info* temp_header = HT_OpenIndex(HT_file);
    if(temp_header == NULL)
        return -1;


    Record* records = Record_extract(record_file);
    if(records == NULL){
        return -1;
    }


    for(int i = 0 ; i < 959 ; i++){
        HT_InsertEntry(*temp_header, records[i]);
    }


    // HT_DeleteEntry(*temp_header, &records[950].id);

    printf("%d\n",HT_GetAllEntries(*temp_header, &records[450].id));

    Record_delete(records);

    HT_CloseIndex(temp_header);

    HashStatistics(HT_file);

    
    return 0;
}
