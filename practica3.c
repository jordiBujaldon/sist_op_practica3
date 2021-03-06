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

void sigus_productor(int signal)
{
	// printf("Pare ha rebut la senyal\n");
}


void productor(int fd, char *filename, int pid)
{
	// char *filename;
	FILE *file;
	int i, temp, num_elements_block, num_elements, *data_array;
	struct data data;

	// filename = &argv[1];
	num_elements_block = 100000;

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
		
		if (i != 0 && i % 30 == 0) {
			data_array[0] = -1;
			data_array[1] = -1;

			write(fd, data_array, sizeof(int *));
			kill(pid, SIGUSR2);
		}

		signal(SIGUSR1, sigus_productor);
	}

	free(data.passenger_count);
	free(data.trip_time_in_secs);
	free(data_array);
	exit(0);
}

void consumidor(int fd, int *data, int pid)
{
	int i, passenger_count, trip_time_in_secs, num_elements, *data_array;

	passenger_count = 0;
	trip_time_in_secs = 0;
	num_elements = 0;
	
	data_array = malloc(sizeof(int) * 2);

	while (1) {
		read(fd, data_array, sizeof(int *));
		
		if (data_array[0] == 0 && data_array[1] == 0) {
			break;
		} else if (data_array[0] == -1 && data_array[1] == -1) {
			data[0] += passenger_count;
			data[1] += trip_time_in_secs;

			kill(pid, SIGUSR1);
		} else {
			passenger_count += data_array[0];
        	        trip_time_in_secs += data_array[1];
			
			// printf("%d\n", data_array[0]);
               		// printf("%d\n", data_array[1]);

                	num_elements++;
		}
	}

	printf("Child has finished\n");

	float pc = 0, tt = 0;
    	pc = (float) data[0] / (float) num_elements;
    	tt = (float) data[1] / (float) num_elements;

    	printf("Aplication read %d elements\n", num_elements);
    	printf("Mean of passengers: %f\n", pc);
    	printf("Mean of trip time: %f secs\n", tt);

	free(data_array);
	exit(0);
}

void sigus_consumidor(int signal)
{
	// printf("Fill ha rebut la senyal\n");
}

int main(int argc, char *argv[])
{
	int fd[2], ret, parent_pid, child_pid;

	pipe(fd);
	ret = fork();

	if (ret == 0) {		// Child --> consumidor
		int *data = malloc(sizeof(int) * 2);
		data[0] = 0;			// Guardem els passenger_count
		data[1] = 0;			// Guardem els trip_time_in_secs
		parent_pid = getppid();

		signal(SIGUSR2, sigus_consumidor);
		consumidor(fd[0], data, parent_pid);
		free(data);
	} else {			// Parent --> productor
		char *filename = argv[1];
		child_pid = ret;
		productor(fd[1], filename, child_pid);
	}
	return 0;
}
