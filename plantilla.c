#include <stdio.h>
#include <stdlib.h>
#include <string.h>

struct data {
    int *passenger_count;
    int *trip_time_in_secs;
};

int get_column_int(char* line, int num);
int get_data(struct data *data, FILE *file, int max);

int main(int argc, char *argv[])
{
    FILE *file;

    struct data data;
    int i, temp, num_elements_block, num_elements, passenger_count, trip_time_in_secs;

    if(argc != 2)
    {
        printf("%s <file>\n", argv[0]);
        exit(1);
    }

    char *filename = argv[1];

    num_elements_block = 100000;
    passenger_count = 0;
    trip_time_in_secs = 0;

    file = fopen(filename, "r");
    if (!file) {
        printf("ERROR: could not open '%s'\n", filename);
        exit(1);
    }

    data.passenger_count   = malloc(sizeof(int) * num_elements_block);
    data.trip_time_in_secs = malloc(sizeof(int) * num_elements_block); 

    num_elements = get_data(&data, file, num_elements_block);

    for(i = 0; i < num_elements; i++)
    {
        temp = data.passenger_count[i];
        passenger_count += temp;
        temp = data.trip_time_in_secs[i];
        trip_time_in_secs += temp;
    }

    free(data.passenger_count);
    free(data.trip_time_in_secs);

    fclose(file);

    float pc = 0, tt = 0;
    pc = (float)passenger_count/(float)num_elements;	
    tt = (float)trip_time_in_secs/(float)num_elements;

    printf("Aplication read %d elements\n", num_elements);
    printf("Mean of passengers: %f\n", pc);
    printf("Mean of trip time: %f\n secs", tt);

    return 0;
}

int get_column_int(char* line, int num)
{
    char new_line[256];
    char* tok;
    int retvalue;

    strncpy(new_line, line, 256);
    for (tok = strtok(new_line, ",\n"); tok; tok = strtok(NULL, ",\n"))
    {
        if (!--num) {
            retvalue = (int) strtol(tok, (char **)NULL, 10);
            return retvalue;
        }
    }

    printf("ERROR: get_column_int\n");
    exit(1); 
}

int get_data(struct data *data, FILE *file, int max)
{
    char line[256];
    int temp, pos;

    // If we are at the beginning of the file, we ignore
    // the header of the file.
    if (ftell(file) == 0)
        fgets(line, sizeof(line), file);

    pos = 0;
    while (pos < max && fgets(line, sizeof(line), file))
    {
        temp = get_column_int(line, 8);
        data->passenger_count[pos] = temp;
        temp = get_column_int(line, 9);
        data->trip_time_in_secs[pos] = temp;
        pos++;
    }

    return pos;
}
