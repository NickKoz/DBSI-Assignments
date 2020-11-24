#include "Heap.h"

int main(void){

    char temp[255];

    char* file = "HP1";

    int returned = HP_CreateFile(file, 'i', "ID", sizeof(int));

    HP_info* temp_header = HP_OpenFile(file);

    if(temp_header == NULL)
        return -1;

    HP_info_print(temp_header);

    Record r1 = {.id = 1, .name = "Andreas", .surname = "Stamaths", .address = "Athens"};

    HP_InsertEntry(*temp_header, r1);


    // printf("%ld\n",sizeof(Record)*5+sizeof(Block_info));


    HP_CloseFile(temp_header);

    return 0;
}
