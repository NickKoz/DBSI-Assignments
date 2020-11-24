#ifndef HEAP
#define HEAP

#include "common_types.h"


typedef struct{

    int fileDesc;
    char attrType;
    char* attrName;
    int attrLength;

}HP_info;




int HP_CreateFile(char*,char,char*,int);
HP_info* HP_OpenFile(char*);
int HP_CloseFile(HP_info*);
int HP_InsertEntry(HP_info, Record);


void Block_info_print(Block_info*);
void HP_info_print(HP_info*);


#endif