#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <arpa/inet.h>
#include <sys/types.h>
#include <sys/socket.h>

#define  BUFF_SIZE   1024

int CharToInt(char chunk[], int index, int length);


int main( int argc, char **argv)
{

   while(1){

      int client_socket;

      struct sockaddr_in   server_addr;
      char buff[BUFF_SIZE+5];
      char Data_send[100];

      client_socket  = socket( PF_INET, SOCK_STREAM, 0);

      int User_PosX, User_PosY, User_DesX, User_DesY, weight, Car_PosX, Car_PosY, In_locX, In_locY;
      int calculated_best = -1;//default value
      //calculated_best = 80;
      //you should write a code that reads the txt file and input the value of the variable
      //reply - no you don't need to

      if( -1 == client_socket)
      {
         printf( "socket 생성 실패\n");
         exit(1);
      }

      memset( &server_addr, 0, sizeof( server_addr));
      server_addr.sin_family = AF_INET;
      server_addr.sin_port = htons(4000);
      server_addr.sin_addr.s_addr= inet_addr("192.168.0.16");//write the server ip address

      if( -1 == connect( client_socket, (struct sockaddr*)&server_addr, sizeof( server_addr) ) )
      {
         printf( "접속 실패\n");
         exit(1);
      }

      char arr_t[20]; int k;
      if(calculated_best == -1)
      {printf("다익스트라 알고리즘이 변수에 전달 안됨 /n");}
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

      User_PosX = CharToInt(buff,0,3);
      User_PosY = CharToInt(buff,3,3);
      User_DesX = CharToInt(buff,6,3);
      User_DesY = CharToInt(buff,9,3);
      weight = CharToInt(buff,12,3);
      Car_PosX = CharToInt(buff,15,3);
      Car_PosY = CharToInt(buff,18,3);
      In_locX = CharToInt(buff,21,3);
      In_locY CharToInt(buff,24,3);
      //printf("%s\n", buff);
      printf( "%d \n", User_PosX); printf( "%d \n", User_PosY); printf( "%d \n", User_DesX); printf( "%d \n", User_DesY); printf( "%d \n", weight);
      printf( "%d \n", Car_PosX); printf( "%d \n", Car_PosY);
      close(client_socket);  
      sleep(1);//1 second    
   }
   return 0;
}


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
