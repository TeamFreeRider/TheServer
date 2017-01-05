#include "stdio.h" 
#include "string.h" 

int main() 
{ 
  int line_number = 0, count = 0; 
  FILE *p_file = NULL; 
  char buffer[1024]; 

  printf("출력할 행의 번호를 입력하세요 : "); 
  scanf("%d", &line_number); 

  p_file = fopen("locations.txt", "rt"); 
  if(p_file != NULL){ 
      while(fgets(buffer, 1024, p_file) != NULL){ 
          if(strchr(buffer, '\n') != NULL) count++; 
          if(line_number == count){ 
              printf("%s", buffer); 
              break; 
          } 
      } 
      fclose(p_file); 
  } 
} 
