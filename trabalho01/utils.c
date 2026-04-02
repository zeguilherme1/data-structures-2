#include <stdio.h>
#include <string.h>
#include "register.h"
#include "header.h"
#include "utils.h"


int csv_to_bin() {
	char csv_filename[100], bin_filename[100];

	scanf("%s %s", csv_filename, bin_filename);

	FILE *csv_file = fopen(csv_filename, "r");
	FILE *bin_file = fopen(bin_filename, READ_BINARY_MODE);

	Header* temp_header = create_header();

	if(!temp_header) return -1;

	save_header(bin_file, temp_header);
	
	char buffer[200]; // Removes the first CSV line
	fgets(buffer, sizeof(buffer), csv_file);

	int register_counter = 0;

	while(fgets(buffer, sizeof(buffer), csv_file)) {
		Record *new_register = tokenize_register(buffer);

		if(!new_register) return -1;

		new_register->removed = '0';
		new_register->next_register = temp_header->top;

		save_register_to_bin(bin_file, new_register);
		register_counter++;

		free_register(&new_register);
	}

	temp_header->status = '0';
	temp_header->nextRRN = register_counter;

	save_header(bin_file, temp_header);

	return 0;
}

int bin_to_text(){
    char bin_filename[100];

	scanf("%s", bin_filename);

	FILE *bin_file = fopen(bin_filename, READ_BINARY_MODE);

    if(!bin_file){
        printf("Falha no processamento do arquivo.");
        return -1;
    }

    Header* temp_header = new_header();
    
    if(!temp_header) return -1;

    int ret_header = read_header(bin_file, temp_header);
    
    if(ret_header == -1){
        free(temp_header);
        temp_header = NULL;
		fclose(bin_file);
        printf("Falha no processamento do arquivo");
        return -1;
    }


    while(1){
        Record* temp_record = new_record();
		int ret_record = read_register(bin_file, temp_record);
		if(ret_record == -1){
			free_register(temp_record);
			if(feof(bin_file)) break;
			else {
				printf("Falha no processamento do arquivo");
				break;
			}
		} else{
			if(temp_record->removed != '1') print_register(temp_record);
			free_register(temp_record);
		}
    }

    fclose(bin_file);
    free(temp_header);

    return 0;
}
