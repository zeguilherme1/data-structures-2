#include <stdio.h>
#include <string.h>
#include "record.h"
#include "header.h"
#include "utils.h"

enum functions {
	EXIT,
	CSV_TO_BINARY,
	PRINT_BINARY,
	SEQUENCIAL_SEARCH,
	RRN_SEARCH
} Functions;

int main() {

	int option;
	scanf("%d", &option);

	switch(option) {
		case EXIT:
			return 0;
			break;
		case CSV_TO_BINARY:
			csv_to_bin();
			break;
		case PRINT_BINARY:
			bin_to_text();		
			break;
		case SEQUENCIAL_SEARCH:
			break;
		case RRN_SEARCH:
			search_rrn();
			break;
	}
	

	

	return 0;
}
