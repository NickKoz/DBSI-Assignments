#include "Record.h"
#include "Heap.h"

int main(void){

    char* HP_file = "HP1";
    char* record_file = "record_examples/records1K.txt";

    int returned = HP_CreateFile(HP_file, 'i', "id", sizeof(int));
    // int returned = HT_CreateIndex(file, 'i', "id", sizeof(int), 5);

    // HT_info* temp_header = HT_OpenIndex(file);
    HP_info* temp_header = HP_OpenFile(HP_file);

    if(temp_header == NULL)
        return -1;


    Record* records = Record_extract(record_file);
    if(records == NULL){
        return -1;
    }

    for(int i = 0 ; i < num_of_records ; i++)
        HP_InsertEntry(*temp_header, records[i]);

    Record_delete(records);

    // int id = 10;
    // HP_GetAllEntries(*temp_header, &id);

    HP_CloseFile(temp_header);

    return 0;
}
