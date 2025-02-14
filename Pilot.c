#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <time.h>
#include <limits.h>

int mine(FILE* fptr);
FILE* wipe(FILE* fptr);

int main(void) {

	FILE* thefile;
	while (1) {
		sleep(2);
		mine(thefile);
	}
	return 0;

}

int mine(FILE* f) {

	int i;
	unsigned int count = 0;
	while (count < 10000)
		count++;
	//open file and read from it (create new file if does not exist)
	if ((f = fopen("database", "r")) == NULL)
		if ((f = fopen("database", "w+")) == NULL) 
			perror("fopen err");
	while (fscanf(f, "%d", &i) == EOF) 
		system("echo 0 > database");
	if (fclose(f) == -1)
		perror("fclose err");
	i++;
	//reopen file for writing (wiping in the process)
	if ((f = wipe(f)) == NULL)
		perror("fwipe err");
	if ((f = fopen("database", "r+")) == NULL)
		perror("fopen err");
	if (fprintf(f, "%d", i) == -1)
		perror("fpf err");
	if (fclose(f) == -1)
		perror("fclose err");
	return i;

}

FILE* wipe(FILE* targfile) {

	if ((targfile = fopen("database", "w")) == NULL) {
		perror("fopen err");
		return NULL;
	}
	if (fclose(targfile) == -1) {
		perror("fclose err");
		return NULL;
	}
	return targfile;

}
