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

      int   client_socket;
      struct sockaddr_in   server_addr;
      char   buff[BUFF_SIZE+5];
      //char c_UserPosX[3], c_UserPosY[3], c_UserDesX[3], c_UserDesY[3], c_weight;//it is temp.. 
      int User_PosX, User_PosY, User_DesX, User_DesY, weight, Car_PosX, car_PosY; 
      //you should write a code that reads the txt file and input the value of the variable

      client_socket  = socket( PF_INET, SOCK_STREAM, 0);

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
      //write( client_socket, argv[1], strlen( argv[1])+1);      // +1: NULL까지 포함해서 전송
      write( client_socket, "1", strlen("1") + 1);//sending its car serial number car1 : 1, car2 : 2
      read ( client_socket, buff, BUFF_SIZE);

      User_PosX = CharToInt(buff,2,3);
      User_PosY = CharToInt(buff,6,3);
      User_DesX = CharToInt(buff,12,3);
      User_DesY = CharToInt(buff,16,3);
      weight = CharToInt(buff,22,2);

      printf( "%d \n", User_PosX); printf( "%d \n", User_PosY); printf( "%d \n", User_DesX); printf( "%d \n", User_DesY); printf( "%d \n", weight);
      //sscanf(myarray, "%d", &i);
      close(client_socket);  
      sleep(1);//1 second    
   }
   return 0;
}
