#include "FreeRider.h"

int interruptrecovery[V][3];
int recoveryweight1[3]; // editweight function에서 graph 내 source/destination node 인접 노드 간에 끊긴 weight 정보를 가짐
int recoveryweight2[3];
int altrecoveryweight1[3];// editweight function에서 altgraph 내 source node 인접 노드 간에 끊긴 weight 정보를 가짐
int path[V], inversepath[V], prevnode[V];
int best[V];
int start = 0;
int size, head, tail; // instructionset
int altsize, althead, alttail; // altinstructionset
struct inst* instructionset;
struct inst* altinstructionset; // altpath에 대한 instruction. 차가 maxnode까지 도달하면 instructionset으로 대입

int uturn_x;
int uturn_y;
int interruptatnode = 0;
int whatinterruptnode;
int altassign = 0;
bool sendUturn;
//struct car_info carlist[50];
int continueUturn = 0;
int length = 0, altlength = 0;
int maxnode = 99;
int altpath[V];
int altgraph[V][V];
int interruptedNode;
int u, v, j;

int coord[V][2] =
{ // 42 X 24
   { 0, 0 },// startvertex
   { 0, 0 },  // 0 // x, y
   { 14, 0 },  // 1
   { 28, 0 }, // 2
   { 42, 0 }, // 3
   { 0, 14 },  // 4
   { 14, 14 },  // 5
   { 28, 14 }, // 6
   { 0, 28 }, // 7
   { 14, 28 }, // 8
   { 28, 28 },// 9
   { 42, 28 },// 10
   { 0,0 } // end vertex
};

int graph[V][V] =
{ //  s  1  2  3  4  5  6  7  8  9  10 11, e
   { 0, 0, 0, 0, 0, 0, 0, 0, 0 ,0, 0, 0, 0 },
   { 0, 0, 5, 0, 0, 2, 0, 0, 0, 0, 0, 0, 0 }, // 1
   { 0, 5, 0, 4, 0, 0, 5, 0, 0, 0, 0, 0, 0 }, // 2
   { 0, 0, 4, 0, 7, 0, 0,12, 0, 0, 0, 0, 0 }, // 3
   { 0, 0, 0, 7, 0, 0, 0, 0, 0, 0, 0,10, 0 }, // 4
   { 0, 2, 0, 0, 0, 0,10, 0, 7, 0, 0, 0, 0 }, // 5
   { 0, 0, 5, 0, 0,10, 0, 5, 0,10, 0, 0, 0 }, // 6
   { 0, 0, 0,12, 0, 0, 5, 0, 0, 0, 3, 0, 0 }, // 7
   { 0, 0, 0, 0, 0, 7, 0, 0, 0, 7, 0, 0, 0 }, // 8
   { 0, 0, 0, 0, 0, 0,10, 0, 7, 0, 9, 0, 0 }, // 9
   { 0, 0, 0, 0, 0, 0, 0, 3, 0, 9, 0, 4, 0 }, // 10
   { 0, 0, 0, 0,10, 0, 0, 0, 0, 0, 4, 0, 0 }, // 11
   { 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0 }, // e 
};


int closenode(int x, int y) {
   int nextnearnode = 0;
   int node = 0, i;
   double min = 99999.999;
   double distance[V];
   for (i = 0; i < V; i++) {
      distance[i] = sqrt(pow((double)(x - coord[i][0]), 2) + pow((double)(y - coord[i][1]), 2));
   }
   for (i = 1; i < V - 1; i++) {
      if (min > distance[i]) {
         min = distance[i];
         nextnearnode = node;
         node = i;
      }
   }
   return node;
}

double distance(int x1, int y1, int x2, int y2) {
   return sqrt(pow((double)(x1 - x2), 2) + pow((double)(y1 - y2), 2));
}

int mindistance(int best[], int sptSet[])
{
   double min = 99999;
   int min_index;

   for (int v = 0; v < V; v++)
      if (sptSet[v] == 0 && best[v] <= min)
         min = best[v], min_index = v;

   return min_index;
}

//Queue Functions

void initQ(int _size, int altchk) {
   if (altchk == 0) {
      size = _size;
      instructionset = (struct inst*)malloc(sizeof(struct inst)*size);
      head = 0; tail = 0;
   }
   else { // altchk = 1 --> altinstructionset
      altsize = _size;
      altinstructionset = (struct inst*)malloc(sizeof(struct inst)*altsize);
      althead = 0; alttail = 0;
   }
}

void insertQ(struct inst data, int altchk) {
   if (altchk == 0) {
      instructionset[tail] = data;
      tail++;
      //      printf("insert is finished successfully, posx : %d, posy : %d, inst : %c\n", data.pos_x, data.pos_y, data.instruction);
   }
   else {
      altinstructionset[alttail] = data;
      alttail++;
      //      printf("insert is finished successfully, posx : %d, posy : %d, inst : %c\n", data.pos_x, data.pos_y, data.instruction);
   }
}

struct inst delQ(int altchk) {
   struct inst temp;
   if (altchk == 0) {
      if (head == tail) {
         temp.instruction = 'Z';
         temp.pos_x = '-1';
         temp.pos_y = '-1';
      }
      else {
         temp = instructionset[head];
         head++;
      }
   }
   else {
      if (althead == alttail) {
         temp.instruction = 'Z';
         temp.pos_x = '-1';
         temp.pos_y = '-1';
      }
      else {
         temp = altinstructionset[althead];
         althead++;
      }
   }
   return temp;
}



//Adding instructions and coordinations of nodes
void addInstructions(int path[], int altchk, int sendUturn) {
   struct inst tempinst;

   int prev[V][2] = { 0,0 }, curr[V][2] = { 0,0 }, next[V][2] = { 0,0 };
   initQ(V, altchk);

   for (int x = 0; x < V; x++) {
      //printf("\nfor starts");
      if (path[x] == 0) {
         if (sendUturn == true && altchk == 1) {
            tempinst.instruction = 'U';
            tempinst.pos_x = coord[path[x]][0];
            tempinst.pos_y = coord[path[x]][1];
         }
         else {
            tempinst.instruction = 'F';
            tempinst.pos_x = coord[path[x]][0];
            tempinst.pos_y = coord[path[x]][1];
         }
      }
      else if (path[x + 1] == -1) {
         tempinst.instruction = 'S';
         tempinst.pos_x = coord[path[x]][0];
         tempinst.pos_y = coord[path[x]][1];
      }

      else {
         curr[x][0] = coord[path[x]][0];
         curr[x][1] = coord[path[x]][1];
         prev[x][0] = coord[path[x - 1]][0];
         prev[x][1] = coord[path[x - 1]][1];
         next[x][0] = coord[path[x + 1]][0];
         next[x][1] = coord[path[x + 1]][1];
         //         printf("coord has been inserted\n");


         if (prev[x][0] == next[x][0] || prev[x][1] == next[x][1]) {
            tempinst.instruction = 'F';
         }
         //prev x > curr x   
         else if (prev[x][0] > curr[x][0]) {
            if (curr[x][1] > next[x][1])
               tempinst.instruction = 'L';
            else tempinst.instruction = 'R';
         }
         // prev x < curr x
         else if (prev[x][0] < curr[x][0]) {
            if (curr[x][1] > next[x][1])
               tempinst.instruction = 'R';
            else tempinst.instruction = 'L';
         }

         // prev y> curr y
         else if (prev[x][1] > curr[x][1]) {
            if (curr[x][0] > next[x][0])
               tempinst.instruction = 'R';
            else tempinst.instruction = 'L';
         }
         // prev y< curr y
         else if (prev[x][1] < curr[x][1]) {
            if (curr[x][0] > next[x][0])
               tempinst.instruction = 'L';
            else tempinst.instruction = 'R';
         }
         else tempinst.instruction = 'F';

         tempinst.pos_x = curr[x][0];
         tempinst.pos_y = curr[x][1];
      }
      printf("%d -th instruction : posx : %d, posy : %d, inst : %c\n", x, tempinst.pos_x, tempinst.pos_y, tempinst.instruction);
      insertQ(tempinst, altchk);
      if (path[x + 1] == -1)
         break;
   }
   //   getchar();
}


bool interruption(int carloc[], int interruptloc[]) {
   struct inst temp;

   int car_next; // 차가 다음에 지나야 할 노드
   int car_prev; // 차가 직전에 지난 노드
   int car_nextorder, car_prevorder; // next or prev 노드의 path array에서의 순서

   int inter_next1, inter_next2; // interruption 인접 노드 2개. 순서상관 x
   int i = 0;
   temp = instructionset[head]; // 다음에 지나야 할 노드의 정보를 담고있는 instructionset
   for (car_next = 0; car_next < V; car_next++) {
      if ((temp.pos_x == coord[car_next][0]) && temp.pos_y == coord[car_next][1]) { // car_next를 정하는 for 문
                                                                 //         printf("the initial car next order is %d\n", car_next);
         break;
      }
   }
   car_prev = prevnode[car_next]; // 당연히 carnext의 이전 노드

   inter_next1 = closenode(interruptloc[0], interruptloc[1]); // interrupt와 가까운 노드중 하나. closenode를 통해 계산
   if (coord[inter_next1][0] == interruptloc[0] && coord[inter_next1][1] == interruptloc[1]) {
      interruptatnode = 1;
      printf("the interruption is occured ON the node %d!\n", inter_next1);
   }
   else {
      for (inter_next2 = V - 1; inter_next2 >= 0; inter_next2--) { // interrupt 인접 노 다른 하나를 구함. graph matrix를 통해 인접노드 중 0이아닌 노드에 대해서 검사.
         if (graph[inter_next1][inter_next2] != 0) {
            if (coord[inter_next2][1] == coord[inter_next1][1]) {//두 노드가 y좌표가 같은 경우
               if ((coord[inter_next2][0] - interruptloc[0])*(coord[inter_next1][0] - interruptloc[0]) < 0) { //두 노드가x축에서 interruptloc[]을 감싸고 있다면 break;
                  printf("we found an interruption! same y\n");
                  printf("the interrupted node is %d and %d\n", inter_next1, inter_next2);
                  break;
               }
            }
            else if (coord[inter_next2][0] == coord[inter_next1][0]) { // 두 노드가 x좌표가 같은 경우
               if ((coord[inter_next2][1] - interruptloc[1])*(coord[inter_next1][1] - interruptloc[1]) < 0) { // 두 노드가 y 축에서 interruptloc을 감싸고 있다면 break/
                  printf("we found an interruption! same x\n");
                  printf("the interrupted node is %d and %d\n", inter_next1, inter_next2);
                  break;
               }
            }
         }
      }
   }

   for (i = 0; i < length; i++) {// car nextorder 계산.
      if (path[i] == car_next) {
         break;
      }
   }
   car_nextorder = i;
   car_prevorder = car_nextorder - 1; // 당연
                              //   printf("the next node the car has to go is %d\n", path[car_nextorder]);

   if (interruptatnode != 0) {
      for (i = 0; i < length - 1; i++) {
         if (path[i] == inter_next1) {//the interruption is in our path
            printf("interruption is in our calculated path!\n");
            if (car_nextorder <= i) {// the car hasn't passed the interrupt node yet
               printf("the interruption ON the node! is in upcoming path\n");
               whatinterruptnode = inter_next1;
               return true;
            }
            else {// the car has passed the node
               printf("we have passed the interruption ON the node!\n");
               return false;
            }
         }
      }
      printf("the interruption ON the node is not in our path! \n");
      return false;
   }

   else {
      for (i = 0; i < length - 1; i++) {
         //         printf("length is : %d", length);
         if ((path[i] == inter_next1 && path[i + 1] == inter_next2) || (path[i] == inter_next2 && path[i + 1] == inter_next1)) { // 경로 내에 interrupt가 있는 경우. 이 때 path[i]는 interrupt인접노드중 앞쪽, path[i+1]은 뒷
            printf("interruption is in our calculated path!\n");
            if (car_nextorder <= i) { // car이 아직 그 interrupt가 있는 edge를 지나지 않은 경우. return true.
               interruptedNode = path[i + 1];
               printf("interruption in upcoming path!\n interrupted node is : %d \n", i);
               return true;
            }
            else if (car_prevorder >= i + 1) { // car 이미 그 interrupt edge를 지난 경우. interrupt가 있어도 신경안씀 return false.
               printf("interruption not in upcoming path!\n");
               return false;
            }
            else if (car_prevorder == path[i] && car_nextorder == path[i + 1]) { // car이 interrupt edge에 있는 경우. path[i+1]과의 distance를 계산하여 car이 더 멀리있다면 아직 interrupt를안지낫으므로 return true.
               if (distance(coord[path[i + 1]][0], coord[path[i + 1]][1], carloc[0], carloc[1]) > distance(coord[path[i + 1]][0], coord[path[i + 1]][1], interruptloc[0], interruptloc[1])) {
                  printf("we found an interruption in current path!\n");
                  return true;
               }

               else {
                  printf("we found an interruption in current path but we passed it!\n");
                  return false;
               }
            }
         }

      }
      printf("the interruption is not in our path!\n");
      return false;
   }
}


void makeNewGraph(int carloc[2], int graph[][V], int path[]) {
   for (u = 0; u < V; u++) {
      for (v = 0; v < V; v++) {
         altgraph[u][v] = graph[u][v];
      }
   }

   altgraph[recoveryweight1[0]][recoveryweight1[1]] = recoveryweight1[2];
   altgraph[recoveryweight1[1]][recoveryweight1[0]] = recoveryweight1[2];
   altgraph[0][recoveryweight1[0]] = 0;
   altgraph[0][recoveryweight1[1]] = 0;
   altgraph[recoveryweight1[0]][0] = 0;
   altgraph[recoveryweight1[1]][0] = 0;

   altgraph[recoveryweight2[0]][recoveryweight2[1]] = recoveryweight2[2];
   altgraph[recoveryweight2[1]][recoveryweight2[0]] = recoveryweight2[2];
   altgraph[12][recoveryweight2[0]] = 0;
   altgraph[12][recoveryweight2[1]] = 0;
   altgraph[recoveryweight2[0]][12] = 0;
   altgraph[recoveryweight2[1]][12] = 0;
   // dijkstra에서 graph에 source / destination 에서 끊고(editweight) 연결해준 weight 전부 복구



   if (interruptatnode == 1) {      //when there is an interruption AT a node, not in bewteen nodes
      interruptrecovery[0][2] = 1;//interrupt recovery has been inserted
                           //      printf("interrupt recovery has been inserted ! : %d\n", interruptrecovery[0][2]);
      for (int j = 0; j < V; j++) {
         if (altgraph[j][whatinterruptnode] != 0) {
            interruptrecovery[j][0] = whatinterruptnode;
            interruptrecovery[j][1] = altgraph[whatinterruptnode][j];
            //            printf("interrupt recovery saved : %d and %d had %d\n", interruptrecovery[j][0], j, interruptrecovery[j][1]);
         }
         altgraph[j][whatinterruptnode] = 0;// whatinterruptnode: the fully interrupted node
         altgraph[whatinterruptnode][j] = 0;
      }
      altgraph[recoveryweight1[0]][recoveryweight1[1]] = recoveryweight1[2];
      altgraph[recoveryweight1[1]][recoveryweight1[0]] = recoveryweight1[2];

      altgraph[recoveryweight2[0]][recoveryweight2[1]] = recoveryweight2[2];
      altgraph[recoveryweight2[1]][recoveryweight2[0]] = recoveryweight2[2];
      for (int j = 0; j < V; j++) {
         //         printf("\n disconnected the connection with interrupted node: %d and %d has cost %d \n", whatinterruptnode, j, altgraph[whatinterruptnode][j]);
      }

   }
   else {
      altgraph[interruptedNode][prevnode[interruptedNode]] = 0; // interruptedNode : 직전에 발생한 interruption fn에서 값 정해짐
      altgraph[prevnode[interruptedNode]][interruptedNode] = 0;
      // interruption 인접 두 노드간 weight 0으로 설정
      //      printf("\n disconnected the interrupted nodes: %d and %d has cost %d \n", interruptedNode, prevnode[interruptedNode], altgraph[interruptedNode][prevnode[interruptedNode]]);

   }

   for (int i = 0; i < 2; i++) {
      coord[0][i] = carloc[i];
   } // 시작점 현재 차의 위치로 재설정


   dijkstra(altgraph, altpath, &altlength, 1); // graph에 끊어진 부분 복구 후 interruption 부분 끊고, 시작점을 재설정한 뒤 dijkstra.
                                    //thorugh this dijkstra,
                                    //altgraph : source, destination 인접 노드간 weight 수정
                                    //altpath : altgraph를 통해 set, altlength : altpath의 length
                                    //altinstructionset : Queue에 altpath에 의한 instruction 입력
                                    //sour
   for (j = 0; j < sizeof(path) / sizeof(int); j++) {
      if (altpath[j] == path[j])
         maxnode = path[j]; // maxnode : 가장 끝까지 겹치는 노드
      else
         break;
   }
   //   printf("\n maxnode is %d ", maxnode);
   altassign = 0;
}


char Sendinstruction(int location[], int location2[], int path[]) { // location : car's location, location2 : interruption's location.
   char sendinstruction;
   struct inst temp;
   struct inst tempnext;



   if (interruption(location, location2) == true) {
      for (int i = 0; i < V; i++) {
         graph[0][i] = 0;
         graph[i][12] = 0;
      }
      if (interruptrecovery[0][2] == 1) {
         for (int j = 0; j < V; j++) {
            graph[interruptrecovery[j][0]][j] = interruptrecovery[j][1];
            graph[j][interruptrecovery[j][0]] = interruptrecovery[j][1];
            //            printf("re-inserted the lost weights due to interruption ON node: %d and %d has %d now\n", interruptrecovery[j][0], j, interruptrecovery[j][1]);
         }
         interruptrecovery[0][2] = 0;
      }

      printf("there is an interruption! calculated the max common node of the alternative graph \n");
      makeNewGraph(location, graph, path);
      whatinterruptnode = 0;
   }


   if (start == 0) {
      temp = delQ(0);
      if (temp.instruction == 'U') {
         continueUturn = 1;
         start = 1;
         uturn_x = temp.pos_x;
         uturn_y = temp.pos_y;
         return temp.instruction;
      }
      else {
         start = 1;
      }
   }
   //   printf("continueUturn : %d / start : %d \n", continueUturn, start);
   //   printf("uturn_x: %d, uturn_y: %d \n", uturn_x, uturn_y);

   if (continueUturn == 1) {
      if ((location[0] != uturn_x) || (location[1] != uturn_y)) {
         printf("the car is not at its original position yet\n");
         sendinstruction = 'U';
         return sendinstruction;
      }
   }

   continueUturn = 0;

   temp = instructionset[head];
   tempnext = instructionset[head + 1];
   if (temp.instruction == 'Z') {
      return;
   }



   if (maxnode == 0 && altassign == 0) {

      altassign = 1;
      printf("\n constant interruption detected, changing path with new graph\n\n");
      for (u = 0; u < V; u++) {
         path[u] = altpath[u];
         for (v = 0; v < V; v++) {
            graph[u][v] = altgraph[u][v];
         }
      }
      initQ(V, 0);
      for (int a = althead; a < alttail; a++) {
         insertQ(delQ(1), 0); // -> 되는 것 확인완료
      }
      start = 0;
      recoveryweight1[0] = altrecoveryweight1[0];
      recoveryweight1[1] = altrecoveryweight1[1];
      recoveryweight1[2] = altrecoveryweight1[2];
   }

   //차가 maxnode까지 갈 때까지 interruption이 사라지지 않았을 때. altgraph, altpath, altinstruction, altrecoveryweight로 변경하고, start = 0으로 재설정
   else if ((coord[maxnode][0] == temp.pos_x && coord[maxnode][1] == temp.pos_y) && (interruption(location, location2) == true) && altassign == 0) {
      altassign = 1;
      printf("\n constant interruption detected, changing path with new graph\n\n");
      for (u = 0; u < V; u++) {

         path[u] = altpath[u];
         for (v = 0; v < V; v++) {
            graph[u][v] = altgraph[u][v];
         }
      }
      initQ(V, 0);
      for (int a = althead; a < alttail; a++) {
         insertQ(delQ(1), 0); // -> 되는 것 확인완료
      }
      start = 0;
      recoveryweight1[0] = altrecoveryweight1[0];
      recoveryweight1[1] = altrecoveryweight1[1];
      recoveryweight1[2] = altrecoveryweight1[2];
      //instruction에 altinstructionset을 집어넣는 과정 필요    

   }





   if (distance(location[0], location[1], temp.pos_x, temp.pos_y) < ROTATIONDISTANCE) {
      if (tempnext.instruction == 'S' && distance(location[0], location[1], tempnext.pos_x, tempnext.pos_y) <= HALTDISTANCE) {
         sendinstruction = 'H'; //Crash Halt
      }
      else {
         sendinstruction = temp.instruction;
      }
   }
   else if (distance(location[0], location[1], tempnext.pos_x, tempnext.pos_y) <= ROTATIONDISTANCE) {
      sendinstruction = tempnext.instruction;
      temp = delQ(0);
      //      printf("deleting queue is : %c (%d,%d)\n", temp.instruction, temp.pos_x, temp.pos_y);
   }
   else {
      printf("not in rotation distance\n");
      sendinstruction = 'F';
   }

   printf("%c at %d, %d \n", sendinstruction, temp.pos_x, temp.pos_y);
   return sendinstruction;

}


void editweight(int editgraph[][V], int nearestnode, int source_or_destination, int altchk) { // editgraph에서 source or destination 인접 두 노드간 weight관계 setting

                                                                       //   printf("\n\n start edit weight \n\n");
   int inter_next1, inter_next2;
   double distanceratio;
   int editedweight, nextnearnode_edit = 0;

   inter_next1 = nearestnode; // startV or endV
   for (inter_next2 = V - 1; inter_next2 >= 0; inter_next2--) { //startV또는 endV의 인접노드를 구함. graph matrix를 통해 인접노드 중 0이아닌 노드에 대해서 검사.
      if (editgraph[inter_next1][inter_next2] != 0) {
         //         printf("%d and %d has connected\n", inter_next1, inter_next2);
         if (coord[inter_next2][1] == coord[inter_next1][1]) {//두 노드가 y좌표가 같은 경우
                                                 //            printf("%d and %d is same y\n", inter_next2, inter_next1);
            if ((coord[inter_next2][0] - coord[source_or_destination][0])*(coord[inter_next1][0] - coord[source_or_destination][0]) < 0) { //두 노드가x축에서 source/destination node을 감싸고 있다면 break;
                                                                                                            //               printf("the two nodes adjacent to %d is %d and %d\n", source_or_destination, inter_next1, inter_next2);
               break;
            }
         }
         else if (coord[inter_next2][0] == coord[inter_next1][0]) { // 두 노드가 x좌표가 같은 경우
                                                      //            printf("%d and %d is same x\n", inter_next2, inter_next1);
            if ((coord[inter_next2][1] - coord[source_or_destination][1])*(coord[inter_next1][1] - coord[source_or_destination][1]) < 0) { // 두 노드가 y 축에서 source/destination node을 감싸고 있다면 break/
                                                                                                            //               printf("the two nodes adjacent to %d is %d and %d\n", source_or_destination, inter_next1, inter_next2);
               break;
            }
         }
         else
            break;
      }
   } // inter_next1, 2를 잡아줌.

   distanceratio = distance(coord[inter_next1][0], coord[inter_next1][1], coord[source_or_destination][0], coord[source_or_destination][1]) / distance(coord[inter_next1][0], coord[inter_next1][1], coord[inter_next2][0], coord[inter_next2][1]);
   // distance ratio : (internext1과 source/destination 거리) / (internext1, 2사이 거리)
   //   printf("\n distance ratio: %f \n", distanceratio);
   editedweight = (int)(editgraph[inter_next1][inter_next2] * distanceratio);
   if (editedweight == 0) editedweight = 1;
   //   printf("edited weight: %d \n", editedweight);
   nextnearnode_edit = editgraph[inter_next1][inter_next2] - editedweight;

   editgraph[source_or_destination][inter_next1] = editedweight;// source와 그 인접노드 간 weight
   editgraph[inter_next1][source_or_destination] = editedweight;

   editgraph[source_or_destination][inter_next2] = nextnearnode_edit;// source와 그 다음 인접노드 간 weight
   editgraph[inter_next2][source_or_destination] = nextnearnode_edit;
   // source와 인접노드간 weight 설정 완료.

   if (altchk == 0 && source_or_destination == 0) { // Graph에 대한 editweight
      recoveryweight1[0] = inter_next1;
      recoveryweight1[1] = inter_next2;
      recoveryweight1[2] = editgraph[inter_next1][inter_next2];
      //      printf("\n recovery weight 1 : %d %d  --> %d\n", recoveryweight1[0], recoveryweight1[1], recoveryweight1[2]);
   }
   else if (altchk == 1 && source_or_destination == 0) { // altGraph에 대한 editweight -> destination은 변함 없고, source만 현재 차의 위치로 변경됨.
      altrecoveryweight1[0] = inter_next1;
      altrecoveryweight1[1] = inter_next2;
      altrecoveryweight1[2] = editgraph[inter_next1][inter_next2];
      //      printf("\n altrecovery weight 1 : %d %d  --> %d\n", altrecoveryweight1[0], altrecoveryweight1[1], altrecoveryweight1[2]);
   }


   if (source_or_destination == 12) {
      recoveryweight2[0] = inter_next1;
      recoveryweight2[1] = inter_next2;
      recoveryweight2[2] = editgraph[inter_next1][inter_next2];
      //      printf("\n recovery weight 2 : %d %d  --> %d\n", recoveryweight2[0], recoveryweight2[1], recoveryweight2[2]);
   }

   editgraph[inter_next1][inter_next2] = 0;
   editgraph[inter_next2][inter_next1] = 0;
   if (inter_next1 == whatinterruptnode) {
      editgraph[inter_next1][source_or_destination] = 0;
      editgraph[source_or_destination][inter_next1] = 0;
   }
   // 인접노드간 weight 0으로 설정 완료.
   return;
}

bool checkUturn(int path[], int altpath[]) {
   if (path[1] != altpath[1])
      return true;
   else
      return false;
}

void dijkstra(int editgraph[][V], int* editpath, int* editlength, int altchk) { // chkalt : altgraph에 의한 dijkstra인지 chk. chk ==1 이면 altgraph
   int  i, sptSet[V], j = 0;
   int startV, endV; // source or destination 좌표에 가장 인접한 vertex.
                 // sptSet[i] will true if vertex i is included in shortest
                 // path tree or shortest distance from src to i is finalized                    


   startV = closenode(coord[0][0], coord[0][1]);
   endV = closenode(coord[12][0], coord[12][1]);
   printf("\nStartV : %d // EndV : %d\n", startV, endV);

   editweight(editgraph, startV, 0, altchk); //editing the weight of adjacent nodes of the source node  [ㅁ --5-- S -3- ㅁ], weight between ㅁ : 0.
   editweight(editgraph, endV, 12, altchk);  //editing the weight of adjacent nodes of the destination node   
                                   /*   if (startV == whatinterruptnode) {
                                   startV =
                                   }*/
   for (i = 0; i < V; i++) // initial setting.
      best[i] = 99999, sptSet[i] = 0, inversepath[i] = -1, editpath[i] = -1;

   best[0] = 0; // distance from 0 to 0 : 0
   prevnode[0] = -1; // 0 is source

   for (i = 0; i < V; i++) {
      for (int j = 0; j < V; j++) {
         printf("%2d  ", editgraph[i][j]);  // printing editgraph
      }
      printf("\n");
   }

   for (int count = 0; count < V - 1; count++) // dijkstra calculating part
   {
      int u = mindistance(best, sptSet);
      sptSet[u] = 1;

      for (int v = 0; v < V; v++) {
         if (!sptSet[v] && editgraph[u][v] && best[u] + editgraph[u][v] < best[v]) {
            best[v] = best[u] + editgraph[u][v];
            prevnode[v] = u;
         }
      }
   }
   // print the constructed distance array
   printf("Vertex Distance from Source\n");
   for (int i = 0; i < V; i++)
      printf("%d \t\t %d\n", i, best[i]);

   inversepath[j] = 12;

   while (1) {
      if (prevnode[inversepath[j]] == -1 && inversepath[j] != 12) {
         j++;
         break;
      }
      inversepath[j + 1] = prevnode[inversepath[j]];
      j++;
   } // j : length of path.

   *editlength = j;
   for (i = 0; i < *editlength; i++) {
      editpath[i] = inversepath[*editlength - i - 1];
   }// dijkstra를 모두 돌리고 editpath에 입력 완료. editpath의 length는 editlength에 저장. editpath[0] ~ editpath[length-1]

    //source로 설정한 위치가 graph상 important node에 있을 경우 0->2->3->12라면 0->3->12로 바꿔줌.
   if ((coord[editpath[0]][0] == coord[editpath[1]][0]) && (coord[editpath[0]][1] == coord[editpath[1]][1])) {
      for (i = 1; i < *editlength; i++) {
         editpath[i] = editpath[i + 1];
      }
      best[12]--;
      *editlength--;
   }
   //destination으로 설정한 위치가 graph상 important node에 있을 경우. 0->2->3->4->12라면 0->2->3->12로 바꿔줌.(12가 4의 위치일때)
   if ((coord[12][0] == coord[prevnode[12]][0]) && (coord[12][1] == coord[prevnode[12]][1])) {
      editpath[*editlength - 2] = editpath[*editlength - 1];
      editpath[*editlength - 1] = -1;
      *editlength--;
      best[12]--;
   }

   for (i = 0; i < *editlength; i++) { // print out
      if (i == *editlength - 1)
         printf("%d\n", editpath[i]);
      else printf("%d->", editpath[i]);
   }
   //   printf("dijkstra is fine\n");
   sendUturn = checkUturn(path, altpath);
   addInstructions(editpath, altchk, sendUturn); // altchk에 따라서 instructionset or altinstructionset에 insertQ 실행
   sendUturn = false;
}

struct car_info FindNearestDijkstra(int carselected, int limit, char serial) {
   struct car_info data;
   while (carselected == 0) {
      if (limit * 10 > best[12]) {
         data.serial = serial;
         data.cost = best[12];
      }
      else
         data.serial = 'Z';
   }
   return data;
}


