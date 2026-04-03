#include <stdio.h>
#include <string.h>
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

	if(!temp_header) return -1;

	save_header(bin_file, temp_header);
	
	char buffer[200]; // Removes the first CSV line
	fgets(buffer, sizeof(buffer), csv_file);

	int record_counter = 0;

	while(fgets(buffer, sizeof(buffer), csv_file)) {
		Record *new_record = tokenize_record(buffer);

		if(new_record == NULL) return -1;

		new_record->removed = '0';
		new_record->next_record = temp_header->top;

		save_record_to_bin(bin_file, new_record);
		record_counter++;

		free_record(&new_record);
	}

	temp_header->status = '0';
	temp_header->nextRRN = record_counter;

	save_header(bin_file, temp_header);

	return 0;
}

int bin_to_text(){

	char bin_filename[100];
	scanf("%s", bin_filename);
	
	FILE *bin_file = fopen(bin_filename, READ_BINARY_MODE);

    if(!bin_file){
        printf("Falha no processamento do arquivo.\n");
        return -1;
    }

    Header* temp_header = new_header();
    
    if(!temp_header) return -1;

    int ret_header = read_header(bin_file, temp_header);
    
    if(ret_header == -1){
        free(temp_header);
        temp_header = NULL;
		fclose(bin_file);
        printf("Falha no processamento do arquivo.\n");
        return -1;
    }


    while(1){
        Record* temp_record = new_record();
		int ret_record = read_record(bin_file, temp_record);
		if(ret_record == -1){
			free_record(&temp_record);
			if(feof(bin_file)) break;
			else {
				printf("Falha no processamento do arquivo.\n");
				break;
			}
		} else{
			if(temp_record->removed != '1') print_record(temp_record);
			free_record(&temp_record);
		}
    }

	free(temp_header);
    fclose(bin_file);

    return 0;
}

void ScanQuoteString(char *str) {
    char R;

    while ((R = getchar()) != EOF && isspace(R))
        ; // ignorar espaços, \r, \n...

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
	char bin_filename[100];
	scanf("%s", bin_filename);

	int comparaton_num; //n
	scanf("%d", &comparaton_num);

	for(int i = 0; i < comparaton_num; i++){
		FILE *bin_file = fopen(bin_filename, READ_BINARY_MODE);

    	if(!bin_file){
        	printf("Falha no processamento do arquivo.\n");
        	return -1;
    	}
		
		Header* temp_header = new_header();
    
		if(!temp_header) return -1;

		int ret_header = read_header(bin_file, temp_header);
		
		if(ret_header == -1){
			free(temp_header);
			temp_header = NULL;
			fclose(bin_file);
			printf("Falha no processamento do arquivo.\n");
			return -1;
		}
		
		int num_fiels; //m
		scanf("%d", &num_fiels);

		Search_criteria criteria[num_fiels];

		for(int j = 0; j < num_fiels; j++){
			scanf("%s", criteria[j].field_name);
			if(strcmp(criteria[j].field_name, "nomeEstacao") == 0 || strcmp(criteria[j].field_name, "nomeLinha") == 0){
				ScanQuoteString(criteria[j].field_value);
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
				if(feof(bin_file)) break;
				else {
					printf("Falha no processamento do arquivo.\n");
					return -1;
				}
			} else{
				if(temp_record->removed != '1'){
					int ret_matches = matches_record_criteria(temp_record, criteria, num_fiels);
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
