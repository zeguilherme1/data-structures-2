/*
	Primeiro trabalho da disciplina Algoritmos e Estruturas de Dados II (SCC0503)
	Realizado por:
	- José Guilherme Santos Riberto (NUSP: 15647503)
	- Letícia Adriana dos Santos (NUSP: 16882428)
*/

#include <cstdio>
#include <cstring>
#include <cstdlib>

#include "models/record.h"
#include "models/header.h"
#include "models/index.h"


#include "io/io.h"

#include "search/search.h"
#include "utils/utils.h"
#include "edit_record/edit.h"

enum functions
{
	EXIT,
	CSV_TO_BINARY,
	PRINT_BINARY,
	SEQUENCIAL_SEARCH,
	RRN_SEARCH,
	CREATE_PRIMARY_INDEX,
	INDEX_SEARCH,
	DELETE,
	INSERT,
	UPDATE,
	GENERATE_GRAPH,
	SHORTEST_PATH,
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
	case INDEX_SEARCH:
		index_or_criteria_search();
		break;
	case DELETE:
		delete_records();
		break;
	case INSERT:
		insert_records();
		break;
	case UPDATE:
		update_records();
		break;
	case GENERATE_GRAPH:
		generate_graph();
		break;
	case SHORTEST_PATH:
		shortest_path();
		break;
	}

	return 0;
}
