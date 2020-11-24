#include "Heap.h"

int main(void){

    char* file = "HP1";

    int returned = HP_CreateFile(file, 'i', "id", sizeof(int));

    HP_info* temp_header = HP_OpenFile(file);

    if(temp_header == NULL)
        return -1;

    // HP_info_print(temp_header);

    Record r1 = {.id = 0, .name = "Andreas", .surname = "Papandreou", .address = "Athens"};
    Record r2 = {.id = 1, .name = "Nikos", .surname = "Kozios", .address = "Thessaloniki"};
    Record r3 = {.id = 2, .name = "Kostas", .surname = "Paparhs", .address = "Athens"};
    Record r4 = {.id = 3, .name = "Dionysis", .surname = "Maniatakos", .address = "Athens"};

    HP_InsertEntry(*temp_header, r1);
    HP_InsertEntry(*temp_header, r2);
    HP_InsertEntry(*temp_header, r3);
    HP_InsertEntry(*temp_header, r4);

    HP_GetAllEntries(*temp_header, &r1.id);

    // HP_DeleteEntry(*temp_header, &r2.id);
    // printf("%ld\n",sizeof(Record)*5+sizeof(Block_info));


    HP_CloseFile(temp_header);

    return 0;
}
