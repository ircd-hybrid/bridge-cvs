#ifndef __STRING_H_INCLUDED
#define __STRING_H_INCLUDED

#define MAX_ARG 64

int create_vector(int *, char *[MAX_ARG], char *);
int create_raw_vector(int *, char *[MAX_ARG], char *, char *);
void free_vector(char **, int);

#endif
