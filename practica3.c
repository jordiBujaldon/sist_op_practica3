#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/wait.h>

// Estructura de dada que enviarem (columna de passatgers i temps)
struct data {
    int *passenger_count;
    int *trip_time_in_secs;
};

// Funcions
int get_column_int(char* line, int num);
int get_data(struct data *data, FILE *file, int max);

// Funció per obtenir la columna del fitxer
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

void productor(char *argv, int fd)
{
	char *filename;
	FILE *file;
	int i, temp, num_elements_block, passenger_count, trip_time_in_secs, num_elements;
	struct data data;

	filename = &argv[1];
	num_elements_block = 30;
	passenger_count = 0;
	trip_time_in_secs = 0;

	file = fopen(filename, "r");
	if (!file) {
		printf("ERROR: could not open '%s'\n", filename);
		exit(1);
	}

	data.passenger_count = malloc(sizeof(int) * num_elements_block);
	data.trip_time_in_secs = malloc(sizeof(int) * num_elements_block);
}

void consumidor(int fd)
{
}

int main(int argc, char *argv[])
{
	int fd[2];

	pipe(fd);

	if (fork() == 0) {		// Child --> consumidor
		consumidor(fd[0]);
	} else {			// Parent --> productor
		productor(fd[1]);
	}
	return 0;
}
