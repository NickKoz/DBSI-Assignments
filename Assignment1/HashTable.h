#ifndef HASHTABLE
#define HASHTABLE

#include "common_types.h"


typedef struct{

    int fileDesc;
    char attrType;
    char* attrName;
    int attrLength;
    long int numBuckets;

}HT_info;


int HT_CreateIndex(char*, char, char*, int, int);
HT_info* HT_OpenIndex(char*);
int HT_CloseIndex(HT_info*);
int HT_InsertEntry(HT_info, Record);


void HT_info_print(HT_info*);


#endif