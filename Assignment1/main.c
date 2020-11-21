#include <stdio.h>
#include <assert.h>
#include "BF.h"

int main(void){

    char temp[255];
    BF_Init();

    const char* file = "file1";

    if(BF_CreateFile(file) < 0)
        printf("Wtf?\n");

    // int fd = BF_OpenFile(file);
    
    
    // BF_AllocateBlock(fd);
    // BF_WriteBlock(fd,fd);
    BF_PrintError(temp);

    // printf("%d\n",BF_GetBlockCounter(fd));


    // BF_CloseFile(fd);
}
