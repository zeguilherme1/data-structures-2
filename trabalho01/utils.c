#include <stdio.h>
#include <string.h>
#include <ctype.h>
#include "record.h"
#include "header.h"
#include "utils.h"



int integer_or_null(char* str) {
    return str == NULL || strcspn(str, "\n\r") == 0 ? -1 : atoi(str);
}

int csv_to_bin() {

	char csv_filename[100], bin_filename[100];

	scanf("%s %s", csv_filename, bin_filename);
	FILE *csv_file = fopen(csv_filename, "r");
	FILE *bin_file = fopen(bin_filename, WRITE_BINARY_MODE);

	Header* temp_header = new_header();

	if(temp_header == NULL) return MALLOC_ERROR;

	save_header(bin_file, temp_header);
	
	char buffer[200]; // Removes the first CSV line
	fgets(buffer, sizeof(buffer), csv_file);

	int record_counter = 0;

	while(fgets(buffer, sizeof(buffer), csv_file)) {
		Record *new_record = tokenize_record(buffer);

		if(new_record == NULL) return MALLOC_ERROR;

		new_record->removed = FALSE;
		new_record->next_record = temp_header->top;

		save_record_to_bin(bin_file, new_record);
		record_counter++;

		free_record(&new_record);
	}

	temp_header->status = FALSE;
	temp_header->nextRRN = record_counter;

	save_header(bin_file, temp_header);

	return SUCCESS;
}

int bin_to_text(){
	/*
        This function reads the binary file and print it, 
		formatting if there are null fields.

        Args:
            No args.

        Return:
            0 for success and -1 for failure
    
    */

	char bin_filename[100];
	scanf("%s", bin_filename);
	
	FILE *bin_file = fopen(bin_filename, READ_BINARY_MODE);

    if(!bin_file){
        printf("Falha no processamento do arquivo.\n");
        return -1;
    }

    Header* temp_header = new_header();
    
    if(temp_header == NULL) return MALLOC_ERROR;

    int ret_header = read_header(bin_file, temp_header); // reads the header to skip its bytes
    
    if(ret_header == -1){
        free(temp_header);
        temp_header = NULL;
		fclose(bin_file);
        printf("Falha no processamento do arquivo.\n");
        return -1;
    }


    while(1){
        Record* temp_record = new_record();
		int ret_record = read_record(bin_file, temp_record); // sequentially reads the record 
		if(ret_record == -1){
			free_record(&temp_record);
			if(feof(bin_file)) break; // checks if the end of file was reached
			else {
				printf("Falha no processamento do arquivo.\n");
				free(temp_header);
				fclose(bin_file);
				return -1;
			}
		} else{
			if(temp_record->removed == '0') print_record(temp_record); // if it was not removed, prints it
			free_record(&temp_record);
		}
    }

	free(temp_header);
    fclose(bin_file);

    return SUCCESS;
}

void scan_quote_string(char *str) {
    char R;

    while ((R = getchar()) != EOF && isspace(R)); // ignorar espaços, \r, \n...

    if (R == 'N' || R == 'n') { // campo NULO
        getchar();
        getchar();
        getchar();       // ignorar o "ULO" de NULO.
        strcpy(str, ""); // copia string vazia
    } else if (R == '\"') {
        if (scanf("%[^\"]", str) != 1) { // ler até o fechamento das aspas
            strcpy(str, "");
        }
        getchar();         // ignorar aspas fechando
    } else if (R != EOF) { // vc tá tentando ler uma string que não tá entre
                           // aspas! Fazer leitura normal %s então, pois deve
                           // ser algum inteiro ou algo assim...
        str[0] = R;
        scanf("%s", &str[1]);
    } else { // EOF
        strcpy(str, "");
    }
}
	
int criteria_search(){
	/*
        This function reads the file and the n numbers of searches each with m criteria fields. 
		So it reads every record and checks if the record field is equal to the given criteria, 
		if it is, then the record is printed. 
		If no correspondence is found for a given search, a "not found" message is printed.

        Args:
            No args.

        Return:
            0 for success and -1 for failure
    
    */

	char bin_filename[100];
	scanf("%s", bin_filename);

	int comparaton_num; //n number of searches
	scanf("%d", &comparaton_num);

	for(int i = 0; i < comparaton_num; i++){
		FILE *bin_file = fopen(bin_filename, READ_BINARY_MODE); //the file is opened and closed at every iteration 

    	if(!bin_file){
        	printf("Falha no processamento do arquivo.\n");
        	return -1;
    	}
		
		Header* temp_header = new_header();
    
		if(temp_header == NULL) return MALLOC_ERROR;

		int ret_header = read_header(bin_file, temp_header); // reads the header to skip its bytes
		
		if(ret_header == -1){
			free(temp_header);
			temp_header = NULL;
			fclose(bin_file);
			printf("Falha no processamento do arquivo.\n");
			return -1;
		}
		
		int num_fields; // m number of criteria fields
		scanf("%d", &num_fields);

		Search_criteria criteria[num_fields]; //array of structs to store the field name and value

		for(int j = 0; j < num_fields; j++){
			scanf("%s", criteria[j].field_name);
			if(strcmp(criteria[j].field_name, "nomeEstacao") == 0 || strcmp(criteria[j].field_name, "nomeLinha") == 0){
				scan_quote_string(criteria[j].field_value); // if it is a string, removes the surrounding quotes from input
			} else {
				scanf("%s", criteria[j].field_value);
			}		
    	}

		int found = 0;

		while(1){
			Record* temp_record = new_record();
			int ret_record = read_record(bin_file, temp_record);
			if(ret_record == -1){ 
				free_record(&temp_record); 
				if(feof(bin_file)) break; // checks if the end of file was reached
				else {
					printf("Falha no processamento do arquivo.\n");
					free(temp_header);
					fclose(bin_file);
					return -1;
				}
			} else{
				if(temp_record->removed == '0'){ //if the record isn't removed, checks if it matches the criteria and prints it
					int ret_matches = matches_record_criteria(temp_record, criteria, num_fields);
					if(ret_matches == 0){
						print_record(temp_record);
						found = 1;
					}
				} 
				free_record(&temp_record);
			}
		}
		if(found == 0) printf("Registro inexistente.\n"); 
		free(temp_header);
		fclose(bin_file);
	}
	return 0;
}

int read_rrn_record(char* bin_filename, int rrn) {
	// calcula o byteoffset
	// move o ponteiro usando fseek
	// le os campos fixos do registro
	// le os campos variaveis


	FILE* bin_file = fopen(bin_filename, READ_BINARY_MODE);

	int byte_offset = (HEADER_SIZE + (RECORD_SIZE * rrn));
	fseek(bin_file, byte_offset, SEEK_SET);

	Record *find_record = (Record*)malloc(sizeof(Record));
	if(find_record == NULL) return MALLOC_ERROR;

	fread(&find_record->next_record, sizeof(int), 1, bin_file);
	fread(&find_record->station_code, sizeof(int), 1, bin_file);
	fread(&find_record->line_code, sizeof(int), 1, bin_file);
	fread(&find_record->next_station_code, sizeof(int), 1, bin_file);
	fread(&find_record->next_station_distance, sizeof(int), 1, bin_file);
	fread(&find_record->line_integration_code, sizeof(int), 1, bin_file);
	fread(&find_record->station_integration_code, sizeof(int), 1, bin_file);


	fread(&find_record->station_name_size, sizeof(int), 1, bin_file);
	if(find_record->station_name_size > 0){
		find_record->station_name = (char*)calloc(find_record->station_name_size, sizeof(char));
	}
	fread(&find_record->station_name, sizeof(char), find_record->station_name_size, bin_file);
	
	fread(&find_record->line_name_size, sizeof(int), 1, bin_file);
	if(find_record->line_name_size > 0){
		find_record->line_name = (char*)calloc(find_record->line_name_size, sizeof(char));
	}
	fread(&find_record->line_name, sizeof(char), find_record->line_name_size, bin_file);


	return find_record;
}
