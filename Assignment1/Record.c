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




// Returns lines' number of a file.
int file_lines(FILE* fp){   

    int line_counter = 0;
    char ch;

    while(feof(fp) == 0){
        // We read each character.
        ch = fgetc(fp);
        // If we reach \n, we have a line.
        if(ch == '\n')
            line_counter++;
    }
    // Resets file pointer to the start of the file.
    rewind(fp);
    return line_counter;
}


int num_of_records = 0;


// Returns an array of records read in given file.
Record* Record_extract(char* filename){

    FILE* fp = fopen(filename, "r");
    if(fp == NULL){
        printf("File with filename %s doesn't exist!\n", filename);
        return NULL;
    }

    num_of_records = file_lines(fp);
    
    Record* record_array = malloc(num_of_records * sizeof(Record));

    char buffer_line[BUFFER_SIZE];
    char* token;
    int id;
    for(int i = 0 ; i < num_of_records ; i++){

        // Fetches each line of the file.
        fgets(buffer_line, BUFFER_SIZE, fp);

        // Collects data.

        token = strtok(buffer_line, "{}");

        token = strtok(token, ",");
        id = atoi(token);
        record_array[i].id = id;

        token = strtok(NULL, "\"\"");
        strcpy(record_array[i].name, token);

        token = strtok(NULL, ",\"\"");
        strcpy(record_array[i].surname, token);

        token = strtok(NULL, ",\"\"");
        strcpy(record_array[i].address, token);

    }

    fclose(fp);

    return record_array;

}

void Record_delete(Record* records){
    free(records);
    records = NULL;
}
