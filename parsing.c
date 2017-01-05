#include <stdio.h>

int main() {
    FILE *file;
    char buf[1000];

    file = fopen("locations.txt", "r");
    if (!file) return 1;
    
    while (fgets(buf, 1000, file) != NULL) 
	printf("%s", buf);

    fclose(file);
    return 0;
}
