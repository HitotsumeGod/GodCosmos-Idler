#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <signal.h>
#include <syslog.h>
#include <time.h>
#include <assert.h>
#include <sys/stat.h>

#define FILEPATH /

static void daemonize();
int mine(FILE* fptr);
FILE* wipe(FILE* fptr, char* filename);
void on_term(int sig);
char* getlocaltime(char* s);

char* coms;

int main(void) {
	
	//code copied from stackoverflow (https://stackoverflow.com/a/30759067) begins
	char s[64];
	time_t t = time(NULL);
    	struct tm *tm = localtime(&t);
   	size_t ret = strftime(s, sizeof(s), "%c", tm);
    	assert(ret);
	//code copied from stackoverflow ends
	char *env_path;
	if ((coms = malloc(30)) == NULL) {
		perror("malloc err");
		exit(EXIT_FAILURE);
	}
       	if ((env_path = getenv("HOME")) == NULL) {
		perror("error getting environment variable");
		exit(EXIT_FAILURE);
	}
	if (snprintf(coms, 30, "%s%s", env_path, "/.godcosmos") == -1) {
		perror("snprintf err");
		exit(EXIT_FAILURE);
	}
	if (mkdir(coms, 0777) == -1) 
		perror("mkdir err");
	daemonize();
	syslog(LOG_NOTICE, "%s%s", "Daemon is begun at ", s);
	FILE *thefile, *wipef;
	struct sigaction sigh;
	free(coms);
	wipe(wipef, "f_data");
	memset(&sigh, 0, sizeof(sigh));
	sigh.sa_handler = &on_term;
	while (1) {
		if (sigaction(SIGTERM, &sigh, NULL) == -1)
			syslog(LOG_NOTICE, "sigaction err");
		if (sigaction(SIGINT, &sigh, NULL) == -1)
			syslog(LOG_NOTICE, "sigact err");
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
	if (chdir(coms) == -1)
		syslog(LOG_NOTICE, "FATAL CHDIR ERR. STOP DAEMON IMMEDIATELY.");
	closelog();
}

int mine(FILE* f) {

	int i;
	unsigned int count = 0;
	while (count < 10000)
		count++;
	//open file and read from it (create new file if does not exist)
	if ((f = fopen("database", "r")) == NULL)
		if ((f = fopen("database", "w+")) == NULL) 
			syslog(LOG_NOTICE, "FOPEN err");
	while (fscanf(f, "%d", &i) == EOF) {
	       	if (fclose(f) == -1)
			syslog(LOG_NOTICE, "FCLOSE error.");
		if ((f = fopen("database", "r")) == NULL)
			syslog(LOG_NOTICE, "FOPEN error.");	
		system("echo 0 > database");
		if (fscanf(f, "%d", &i) == EOF) {
			syslog(LOG_NOTICE, "FSF error.");
			exit(EXIT_FAILURE);
		}
	}
	if (fclose(f) == -1)
		perror("fclose err");
	i++;
	//reopen file for writing (wiping in the process)
	if ((f = wipe(f, "database")) == NULL)
		syslog(LOG_NOTICE, "FWIPE err");
	if ((f = fopen("database", "r+")) == NULL)
		syslog(LOG_NOTICE, "FOPEN err");
	if (fprintf(f, "%d", i) == -1)
		syslog(LOG_NOTICE, "FPF err");
	if (fclose(f) == -1)
		syslog(LOG_NOTICE, "FCLOSE err");
	return i;

}

FILE* wipe(FILE* targfile, char* filename) {

	if ((targfile = fopen(filename, "w")) == NULL) {
		syslog(LOG_NOTICE, "fopen err");
		return NULL;
	}
	if (fclose(targfile) == -1) {
		syslog(LOG_NOTICE, "fclose err");
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
				syslog(LOG_NOTICE, "finalf err");
				exit(1);
			}
		if ((tempfile = fopen("database", "r")) == NULL)
			syslog(LOG_NOTICE, "f1open err");
		if (fscanf(tempfile, "%d", &i) == -1)
			syslog(LOG_NOTICE, "fsf err");
		if (fscanf(finalf, "%d", &d) == -1)
			d = 0;
		if (fprintf(finalf, "%d", i + d) == -1)
			syslog(LOG_NOTICE, "fpf err");
		if (fclose(finalf) == -1)
			syslog(LOG_NOTICE, "fclose err");
		if (fclose(tempfile) == -1)
			syslog(LOG_NOTICE, "fclose err");
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
