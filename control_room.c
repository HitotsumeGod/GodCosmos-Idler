#include <stdio.h>
#include <stdlib.h>

int main(void) {
	
	FILE* fptr;
	int coins;
	if ((fptr = fopen("f_data", "r+")) == NULL) {
		perror("fopen err");
		exit(1);
	}
	if (fscanf(fptr, "%d", &coins) == EOF)
		perror("fsf err");
	printf("%s%d%s\n", "You have ", coins, " coins.");
	if (fclose(fptr) == -1) {
		perror("fclose err");
		exit(1);
	}
	return 0;

}
