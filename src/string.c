#include <stdio.h>
#include <string.h>
#include <stdlib.h>

#include "str.h"

int
create_vector(int *argc, char *argv[MAX_ARG], char *string)
{
	char *longarg = NULL;
	int i = 0;

	*argc = 0;
	longarg = string;

	if(strsep(&longarg,":")) /* Tear off short args */
		if(longarg && !strcmp(longarg, string)) /* If there wasn't a longarg..*/
			longarg = NULL;

	if(longarg)
		string[(longarg-string)-1] = '\0';

	if(create_raw_vector(argc, argv, string, " ")) {
		return -1;
	}

	i = *argc;

	if(longarg)
		argv[i] = strdup(longarg), ++i;

	*argc = i;
	argv[*argc] = NULL;
	return 0;
}

int
create_raw_vector(int *argc, char *argv[MAX_ARG], char *string, char *delim)
{
	char *ap;
	int i = 0;

	while((ap = strsep(&string, delim)) != NULL) {
		if(*ap != '\0') {
			argv[i] = strdup(ap);
			if(i < MAX_ARG)
				++i;
			else
				break;
		}
	}
	
	if(i == MAX_ARG) {
		free_vector(argv, i);
		return -1;
	}
	
	*argc = i;
	argv[*argc] = NULL;
	
	return 0;
}

void
free_vector(char **argv, int argc)
{
	while(argc)
		free(argv[argc--]);
	free(argv[0]);
}



