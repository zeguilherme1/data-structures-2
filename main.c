/*
	Primeiro trabalho da disciplina Algoritmos e Estruturas de Dados II (SCC0503)
	Realizado por:
	- José Guilherme Santos Riberto (NUSP: 15647503)
	- Letícia Adriana dos Santos (NUSP: 16882428)
*/

#include <stdio.h>
#include <string.h>
#include <stdlib.h>

#include "models/record.h"
#include "models/header.h"
#include "models/index.h"

#include "io/csv_io.h"
#include "io/binary_io.h"

#include "search/criteria.h"
#include "utils/string_utils.h"

enum functions
{
	EXIT,
	CSV_TO_BINARY,
	PRINT_BINARY,
	SEQUENCIAL_SEARCH,
	RRN_SEARCH,
	CREATE_PRIMARY_INDEX,
} Functions;

int main()
{

	int option;
	scanf("%d", &option);

	switch (option)
	{
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
		criteria_search();
		break;
	case RRN_SEARCH:
		search_rrn();
		break;
	case CREATE_PRIMARY_INDEX:
		create_index_file();
		break;
	}

	return 0;
}
