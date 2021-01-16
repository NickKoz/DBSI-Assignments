#ifndef SHT
#define SHT

#include "common_types.h"

#include "HashTable.h"


typedef struct{

    int fileDesc;
    char* attrName;
    int attrLength;
    long int numBuckets;
    char* fileName;

}SHT_info;



int SHT_CreateSecondaryIndex(char*, char*, int, int, char*);
SHT_info* SHT_OpenSecondaryIndex(char*);
int SHT_CloseSecondaryIndex(SHT_info*);
int SHT_SecondaryInsertEntry(SHT_info, SecondaryRecord);
int SHT_SecondaryGetAllEntries(SHT_info, HT_info, void*);
int SHT_HashStatistics(char*);


#endif
