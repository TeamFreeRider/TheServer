/*
OrderStatus 변수 설명

[B(blank): 서버 클라이언트멍때림 (클라이언트가 자신의 시리얼넘버만 보내는 상태)]
서버 -> 클라이언트 : 보내는거 X, 
/// 클라이언트 -> 서버 : 자신의 시리얼넘버 보냄(연결하려는 의지 표현), 상태변수

[M(Mobile): 모바일에서 사용자 관련 정보들을 입력하면 서버에서 클라이언트한테 사용자 정보를 입력한다.]
서버 -> 클라이언트 : 사용자 위치,현재 자동차의 위치 /// 클라이언트 -> 서버 : 시리얼 넘버, 상태변수

[D(Distance): 클라이언트에서 다익스트라를 돌린 다음에 거리값을 반환한다.]
서버 -> 클라이언트 : 보내는거 X 
/// 클라이언트 -> 서버 : 시리얼 넘버, 상태변수, 거리값

[S(Selected):그런 다음에 서버에서 모든 거리값들을 비교한 다음에 서버에서 거리 반환한 모든 클라이언트들에게 선택된 클라이언트의 고유코드를 반환한다.]
서버 -> 클라이언트 : 모든 차들에게 선택된 시리얼 넘버를 보낸다 /// 클라이언트 -> 서버 : 시리얼 넘버, 상태변수

[W():핸드폰으로부터 사용]//선택된 차에게만 보내진다
서버 -> 클라이언트 : 핸드폰 입력 여부 확인 /// 클라이언트 -> 서버 : 시리얼 넘버, 상태변수

[R(Run):만약 선택되면 선택된 차는 모터를 활성화 시키고 서버로부터 지속적으로 자신의 위치와 다른 자동차의 위치를 받는다.]
서버 -> 클라이언트 : 자동차1 위치, 자동차2 위치/// 클라이언트 -> 서버 : 시리얼 넘버, 상태변수

[F(Finish):만약 자동차가 사용자의 목적지에 도착한다면 활성화 변수를 중단 시키고 서버한테 다시 멍때리는 상태가 된다.]
서버 -> 클라이언트 : 자동차1 위치, 자동차2 위치 /// 클라이언트 -> 서버 : 시리얼 넘버, 상태변수

*/
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <arpa/inet.h>
#include <sys/types.h>
#include <sys/socket.h>

#define  BUFF_SIZE   1024

int main( int argc, char **argv)
{
    int calculated_best = -1;//default value


    while(1){

        int client_socket;
        struct sockaddr_in   server_addr;
        char buff[BUFF_SIZE+5];
        char Data_send[3];
        //Data_send : (Car Serial), (orderstatus), (distance return)
        client_socket  = socket( PF_INET, SOCK_STREAM, 0);
        //calculated_best = 80;
        //you should write a code that reads the txt file and input the value of the variable
        if( -1 == client_socket){
            printf( "socket 생성 실패\n");
            exit(1);
        }
        memset( &server_addr, 0, sizeof( server_addr));
        server_addr.sin_family = AF_INET;
        server_addr.sin_port = htons(4000);
        server_addr.sin_addr.s_addr= inet_addr("192.168.0.16");//write the server ip address
        if( -1 == connect( client_socket, (struct sockaddr*)&server_addr, sizeof( server_addr) ) ){
            printf( "접속 실패\n");
            exit(1);
        }

/*
        Data_send[0] = 'R';

            write( client_socket, Data_send, strlen(Data_send) + 1);
            sleep(1);
            read ( client_socket, buff, BUFF_SIZE);
            printf("%s\n", buff);
*/
        
        Data_send[0] = 'x';
        Data_send[1] = 'x';
        Data_send[2] = 'x';

        write(client_socket, Data_send, strlen(Data_send)+1);
        printf("%s\n", Data_send);
        
        read(client_socket, buff, BUFF_SIZE);
        printf("%s\n", buff);



/*
            if (buff[0]=='P') {
                Data_send[0] = 'a'; Data_send[1] = 'R';
                Data_send[2] = '3';
            }
*/    





        close(client_socket);  
        sleep(1);//1 second    
     }
     return 0;
}

