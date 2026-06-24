#ifndef MAZE_H
#define MAZE_H

#include <stdio.h>
#include <stdbool.h>
#define QUEUE_SIZE 256

extern bool done;

typedef struct {
    bool walls[4];
    int x;
    int y;
    int distance; // This is for the BFS algorithm, to keep track of how far each cell is from the origin.
    bool visited; // This is for the BFS algorithm, to keep track of which cells have been visited.
} cell_t;


extern cell_t maze[16][16]; // Remember that because of how arrays work, the first index is the row and the second index is the column, so it's (y, x) not (x, y). 
/*This is the robots memory. It is also a 16x16 grid (duh) and 
    it will fill in with ones to match the maze as it goes through using the see() function.*/ 


void maze_init();

extern cell_t queue[QUEUE_SIZE]; // This is the queue for the BFS algorithm. Remember it is (y, x).

extern int front;
extern int rear;

void enqueue(cell_t cell);

void dequeue();

extern int path[256][2]; // Again (y, x)
extern int start;
extern int end;

void bfs_pathfind(int x, int y, int next_distance);

void bfs();


// int main(){
//     maze_init();
//     bfs();
//    for(int i = 0; i < 16; i++){
//        for(int j = 0; j < 16; j++){
//            printf("{%d}", maze[i][j].distance);
//        }
//        printf("\n");
//    }
   
//    return 0; 
// }

#endif