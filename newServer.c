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

    //just for testing
    char DataSend_M[12], DataSend_R[8], DataSend_W[4];
    DataSend_M[0]='P';DataSend_W[0]='D';
    int range = 1;//다이스트라 탐색범위.. 

    struct car_info {
	char color;
	int cost;
    };

    int i,j;

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

    while(1)//outer infinite loop
    {
        client_addr_size = sizeof(client_addr);
        client_socket = accept(server_socket, (struct sockaddr*)&client_addr, &client_addr_size);
        if ( -1 == client_socket) {
            printf( "클라이언트 연결 수락 실패\n");
            exit(1);
        }
/*Number 1*/      
	   while (1) {	
            char buf[10]; 
            file = fopen("locations.txt", "r");
            while (fgets(buf, 100, file) != NULL)
                for ( int i=0; i<8; i++ )
                    DataSend_R[i] = buf[i];
            fclose(file);
	        write( client_socket, DataSend_R, strlen( DataSend_R )+1 );
 	        read( client_socket, buff_rcv, BUFF_SIZE);
	        if (buff_rcv[0] == 'C') { 
                    for ( int i=0; i<4; i++ ){
                        DataSend_M[i+2] = buff_rcv[i+1];
		        DataSend_W[i+2] = buff_rcv[i+5];
	            }
		    break;
	        }
	    }
	//if request(C:call) singal comes in, save data and move on to next
	//strlen(REQUEST) will be 9 ('C' + present[4] + dest[4])


/*Number 2*/
	//initialize
	    struct car_info carlist[10];
	    for ( i=0; i<10; i++ ){
	        carlist[i].cost = -1;
	        carlist[i].color = '\0';
	    }

    	int index = 0;
	    int range = 1;// initialize range as 1..
	    while (1){   	
    	    DataSend_M[0] = 'P';
            DataSend_M[1] = range;
            write( client_socket, DataSend_M, strlen( DataSend_M )+1 );
            sleep(1);
//	    color ack; ack.red = -1; ack.blue = -1;
	//should wait until ack signal comes in.. 

	        for ( int i=0; i<10; i++ ){
	            CARLIST : read( client_socket, buff_rcv, BUFF_SIZE );
    	        if ( buff_rcv[0] == 'a' ){ //a: acknowledge
		            for ( j=0; j<index; j++ )//check if this color already exist in carlist
			            if (carlist[j].color == buff_rcv[1]) goto CARLIST;
		            carlist[index].color = buff_rcv[1];
	 	            carlist[index].cost = buff_rcv[2];
		            index++;
		        }
	        }
	        for ( i=0; i<10; i++ )
		        if (carlist[i].cost > 0) { 
		            index--; goto FINDMIN;
		        }
	        range++; // if all value is lower than 0, add 1 to range
	    }
/*Number 3*/
        int mini = 0;
	    FINDMIN : 
	    for ( i=0; i<index; i++ )
  	        if (carlist[i].cost < carlist[mini].cost) mini = i;
	    write( client_socket, carlist[mini].color, strlen(carlist[mini].color)+1 ); // send selected car info

        //run run run until get User's Present location..
        int running = 1;
        int onBoard = 0;
        while ( running ){
            char buf[10] = {'\0'}; 
            file = fopen("locations.txt", "r");
            while (fgets(buf, 100, file) != NULL)
                for ( int i=0; i<8; i++ )
                    DataSend_R[i] = buf[i];
            fclose(file);
            write( client_socket, DataSend_R, strlen(DataSend_R)+1 );
            read( client_socket, buff_rcv, BUFF_SIZE);
            
            if ( buff_rcv[0] == 'U' ){ // receive from mobile application 
                if ( buff_rcv[1] == 1 ){ //arrive at User's Present Location
                    onBoard = 1;
                    DataSend_W[1] = onBoard;
                    write( client_socket, DataSend_W, strlen(DataSend_W)+1 );
                }
                else if ( buff_rcv[1] == 0 ){ 
                    onBoard = 0; // arrive at destination
                    running = 0;
                    write( client_socket, , strlen('0')+1 );
                }
                do {
                    read( client_socket, buff_rcv, BUFF_SIZE );
                    }while(buff_rcv[0] != 'a');
            }
        }
	
        //if server receives distance value and all the values are returning z value, weight++;
        //if min value is defined, send the activation data to the car which has the min value
        sprintf( buff_snd, "%d : %s", strlen( buff_rcv), buff_rcv);
        close( client_socket);
    }
}














