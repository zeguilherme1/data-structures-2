#include <stdio.h>
#include <string.h>
#include <ctype.h>
#include "record.h"
#include "header.h"
#include <stdbool.h>
#include "utils.h"

typedef struct
{
    int station_code;
    int next_station_code;
} Pair;

void BinarioNaTela(char *arquivo)
{
    FILE *fs;
    if (arquivo == NULL || !(fs = fopen(arquivo, "rb")))
    {
        fprintf(stderr,
                "ERRO AO ESCREVER O BINARIO NA TELA (função binarioNaTela): "
                "não foi possível abrir o arquivo que me passou para leitura. "
                "Ele existe e você tá passando o nome certo? Você lembrou de "
                "fechar ele com fclose depois de usar?\n");
        return;
    }

    fseek(fs, 0, SEEK_END);
    size_t fl = ftell(fs);

    fseek(fs, 0, SEEK_SET);
    unsigned char *mb = (unsigned char *)malloc(fl);
    fread(mb, 1, fl, fs);

    unsigned long cs = 0;
    for (unsigned long i = 0; i < fl; i++)
    {
        cs += (unsigned long)mb[i];
    }

    printf("%lf\n", (cs / (double)100));

    free(mb);
    fclose(fs);
}

int integer_or_null(char *str)
{

    /*
        This function receives a string and compare if is a integer or a NULL

        Args:
            (char*) str: string that function will compare

        Return:
            atoi(str) if is a integer
            -1 if string is empty or NULL


    */
    if (str == NULL)
        return -1;

    // removes \n e \r
    str[strcspn(str, "\r\n")] = '\0';

    // empty string
    if (str[0] == '\0')
        return -1;

    return atoi(str);
}

int csv_to_bin()
{

    /*
        This functions reads a csv and a binary file, then process
        all the csv into the binary file.

        Args:
            No args

        Return:
            0 if success or -1 if failed
    */

    char csv_filename[100], bin_filename[100];

    scanf("%s %s", csv_filename, bin_filename);
    FILE *csv_file = fopen(csv_filename, "r");
    FILE *bin_file = fopen(bin_filename, WRITE_BINARY_MODE);

    if (!csv_file || !bin_file)
    {
        printf("Falha no processamento do arquivo.");
        return 0;
    }

    Header *temp_header = new_header();
    if (temp_header == NULL)
        return MALLOC_ERROR;

    temp_header->status = FALSE;
    save_header(bin_file, temp_header);

    char buffer[200];
    fgets(buffer, sizeof(buffer), csv_file);

    int record_counter = 0;

    Pair pairs[10000];
    int pair_counter = 0;

    char stations[10000][100];
    int station_counter = 0;

    while (fgets(buffer, sizeof(buffer), csv_file))
    {

        Record *new_record = tokenize_record(buffer);
        if (new_record == NULL)
            continue;

        new_record->removed = FALSE;
        new_record->next_record = temp_header->top;

        int a = new_record->station_code;
        int b = new_record->next_station_code;

        if (b != -1 && a != b)
        {

            int first = (a < b) ? a : b;
            int scnd = (a < b) ? b : a;

            bool exists = false;

            for (int i = 0; i < pair_counter; i++)
            {
                if (pairs[i].station_code == first &&
                    pairs[i].next_station_code == scnd)
                {
                    exists = true;
                    break;
                }
            }

            if (!exists)
            {
                pairs[pair_counter].station_code = first;
                pairs[pair_counter].next_station_code = scnd;
                pair_counter++;
            }
        }

        int station_exists = 0;
        for (int i = 0; i < station_counter; i++)
        {
            if (strcmp(stations[i], new_record->station_name) == 0)
            {
                station_exists = 1;
                break;
            }
        }

        if (!station_exists)
        {
            strcpy(stations[station_counter], new_record->station_name);
            station_counter++;
        }

        save_record_to_bin(bin_file, new_record);
        record_counter++;

        free_record(&new_record);
    }

    temp_header->status = TRUE;
    temp_header->nextRRN = record_counter;
    temp_header->station_num = station_counter;
    temp_header->station_pairs_num = pair_counter - 1;

    save_header(bin_file, temp_header);

    BinarioNaTela(bin_filename);

    fclose(csv_file);
    fclose(bin_file);
    free(temp_header);

    return SUCCESS;
}
int bin_to_text()
{
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

    if (!bin_file)
    {
        printf("Falha no processamento do arquivo.\n");
        return -1;
    }

    Header *temp_header = new_header();

    if (temp_header == NULL)
        return MALLOC_ERROR;

    int ret_header = read_header(bin_file, temp_header); // reads the header to skip its bytes

    if (ret_header == -1)
    {
        free(temp_header);
        temp_header = NULL;
        fclose(bin_file);
        printf("Falha no processamento do arquivo.\n");
        return -1;
    }

    while (1)
    {
        Record *temp_record = new_record();
        int ret_record = read_record(bin_file, temp_record); // sequentially reads the record
        if (ret_record == -1)
        {
            free_record(&temp_record);
            if (feof(bin_file))
                break; // checks if the end of file was reached
            else
            {
                printf("Falha no processamento do arquivo.\n");
                free(temp_header);
                fclose(bin_file);
                return -1;
            }
        }
        else
        {
            if (temp_record->removed == '0')
                print_record(temp_record); // if it was not removed, prints it
            free_record(&temp_record);
        }
    }

    free(temp_header);
    fclose(bin_file);

    return SUCCESS;
}

void scan_quote_string(char *str)
{
    char R;

    while ((R = getchar()) != EOF && isspace(R))
        ; // ignorar espaços, \r, \n...

    if (R == 'N' || R == 'n')
    { // campo NULO
        getchar();
        getchar();
        getchar();       // ignorar o "ULO" de NULO.
        strcpy(str, ""); // copia string vazia
    }
    else if (R == '\"')
    {
        if (scanf("%[^\"]", str) != 1)
        { // ler até o fechamento das aspas
            strcpy(str, "");
        }
        getchar(); // ignorar aspas fechando
    }
    else if (R != EOF)
    { // vc tá tentando ler uma string que não tá entre
      // aspas! Fazer leitura normal %s então, pois deve
      // ser algum inteiro ou algo assim...
        str[0] = R;
        scanf("%s", &str[1]);
    }
    else
    { // EOF
        strcpy(str, "");
    }
}

int criteria_search()
{
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

    int comparaton_num; // n number of searches
    scanf("%d", &comparaton_num);

    for (int i = 0; i < comparaton_num; i++)
    {
        FILE *bin_file = fopen(bin_filename, READ_BINARY_MODE); // the file is opened and closed at every iteration

        if (!bin_file)
        {
            printf("Falha no processamento do arquivo.\n");
            return -1;
        }

        Header *temp_header = new_header();

        if (temp_header == NULL)
            return MALLOC_ERROR;

        int ret_header = read_header(bin_file, temp_header); // reads the header to skip its bytes

        if (ret_header == -1)
        {
            free(temp_header);
            temp_header = NULL;
            fclose(bin_file);
            printf("Falha no processamento do arquivo.\n");
            return -1;
        }

        int num_fields; // m number of criteria fields
        scanf("%d", &num_fields);

        Search_criteria criteria[num_fields]; // array of structs to store the field name and value

        for (int j = 0; j < num_fields; j++)
        {
            scanf("%s", criteria[j].field_name);
            if (strcmp(criteria[j].field_name, "station_name") == 0 || strcmp(criteria[j].field_name, "nomeLinha") == 0)
            {
                scan_quote_string(criteria[j].field_value); // if it is a string, removes the surrounding quotes from input
            }
            else
            {
                scanf("%s", criteria[j].field_value);
            }
        }

        int found = 0;

        while (1)
        {
            Record *temp_record = new_record();
            int ret_record = read_record(bin_file, temp_record);
            if (ret_record == -1)
            {
                free_record(&temp_record);
                if (feof(bin_file))
                    break; // checks if the end of file was reached
                else
                {
                    printf("Falha no processamento do arquivo.\n");
                    free(temp_header);
                    fclose(bin_file);
                    return -1;
                }
            }
            else
            {
                if (temp_record->removed == '0')
                { // if the record isn't removed, checks if it matches the criteria and prints it
                    int ret_matches = matches_record_criteria(temp_record, criteria, num_fields);
                    if (ret_matches == 0)
                    {
                        print_record(temp_record);
                        found = 1;
                    }
                }
                free_record(&temp_record);
            }
        }
        if (found == 0)
            printf("Registro inexistente.\n");
        if (i < comparaton_num - 1)
            printf("\n");
        free(temp_header);
        fclose(bin_file);
    }
    return 0;
}

void search_rrn()
{
    /*
   This function reads a binary file and a RRN value, then reads the
   record based on the RRN position using read_rrn_record function.

   Args:
       No args

   Return:
       No return
   */

    char bin_filename[100];
    int rrn;

    scanf("%s %d", bin_filename, &rrn);

    FILE *bin_file = fopen(bin_filename, READ_BINARY_MODE);

    Header *bin_header = read_binary_header(bin_file);

    if (bin_header == NULL)
        return MALLOC_ERROR;

    Record *result_record = read_rrn_record(bin_file, rrn);

    print_record(result_record);
}
