#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <arpa/inet.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <math.h>

#define  BUFF_SIZE   1024

double distance(int, int, int, int);

int CharToInt(char chunk[], int index, int length)
{
   int result = 0; int i;
   char arr[2];
   for (i = 0; i < length; i++)
   {
      arr[i] = chunk[i + index];
   }
   result = atoi(arr);
   return result;//ascii 변환 코드로 반영
}

int main(int argc, char **argv)
{
   int userloc[2], dest[2], weight, carloc[2], interruptloc[2], onboard;//다익스트라에 필요한 각종 변수들을 선언한다.
   int calculated_best = -1;//기본값이 -1이고 이 변수는 다이스트라에서 거리계산한 결과값을 저장하는 변수이다.

   char orderstatus = 'x';//클라이언트 자신의 진행 상태를 나타내주는 변수다. 코드 맨 밑에 상태변수 종류가 나열되어 있다.
                     /* orderstatus에 관한 변수
                     x: 자동차가 대기상태일때
                     y: cost 계산 후 데이터를 건내받고 select를 대기하는 상태
                     z: 현재 차가 사용자로부터 선택되어 있거나 운행되고 있는 상태여서 대기 상태가 아닌 상태s
                     */
   char serial = 'R';//자동차의 시리얼 넘버
   char selectedserial = 'x';//서버로부터 선택된 자동차의 시리얼 넘버를 저장하는 변수. 만약 x 이면 아직 아무런 값도 전달받지 못한 것이다.
   char Activate_arduino = 'F';//아두이노 구동 여부를 확인해주는 boolean 변수. 참이면 T이다
   char bool_UserReceived = 'F';//서버로부터 도착지점을 제외한 정보를 받았는가 여부를 확인해주는 변수, 거짓이 F, 참이 T이다.
   
   //char Activate_wait = 'F';//
   char Data_Received[100];//서버로부터 전송받은 데이터를 저장하는 변수  

   while (1) {

      int client_socket;

      struct sockaddr_in   server_addr;
      char buff[BUFF_SIZE + 5];
      char Data_send[100];//서버한테 보낼 내용들을 저장하는 변수
                     //Data_send[0] = 자동차 고유의 시리얼 넘버, Data_send[1] = 자동차의 상태 변수(x,y,z), Data_send[2] = 거리값 반환의 십의 자리, Data_send[3] = 거리값 반환의 일의 자리

      client_socket = socket(PF_INET, SOCK_STREAM, 0);

      if (-1 == client_socket)
      {
         printf("socket 생성 실패\n"); 
         exit(1);
      }
      memset(&server_addr, 0, sizeof(server_addr));
      server_addr.sin_family = AF_INET;
      server_addr.sin_port = htons(4000);
      server_addr.sin_addr.s_addr = inet_addr("192.168.0.16");//write the server ip address
      if (-1 == connect(client_socket, (struct sockaddr*)&server_addr, sizeof(server_addr)))
      {
         printf("접속 실패\n");
         exit(1);
      }

      char arr_t[20]; int k;
      if (calculated_best == -1)
      {
//         printf("다익스트라 알고리즘이 변수에 전달 안됨 \n"); //이거 거슬려서 일단 주석처리
      }
      else
      {
         sprintf(arr_t, "%d", calculated_best);
         for (k = 0; k < strlen(arr_t); k++)
         {
            Data_send[k + 2] = arr_t[k];
         }
      }
      Data_send[0] = orderstatus;
      Data_send[1] = serial;
                       //write( client_socket, argv[1], strlen( argv[1])+1);      // +1: NULL까지 포함해서 전송
                       //write( client_socket, "R", strlen("R") + 1);//sending its car serial number car1 : 1, car2 : 2
      write(client_socket, Data_send, strlen(Data_send) + 1);
      read(client_socket, buff, BUFF_SIZE);//실질적으로 buff에서 서버로부터 데이터를 받지만... 이쁘게 정리하기 위해서
                                  //Data_received변수에다가 저장한다.

      int i; int b = 0;
      for (i = 0; i < strlen(buff); i++)//이거는 서버에서 클라이언트로 올때 불필요한 쓰레기값들이 붙어서 오는데 불필요한 데이터를 제거하기 위한 코드이다.
      {
         if (buff[i] == 'E' || b == 1)//배열값이 들어오는데 E가 있으면 데이터가 끝난다는 것을 의미한다.
         {
            Data_Received[i] = '\0';
            b = 1;
         }
         else
         {
            Data_Received[i] = buff[i];
         }
      }

      switch (Data_Received[0])
      {
      case 'P'://배열값이 들어오는데 P가 있으면 x state // weight(d)값 & user's location 수신 --> 처음에 user가 요청했을 때 각 차에게 뿌리는 상태같음
         if (orderstatus == 'x'){   //x상태일때만 반응하도록
            orderstatus = 'y';
            bool_UserReceived = 'T';
            weight = CharToInt(Data_Received, 1, 2);
            userloc[0] = CharToInt(Data_Received, 3, 2);
            userloc[1] = CharToInt(Data_Received, 5, 2);
            printf("orderstatus : %c\n", orderstatus);
            //carloc[0] = ;
            //carloc[1] = ; P에서 carlocation 정보 안 들어 옴!!!! by 세령 
         } // <-- car의 lcoation이 들어와야 dijkstra를 돌린 후 cost를 계산할 수 있음. 미리 받아놨자낭ㅇ-ㅇby령
         break;
      case 'S':// car's location & interrupt location 수신 --> 이동할 때의 상태
         carloc[0] = CharToInt(Data_Received, 1, 2);
         carloc[1] = CharToInt(Data_Received, 3, 2);
         interruptloc[0] = CharToInt(Data_Received, 5, 2);
         interruptloc[1] = CharToInt(Data_Received, 7, 2);
         break;
      case 'D'://배열값이 들어오는데 D가 있으면 onboard, destination location 수신. --> user가 탑승한 경우에 주는것같음
               //하차할때도 D로 첫 칸이 온다. onboard값이 1이면 승차한거고 0이면 하차한거임 세령이가 수정함
         printf("received %c, Data_Received[1] : %c\n",Data_Received[0], Data_Received[1]);
         if (orderstatus == 'z' && Data_Received[1] == '1'){
            onboard =1;
            dest[0] = CharToInt(Data_Received, 2, 2);
            dest[1] = CharToInt(Data_Received, 4, 2);
            Activate_arduino = 'T';
            printf("%s : user checked in the car\n",Data_Received);
         }
         else if (orderstatus == 'z' && Data_Received[1] == '0'){ 
            onboard =0;
            printf("%s : user checked out the car\n",Data_Received);
            orderstatus = 'x';
         }
         break;
      case 'C'://C 배열에서 broadcasting되는 selected car가 자기자신이면 z로 바뀐다. 아니면 다시 x로
         selectedserial = Data_Received[1];
         if (orderstatus == 'y' && selectedserial == serial) //z상태일땐 selectedserial을 받아도 변동없게
         {
            orderstatus = 'z';
            if (Activate_arduino == 'T' && onboard == 0)
            {
            //호출자의 현재위치 지점까지 가는 다익스트라를 돌린다.
            }
            else if (Activate_arduino == 'T' && onboard == 1)
            {
               printf("Arduino activated");
            //a code to activate the arduino
            //호출자의 현재위치에서 목적지까지 가는 다이스트라 코드 삽입
            }
         }
         else if (orderstatus == 'y' && selectedserial != serial) 
         {
            orderstatus = 'x';
            selectedserial = 'x';
            bool_UserReceived = 'F';
//         selectedserial = Data_Received[1];
            break;
         }
      }
      /* orderstatus에 관한 변수
      x: 자동차가 대기상태일때
      y: cost 계산 후 데이터를 건내받고 select를 대기하는 상태
      z: 현재 차가 사용자로부터 선택되어 있거나 운행되고 있는 상태여서 대기 상태가 아닌 상태
      */










      if (orderstatus == 'y' && bool_UserReceived == 'T')
      {
         //run the dijkstra
         //calculated_best = 결과값
         if (calculated_best == -1) // 이 코드 추가 calculated_best == -1 || 다익스트라반환값 == z 인 경우
         {// 언제 calculated_best를 -1로 주는지 설명 필요.
            Data_send[2] = 'Z';//이거 상태변수 아니니깐 주의바람
            Data_send[2] = '7';//이건 실험때문에 넣은 값임
         }
         else if (calculated_best != -1) //이거 십/일의 자리 나누지 말고 한 칸에 넣을게연 by세령 
         {
            if (calculated_best <= weight * 10) 
            { // 계산된 cost가 정해진 range(weight에 의해 결정) 이내에 있을 경우에만 Data_send 정의
               Data_send[2] = calculated_best + '0';//거리값의 일의 자리
            }//거리값은 100이상을 가지지않는다고 가정하고 짠 코드이니깐 유의바람
            // range 밖에 있다면 아무런 작업하지않고, do not send any Data send to server.
         }
      }

      // ↑ Dijkstra를 돌려서 반환하기 위한 Data_send 결정 후, Data_send를 server로 송신
      // ↓ server가 Data_send를 받은 후, Serial을 골라서 차들에게 넘겨줌. 
/*
      if (orderstatus == 'y' && selectedserial == serial)
      {
         orderstatus = 'z';
         if (Activate_arduino == 'T' && onboard == 0)
         {
            //호출자의 현재위치 지점까지 가는 다익스트라를 돌린다.
         }
         else if (Activate_arduino == 'T' && onboard == 1)
         {
            printf("Arduino activated");
         
            //a code to activate the arduino
            //호출자의 현재위치에서 목적지까지 가는 다이스트라 코드 삽입
         }
      }
      else if (orderstatus == 'y' && selectedserial != serial) 
      {
         orderstatus = 'x';
         selectedserial = 'x';
         bool_UserReceived = 'F';
      }
*/
/*      if (distance(userloc[0], userloc[1], dest[0], dest[1]) <= 1.5)
      {
         Activate_arduino = 'F';//false

                           //resetting the variables
         orderstatus = 'x';
         selectedserial = 'x';
         Activate_arduino = 'F';
         bool_UserReceived = 'F';
         onboard = 0;
      }
*/
 //     printf("%s\n", buff);
      printf("Data_Received : %s, orderstatus : %c\n", Data_Received, orderstatus);
 //     printf("%d \n", userloc[0]); printf("%d \n", userloc[1]); printf("%d \n", dest[0]); printf("%d \n", dest[1]); printf("%d \n", weight);
 //     printf("%d \n", carloc[0]); printf("%d \n", carloc[1]); printf("%d \n", interruptloc[0]); printf("%d \n", interruptloc[1]);
      close(client_socket);

      //아래에 if문들이...쭉 있는데 대기상태인 자동차가 서버랑 각종 통신 과정들을 거치고 나서 자동차가 선택되면
      //가동이 되도록 하거나 아니면 선택받지를 못하면 다시 대기 상태로 가게 하는 순서를 코드로 나타낸 것이다.
      //여담인데 알고리즘에서 결함이 있을 수도 있으니 주의... ㅋㅋㅋ

      sleep(1);//1 second    
   }
   return 0;
}

double distance(int n1, int n2, int m1, int m2)
{
   return sqrt(pow(n2 - n1, 2) + pow(m2 - m1, 2));
}