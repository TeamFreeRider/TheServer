// mobile client 


#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <arpa/inet.h>
#include <sys/types.h>
#include <sys/socket.h>

#define  BUFF_SIZE   1024

int main( int argc, char **argv){

//      char arr[9]={1,2,3,4,5,6,7,8,9};
//   	char onBoard[3];
	int play;
      int client_socket;

      struct sockaddr_in   server_addr;
      char buff[BUFF_SIZE+5];
      char Data_send[100];

      client_socket  = socket( PF_INET, SOCK_STREAM, 0);


      //calculated_best = 80;
      //you should write a code that reads the txt file and input the value of $

      if( -1 == client_socket){
         printf( "socket 생성 실패\n");
         exit(1);
      }

      memset( &server_addr, 0, sizeof( server_addr));
      server_addr.sin_family = AF_INET;
      server_addr.sin_port = htons(4000);
      server_addr.sin_addr.s_addr= inet_addr("192.168.0.6");//write the server$

      if( -1 == connect( client_socket, (struct sockaddr*)&server_addr, sizeof( server_addr) ) )
      {
         printf( "접속 실패\n");
         exit(1);
      }
      
      char arr[9] = {'1','2','6','1','4','0','3','0','0'};
      int i;
      arr[0] = 'C';      
   
      write( client_socket, arr, sizeof(arr)+1 );
      printf("%s\n", arr);

      read( client_socket, arr, sizeof(arr)+1 );
      printf("%s\n", arr);
      close(client_socket);

      scanf("%d", &play);
      Data_send[0] = 'U';
      Data_send[1] = play + '0';
      printf("%s\n", Data_send);

      
      if( -1 == client_socket){
         printf( "socket 생성 실패\n");
         exit(1);
      }

      memset( &server_addr, 0, sizeof( server_addr));
      server_addr.sin_family = AF_INET;
      server_addr.sin_port = htons(4000);
      server_addr.sin_addr.s_addr= inet_addr("192.168.0.6");//write the server$

      if( -1 == connect( client_socket, (struct sockaddr*)&server_addr, sizeof( server_addr) ) )
      {
         printf( "접속 실패\n");
         exit(1);
      }
      
      write(client_socket, Data_send, sizeof(Data_send)+1);


      close(client_socket);
      sleep(1);//1 second

   return 0;
}




