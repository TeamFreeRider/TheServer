#include <stdio.h>
#include <stdlib.h>
#include <string.h>

this code is for parsing locations.txt

char str[999];
FILE * file;
file = fopen("locations.txt", "r");
if (file) {
    while (fscanf(file, "%s", str) != EOF)
	printf("%s",str);
    fclose;
}


