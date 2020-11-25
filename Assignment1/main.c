#include "Heap.h"
#include "HashTable.h"

int main(void){

    char* file = "HT1";

    // int returned = HP_CreateFile(file, 'i', "id", sizeof(int));
    int returned = HT_CreateIndex(file, 'i', "id", sizeof(int), 5);

    HT_info* temp_header = HT_OpenIndex(file);

    if(temp_header == NULL)
        return -1;


    Record r1 = {.id = 0, .name = "Andreas", .surname = "Papandreou", .address = "Athens"};
    // Record r2 = {.id = 1, .name = "Nikos", .surname = "Kozios", .address = "Thessaloniki"};
    // Record r3 = {.id = 2, .name = "Kostas", .surname = "Paparhs", .address = "Athens"};
    // Record r4 = {.id = 3, .name = "Dionysis", .surname = "Maniatakos", .address = "Athens"};

    // HT_InsertEntry(*temp_header, r1);
    // HP_InsertEntry(*temp_header, r2);
    // HP_InsertEntry(*temp_header, r3);
    // HP_InsertEntry(*temp_header, r4);

    // HP_GetAllEntries(*temp_header, &r1.id);

    HT_info_print(temp_header);
    // HP_CloseFile(temp_header);
    // HT_CloseIndex(temp_header);

    return 0;
}
