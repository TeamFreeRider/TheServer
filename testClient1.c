/*
OrderStatus 변수 설명
B: 서버랑 클라이언트가 멍때리고 있다가 (클라이언트가 자신의 시리얼넘버만 보내는 상태)
M: 모바일에서 사용자 관련 정보들을 입력하면 서버에서 클라이언트한테 사용자 정보를 입력한다.
D: 클라이언트에서 다익스트라를 돌린 다음에 거리값을 반환한다. 
R:그런 다음에 서버에서 모든 거리값들을 비교한 다음에 서버에서 거리 반환한 모든 클라이언트들에게 선택된 클라이언트의 고유코드를 반환한다.
W:핸드폰으로부터 사용
A:만약 선택되면 선택된 차는 모터를 활성화 시키고 서버로부터 지속적으로 자신의 위치와 다른 자동차의 위치를 받는다. 
F:만약 자동차가 사용자의 목적지에 도착한다면 활성화 변수를 중단 시키고 서버한테 다시 멍때리는 상태가 된다. 
*/
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
   char arr[2];
   for(i = 0 ; i < length ; i++)
   {
      arr[i] = chunk[i + index];
   }
   result = atoi(arr);
   return result;//ascii 변환 코드로 반영
}

int main( int argc, char **argv)
{
   int userloc[2], dest[2], weight, carloc[2], interruptloc[2], onboard;
   int calculated_best = -1;//default value

   char orderstatus = 'B';
   //B,M,D,R,W,A,F... 
   char selectedserial = 'x';

   while(1){

      int client_socket;

      struct sockaddr_in   server_addr;
      char buff[BUFF_SIZE+5];
      char Data_send[100];
      //Data_send : (Car Serial), (orderstatus), (distance return)
      client_socket  = socket( PF_INET, SOCK_STREAM, 0);


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

      Data_send[0] = 'R';//this is not orderstatus... be aware of that
      Data_send[1] = orderstatus;
      //write( client_socket, argv[1], strlen( argv[1])+1);      // +1: NULL까지 포함해서 전송
      //write( client_socket, "R", strlen("R") + 1);//sending its car serial number car1 : 1, car2 : 2
      write( client_socket, Data_send, strlen(Data_send) + 1);
      read ( client_socket, buff, BUFF_SIZE);

      userloc[0] = CharToInt(buff,0,2);
      userloc[1] = CharToInt(buff,2,2);
      dest[0] = CharToInt(buff,4,2);
      dest[1] = CharToInt(buff,6,2);
      weight = CharToInt(buff,8,2);
      carloc[0] = CharToInt(buff,10,2);
      carloc[1] = CharToInt(buff,12,2);
      interruptloc[0] = CharToInt(buff,14,2);
      interruptloc[1]= CharToInt(buff,16,2);
      //printf("%s\n", buff);
      printf( "%d \n", userloc[0]); printf( "%d \n", userloc[1]); printf( "%d \n", dest[0]); printf( "%d \n", dest[1]); printf( "%d \n", weight);
      printf( "%d \n", carloc[0]); printf( "%d \n", carloc[1]); printf( "%d \n", interruptloc[0]); printf( "%d \n", interruptloc[1]);
      close(client_socket);  
      sleep(1);//1 second    
   }
   return 0;
}
