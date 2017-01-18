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
#include <time.h>
#include <arpa/inet.h>
#include <sys/types.h>
#include <sys/socket.h>


#define  BUFF_SIZE   1024

        
int main (void)
{
    FILE *file;

    int i,j;

    time_t bfrSEC, aftrSEC;

    int server_socket;
    int client_socket;
    int client_addr_size;

    struct sockaddr_in   server_addr;
    struct sockaddr_in   client_addr;

    char buff_rcv[BUFF_SIZE+5];//26,30
    char buff_snd[BUFF_SIZE+5];

    //P: present, R:running, D: destination, C: car..
    char DataSend_P[8], DataSend_R[10], DataSend_D[8], DataSend_C[3];
    DataSend_P[0]='P'; DataSend_R[0]='S'; DataSend_D[0]='D'; DataSend_C[0]='C'; 
    DataSend_P[6]='E'; DataSend_R[9]='E'; DataSend_D[6]='E'; DataSend_C[2]='E';
    //int range = 1;//다이스트라 탐색범위.. 
    struct car_info {
	char color;
	int cost;
    };

    struct car_info carlist[5];

    for ( i=0; i<5; i++ ){
        carlist[i].cost = -1;
        carlist[i].color = '\0';
    }

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
        int onBoard = 0;
        int range = 1;// initialize range as 1..
        int mini = 0;
        int index = 0;
        int receive = 0;

        client_addr_size = sizeof(client_addr);
        client_socket = accept(server_socket, (struct sockaddr*)&client_addr, &client_addr_size);
        if ( -1 == client_socket) {
            printf( "failed to accept\n");
            exit(1);}

        read(client_socket, buff_rcv, BUFF_SIZE);
        printf("%s\n", buff_rcv);

        close( client_socket );
        switch (buff_rcv[0]) {

            case 'x' : 

/*Number 1*/      
	   while (1) {	
        
            client_addr_size = sizeof(client_addr);
            client_socket = accept(server_socket, (struct sockaddr*)&client_addr, &client_addr_size);
            if ( -1 == client_socket) {
                printf( "fail\n");
                exit(1);
            }
        
            char buf[10]; 
            file = fopen("locations.txt", "r");
            while (fgets(buf, 10, file) != NULL)
                for ( int i=0; i<8; i++ )
                    DataSend_R[i+1] = buf[i];
            fclose(file);
	        write( client_socket, DataSend_R, strlen( DataSend_R )+1 );
            printf("%s\n", DataSend_R);

            read( client_socket, buff_rcv, BUFF_SIZE);            
	        if (buff_rcv[0] == 'C'){ 
                for ( int i=0; i<4; i++ ){
                    DataSend_P[i+2] = buff_rcv[i+1];
		            DataSend_D[i+2] = buff_rcv[i+5];
	            }
                printf("user call : %s\n",buff_rcv);
                printf("break\n");
		        break; 
	        }
            printf("rewind\n");

        }
	//if request(C:call) singal comes in, save data and move on to next
	//strlen(REQUEST) will be 9 ('C' + present[4] + dest[4])

            time(&bfrSEC);
        do {    
            client_addr_size = sizeof(client_addr);
            client_socket = accept(server_socket, (struct sockaddr*)&client_addr, &client_addr_size);
            if ( -1 == client_socket) {
                printf( "fail\n");
                exit(1);
            }
            printf("out of 1st loop\n");
            DataSend_P[0] = 'P';
            DataSend_P[1] = range + '0';
            write( client_socket, DataSend_P, strlen( DataSend_P )+1 );
            printf("%s를 보냄\n", DataSend_P);
            close ( client_socket );
            time(&aftrSEC);
        } while(aftrSEC - bfrSEC < 2);


            case 'y' :  
/*Number 2*/

        RECEIVE : 
        while (1){
            do  {        
                client_addr_size = sizeof(client_addr);
                client_socket = accept(server_socket, (struct sockaddr*)&client_addr, &client_addr_size);
                if ( -1 == client_socket) {
                    printf( "fail\n");
                    exit(1);
                }
                read( client_socket, buff_rcv, BUFF_SIZE );
            }while(buff_rcv[2] == '\0');
            printf("%s\n", buff_rcv);

            int exist =0;
            

		    for ( j=0; j<index; j++ )//check if this color already exist in carlist
                if (carlist[j].color == buff_rcv[1]){ 
                    exist++;
                    printf("already exist, color: %c\n", buff_rcv[1]);
                }
            if ( exist == 0 && buff_rcv[2] != '\0'){ // save new data
                carlist[index].color = buff_rcv[1];
                carlist[index].cost = buff_rcv[2];
                index++;
                printf("saved!!\n");
            }    
            receive += 1;
            printf("receive : %d\n", receive);

            if (receive < 5 ) {
                printf("back to receive\n");
                close(client_socket);
                goto RECEIVE;
            }
	        //Find Minimum value..
            else if ( receive >= 5 && index > 0 ) break;

	        else range++; // if all value is lower than 0, add 1 to range
        }
        close(client_socket);


	    FINDMIN : 

        printf("out of 2nd loop\n");
	    
        if (index == 1) 
            DataSend_C[1] = carlist[index].color;
        else   
            for ( i=0; i<index; i++ )
  	             if (carlist[i].cost < carlist[mini].cost) mini = i;

        printf("%c\n",carlist[mini].color);
        DataSend_C[1]= carlist[mini].color;
	    for (int i=0; i<index; i++){
            client_addr_size = sizeof(client_addr);
            client_socket = accept(server_socket, (struct sockaddr*)&client_addr, &client_addr_size);
            if ( -1 == client_socket) {
                printf( "fail\n");
                exit(1);
            }
            write( client_socket, DataSend_C, strlen(DataSend_C)+1 ); // send selected car info
            close(client_socket);
        }
        
        //run run run until get User's Present location..
/*
        int onBoard = 0;
*/

            case 'z' :
/*Number 3*/
        while (1){
            do{
                client_addr_size = sizeof(client_addr);
                client_socket = accept(server_socket, (struct sockaddr*)&client_addr, &client_addr_size);
                if ( -1 == client_socket) {
                    printf( "fail\n");
                    exit(1);
                }
                read( client_socket, buff_rcv, BUFF_SIZE);
                printf("%s\n", buff_rcv);
            }while(buff_rcv[0] == 'x');
            
            write( client_socket, buff_snd, strlen(buff_snd) );
            if (buff_snd[0] == 'D' && buff_snd[1] == '0') break;
            
            if ( buff_rcv[0] == 'U' ){ // receive from mobile application 
                if ( buff_rcv[1] == '1' ){ //arrive at User's Present Location
                    printf("user checked in the car \n");
                    onBoard = 1; // user is now on board
                    DataSend_D[1]='1';DataSend_D[7]='\0';
                    //memset(DataSend_D, 0, sizeof(DataSend_D));
                    strcpy(buff_snd, DataSend_D);
                    printf("U1 -> %s\n", DataSend_D);
                }
                else if ( buff_rcv[1] == '0' ){ 
                    printf("user checked out the car \n");
                    onBoard = 0; // arrive at destination
                    DataSend_D[0]='D';DataSend_D[1]='0';DataSend_D[2]='E';DataSend_D[3]='\0';
                    strcpy(buff_snd, DataSend_D);
                    printf("U0 -> %s\n", DataSend_D);
                }
                printf("%s\n", DataSend_D);
            }
            else {
                char buf[10];
                file = fopen("locations.txt", "r");
                while (fgets(buf, 10, file) != NULL)
                    for ( int i=0; i<8; i++ )
                        DataSend_R[i+1] = buf[i];
                fclose(file);
                DataSend_R[7]='\0';
                strcpy(buff_snd, DataSend_R);
                printf("%s\n", DataSend_R);
            }
            close(client_socket);
        } // 3rd loop
    } // switch 
	
        //if min value is defined, send the activation data to the car which has the min value
        close( client_socket );
    }
}


