#include "Record.h"


void Record_print(Record* r){
    printf("ID: %d\n", r->id);
    printf("Name: %s\n", r->name);
    printf("Surname: %s\n", r->surname);
    printf("Address: %s\n\n", r->address);
}


bool Record_is_empty(Record* r){
    return r->id == 0 && !strcmp(r->name,"") && !strcmp(r->surname,"") && !strcmp(r->address,"");
}

