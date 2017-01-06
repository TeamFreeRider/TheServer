#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <arpa/inet.h>
#include <sys/types.h>
#include <sys/socket.h>

#define  BUFF_SIZE   1024

int CharToInt(char chunk[], int index, int length)
{
   int result = 0; int i;
   char arr[3];
   for(i = 0 ; i < length ; i++)
   {
      arr[i] = chunk[i + index];
   }
   result = atoi(arr);
   return result;
}

int main( int argc, char **argv)
{

   while(1){

      int client_socket;

      struct sockaddr_in   server_addr;
      char buff[BUFF_SIZE+5];
      char Data_send[100];
      char selectedserial = 'x';

      client_socket  = socket( PF_INET, SOCK_STREAM, 0);

      int userloc[2], dest[2], weight, carloc[2], interruptloc[2], onboard;
      int calculated_best = -1;//default value
      //calculated_best = 80;
      //you should write a code that reads the txt file and input the value of the variable

      if( -1 == client_socket)
      {
         printf( "socket 생성 실패\n");
         exit(1);
      }

      memset( &server_addr, 0, sizeof( server_addr));
      server_addr.sin_family = AF_INET;
      server_addr.sin_port = htons(4000);
      server_addr.sin_addr.s_addr= inet_addr("192.168.0.21");//write the server ip address

      if( -1 == connect( client_socket, (struct sockaddr*)&server_addr, sizeof( server_addr) ) )
      {
         printf( "접속 실패\n");
         exit(1);
      }

      char arr_t[20]; int k;
      if(calculated_best == -1)
      {printf("다익스트라 알고리즘이 변수에 전달 안됨 \n");}
      else
      {
         sprintf(arr_t,"%d", calculated_best);
         for(k = 0 ; k < strlen(arr_t) ; k++)
         {
            Data_send[k + 1] = arr_t[k];
         }
      }

      Data_send[0] = 'R';
      //write( client_socket, argv[1], strlen( argv[1])+1);      // +1: NULL까지 포함해서 전송
      //write( client_socket, "R", strlen("R") + 1);//sending its car serial number car1 : 1, car2 : 2
      write( client_socket, Data_send, strlen(Data_send) + 1);
      read ( client_socket, buff, BUFF_SIZE);

      userloc[0] = CharToInt(buff,0,3);
      userloc[1] = CharToInt(buff,3,3);
      dest[0] = CharToInt(buff,6,3);
      dest[1] = CharToInt(buff,9,3);
      weight = CharToInt(buff,12,3);
      carloc[0] = CharToInt(buff,15,3);
      carloc[1] = CharToInt(buff,18,3);
      interruptloc[0] = CharToInt(buff,21,3);
      interruptloc[1]= CharToInt(buff,24,3);
      //printf("%s\n", buff);
      printf( "%d \n", userloc[0]); printf( "%d \n", userloc[1]); printf( "%d \n", dest[0]); printf( "%d \n", dest[1]); printf( "%d \n", weight);
      printf( "%d \n", carloc[0]); printf( "%d \n", carloc[1]); printf( "%d \n", interruptloc[0]); printf( "%d \n", interruptloc[1]);
      close(client_socket);  
      sleep(1);//1 second    
   }
   return 0;
}
