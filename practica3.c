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

void productor(int fd, char *filename)
{
	// char *filename;
	FILE *file;
	int i, temp, num_elements_block, num_elements, *data_array;
	struct data data;

	// filename = &argv[1];
	num_elements_block = 30;

	file = fopen(filename, "r");
	if (!file) {
		printf("ERROR: could not open '%s'\n", filename);
		exit(1);
	}

	data.passenger_count = malloc(sizeof(int) * num_elements_block);
	data.trip_time_in_secs = malloc(sizeof(int) * num_elements_block);
	data_array = malloc(sizeof(int) * 2);

	num_elements = get_data(&data, file, num_elements_block);

	for (i = 0; i < num_elements; i++) {
		// temp = data.passenger_count[i];
       		// passenger_count += temp;
        	// temp = data.trip_time_in_secs[i];
        	// trip_time_in_secs += temp;
		
		data_array[0] = data.passenger_count[i];
		data_array[1] = data.trip_time_in_secs[i];
		
		write(fd, data_array, sizeof(int *));		
	}

	free(data.passenger_count);
	free(data.trip_time_in_secs);
	free(data_array);
}

void consumidor(int fd)
{
	int i, passenger_count, trip_time_in_secs, num_elements, *data_array;

	passenger_count = 0;
	trip_time_in_secs = 0;
	i = 0;
	num_elements = 30;
	
	data_array = malloc(sizeof(int) * 2);

	while (i < num_elements) {
		read(fd, data_array, sizeof(int *));
		
		passenger_count += data_array[0];
		trip_time_in_secs += data_array[1];

		printf("%d\n", data_array[0]);
		// printf("%d\n", data_array[1]);

		i++;
	}

	printf("Child has finished\n");

	float pc = 0, tt = 0;
    	pc = (float)passenger_count / (float)num_elements;
    	tt = (float)trip_time_in_secs / (float)num_elements;

    	printf("Aplication read %d elements\n", num_elements);
    	printf("Mean of passengers: %f\n", pc);
    	printf("Mean of trip time: %f\n secs", tt);

	free(data_array);
}

int main(int argc, char *argv[])
{
	int fd[2];

	pipe(fd);

	if (fork() == 0) {		// Child --> consumidor
		consumidor(fd[0]);
	} else {			// Parent --> productor
		char *filename = argv[1];
		productor(fd[1], filename);
	}
	return 0;
}
