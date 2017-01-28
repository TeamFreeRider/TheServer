#pragma once
#pragma once
#include<stdio.h>
#include<math.h>
//#include<conio.h>
#include<stdlib.h>

#define min(X,Y) ((X) < (Y) ? (X) : (Y))  
#define max(X,Y) ((X) > (Y) ? (X) : (Y)) 
// 이거 이상하게 다시 stdlib에 있다고 뜸
typedef enum { false, true } bool;
#define INT_MAX 2147483647
#define V 13 // number of vertex
#define ROTATIONDISTANCE 5
#define HALTDISTANCE 2 // 급정거

extern int recoveryweight1[3];
extern int recoveryweight2[3];

extern int altrecoveryweight1[3];
// editweight를 통해 끊어진 edge에 대한 정보. recoveryweight[0], [1] : 두 노드, recoveryweight[2] : 두 노드 간 weight

extern int path[V], altpath[V], inversepath[V], prevnode[V];
extern int best[V];
extern int length, altlength;
extern int numofcar;
extern int interruptatnode;
extern int start; // sendinstruction할때 필요한 조건. delQ가 처음인지를 알 수 있음.
extern int altassign;
struct car_info {
   char serial;
   int cost;
};
struct inst {
   char instruction;
   int pos_x;
   int pos_y;
};

extern int coord[V][2];
// node number 설정 total 11개
// 8    9   10    11
// 5    6    7
// 1    2    3    4
extern int graph[V][V], altgraph[V][V];


int closenode(int x, int y);
double distance(int x1, int y1, int x2, int y2);
int mindistance(int best[], int sptSet[]);

extern int sendUturn;
extern int size, head, tail;
extern struct inst* instructionset;

extern int altsize, althead, alttail;
extern struct inst* altinstructionset;

void initQ(int _size, int altchk);
void insertQ(struct inst data, int altchk);
struct inst delQ(int altchk);


void addInstructions(int path[], int altchk, int sendUturn);

bool interruption(int carloc[], int interruptloc[]);

char Sendinstruction(int location[], int location2[], int path[]);

void editweight(int editgraph[][V], int nearestnode, int source_or_destination, int altchk);


void dijkstra(int editgraph[][V], int* editpath, int* editlength, int altchk);

void makeNewGraph(int carloc[2], int graph[][V], int path[]);

struct car_info FindNearestDijkstra(int carselected, int limit, char myserial);
