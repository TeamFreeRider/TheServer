// When Call User sends present locations and destination location to server,
// Server sends User's present location, Blue's location and Red's location
// user's destination point can be sent after car arrives at user's point.. 
// client returns 'distance value' to server if it's located near enough.
// After a car is designated, Server sends Data_send[8] to client.
// This array has Red's loc and Blue's loc
// When journey to user's loc is finished, 
// Server sends the user's destination, Red and blue's loc information. arry[12]
// Server receives 'onBoard' value from Call User and hands it over to client 
// send Data_send[8] to client.. 

//Data_send_M[8] : user's present, destination, R or B's locations
//Data_send_R[8] : Red and Blue's locations... default mode
//Data_send_W[5] : 1/Y and user's destination locations


#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <arpa/inet.h>
#include <sys/types.h>
#include <sys/socket.h>

#define  BUFF_SIZE   1024
#define  REQUEST    0


int fileOpen();

int main (void)
{

    FILE *file;

    int server_socket;
    int client_socket;
    int client_addr_size;

    struct sockaddr_in   server_addr;
    struct sockaddr_in   client_addr;

    char buff_rcv[BUFF_SIZE+5];//26,30
    char buff_snd[BUFF_SIZE+5];

    char DataSend_M[12], DataSend_R[8], DataSend_W[5];
    char User_Location[4], User_Destination[4];
    
    //char User_Pos[2], User_Des[2], weight;
    //order : P,user position x,user position y , u user destination x, user destination y, weight, 차1의 좌표, 차2의 좌표
    int range = 1;//다이스트라 탐색범위.. 

    //User_Pos[0] = 100; User_Pos[1] = 200; User_Des[0] = 129; User_Des[1] = 547;

    server_socket = socket( PF_INET, SOCK_STREAM, 0);
    if( -1 == server_socket)
    {
        printf( "server socket 생성 실패\n");
        exit(1);
    }

    //set ip address and port number
    memset( &server_addr, 0, sizeof( server_addr));
    server_addr.sin_family = AF_INET;
    server_addr.sin_port = htons(4000);
    server_addr.sin_addr.s_addr = htonl(INADDR_ANY);

    //bind is needed for saving address information, ip & port info is connected to socket
    if( -1 == bind( server_socket, (struct sockaddr*)&server_addr, sizeof( server_addr) ) ){
        printf( "bind() 실행 에러\n");
        exit(1);
    }

    if( -1 == listen(server_socket, 5)){
        printf( "listen() 실행 실패\n");
        exit(1);
    }

    client_addr_size = sizeof(client_addr);
    client_socket = accept(server_socket, (struct sockaddr*)&client_addr, &client_addr_size);

    if ( -1 == client_socket){
        printf( "클라이언트 연결 수락 실패\n");
        exit(1);
      }

// Above code is for accepting connections from client
/*
    if ( REQUEST == 1 ){
        strcpy( Data_send_M, User_Location );
        char* Locations = fileOpen();
        for ( int i=0; i<8; i++ )
            Data_send_M[i+4] = Locations[i];
        write( clinet_socket, Data_send_M, strlen);
    }              

    struct weight {
        char color;
        int distance;
    };

    struct weight car1;
    struct weight car2;

    read (clinet_socket, buff_rcv, BUFF_SIZE);
*/


    while(1)//infinite loop
    {      
        //flush array
        for ( int i=0; i<10; i++ )
	      Data_send[i] = 0; 

        //buf[10] is for reading locations text file 
        char buf[10];
        file = fopen("locations.txt", "r");
        while (fgets(buf, 100, file) != NULL)
            for ( int i=0; i<8; i++ )
                Data_send[i] = buf[i];
        fclose(file);

        read ( client_socket, buff_rcv, BUFF_SIZE);
        printf( "receive: %s\n", buff_rcv);
        //send weight, User_info data to the client
        //receive distance value from the client
        //if server receives distance value and all the values are returning z value, weight++
        //if min value is defined, send the activation data to the car which has the min value
        sprintf( buff_snd, "%d : %s", strlen( buff_rcv), buff_rcv);
        //write( client_socket, buff_snd, strlen( buff_snd)+1);          // +1: NULL까지 포함해서 전송
        write( client_socket, Data_send, strlen(Data_send)+1);
    }

    close( client_socket);
 }




char fileOpen(){
    char buffer[8]; Locations[8];
    file = fopen("locations.txt", "r");
    while (fgets(buffer, 100, file) != NULL)
        for (int i=0; i<8; i++)
            Locations[i] = buffer[i];
    fclose(file);
    return Locations;
}












