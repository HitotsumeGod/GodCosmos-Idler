#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <signal.h>
#include <syslog.h>
#include <time.h>
#include <assert.h>

static void daemonize();
int mine(FILE* fptr);
FILE* wipe(FILE* fptr, char* filename);
void on_term(int sig);
char* getlocaltime(char* s);

int main(void) {
	
	//code copied from stackoverflow (https://stackoverflow.com/a/30759067) begins
	char s[64];
	time_t t = time(NULL);
    	struct tm *tm = localtime(&t);
   	size_t ret = strftime(s, sizeof(s), "%c", tm);
    	assert(ret);
	//code copied from stackoverflow ends
	daemonize();
	syslog(LOG_NOTICE, "%s%s", "Daemon is begun at ", s);
	FILE *thefile, *wipef;
	struct sigaction sigh;
	wipe(wipef, "f_data");
	memset(&sigh, 0, sizeof(sigh));
	sigh.sa_handler = &on_term;
	while (1) {
		if (sigaction(SIGTERM, &sigh, NULL) == -1)
			perror("sigaction err");
		if (sigaction(SIGINT, &sigh, NULL) == -1)
			perror("sigact err");
		sleep(2);
		mine(thefile);
	}
	return 0;

}

static void daemonize() {

	pid_t pid;
	pid = fork();
	if (pid > 0)
		exit(EXIT_SUCCESS);
	if (pid < 0) 
		exit(EXIT_FAILURE);
	if (setsid() < 0)
		exit(EXIT_FAILURE);
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
	while (fscanf(f, "%d", &i) == EOF) { 
		system("echo 0 > database");
		fscanf(f, "%d", &i);
	}
	if (fclose(f) == -1)
		perror("fclose err");
	i++;
	//reopen file for writing (wiping in the process)
	if ((f = wipe(f, "database")) == NULL)
		perror("fwipe err");
	if ((f = fopen("database", "r+")) == NULL)
		perror("fopen err");
	if (fprintf(f, "%d", i) == -1)
		perror("fpf err");
	if (fclose(f) == -1)
		perror("fclose err");
	return i;

}

FILE* wipe(FILE* targfile, char* filename) {

	if ((targfile = fopen(filename, "w")) == NULL) {
		perror("fopen err");
		return NULL;
	}
	if (fclose(targfile) == -1) {
		perror("fclose err");
		return NULL;
	}
	return targfile;

}

void on_term(int sig) {

	FILE *finalf, *tempfile;
	int i, d;
	if (sig == SIGTERM || sig == SIGINT) {
		if ((finalf = fopen("f_data", "r+")) == NULL)
			if ((finalf = fopen("f_data", "w+")) == NULL) {
				perror("finalf err");
				exit(1);
			}
		if ((tempfile = fopen("database", "r")) == NULL)
			perror("fopen err");
		if (fscanf(tempfile, "%d", &i) == -1)
			perror("fsf err");
		if (fscanf(finalf, "%d", &d) == -1)
			d = 0;
		if (fprintf(finalf, "%d", i + d) == -1)
			perror("fpf err");
		if (fclose(finalf) == -1)
			perror("fclose err");
		if (fclose(tempfile) == -1)
			perror("fclose err");
	}
	exit(1);
}

/*char* getlocaltime(char* s) {

	//code copied from stackoverflow (https://stackoverflow.com/a/30759067) begins
	time_t t = time(NULL);
    	struct tm *tm = localtime(&t);
   	size_t ret = strftime(s, sizeof(s), "%c", tm);
    	assert(ret);
	//code copied from stackoverflow ends
	return s;

}*/
