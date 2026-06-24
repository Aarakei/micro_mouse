#include <stdio.h>
#include <stdbool.h>
#include "maze.h"

bool done = false;
cell_t maze[16][16]; // Remember that because of how arrays work, the first index is the row and the second index is the column, so it's (y, x) not (x, y). 
/*This is the robots memory. It is also a 16x16 grid (duh) and 
    it will fill in with ones to match the maze as it goes through using the see() function.*/ 
int front = 0;
int rear = 0;
int path[256][2]; // Again (y, x)
int start = 0;
int end = 0;

void maze_init() {
    for (int i = 0; i < 16; i++) {
        for (int j = 0; j < 16; j++) {
            if(i == 4 && j > 2 ){
                maze[i][j].walls[0] = true; // North
                maze[i][j].walls[1] = false; // East
                maze[i][j].walls[2] = false; // South
                maze[i][j].walls[3] = false; // West
                maze[i][j].x = j;
                maze[i][j].y = i;
                maze[i][j].visited = false;
            }
            else if((13 > i) && (i > 4) && (j == 3)){
                maze[i][j].walls[0] = true; // North
                maze[i][j].walls[1] = false; // East
                maze[i][j].walls[2] = false; // South
                maze[i][j].walls[3] = true; // West
                maze[i][j].x = j;
                maze[i][j].y = i;
                maze[i][j].visited = false;
            }
            else{
                maze[i][j].walls[0] = false; // North
                maze[i][j].walls[1] = false; // East
                maze[i][j].walls[2] = false; // South
                maze[i][j].walls[3] = false; // West
                maze[i][j].x = j;
                maze[i][j].y = i;
                maze[i][j].visited = false;
            }
        }
    }
}

cell_t queue[QUEUE_SIZE]; // This is the queue for the BFS algorithm. Remember it is (y, x).

void enqueue(cell_t cell) {
    queue[rear] = cell;
    rear++;
}

void dequeue() {
    front++; 
}

void bfs_pathfind(int x, int y, int next_distance){ // This function finds the next step in the path going from the x y you give it to the origin (0, 0). 
    int dy[] = {-1, 0, 1, 0};
    int dx[] = {0, 1, 0, -1};
    while(next_distance > 0){
        for(int i = 0; i < 4; i++){
            if(maze[y + dy[i]][x + dx[i]].distance == next_distance - 1){
                x = x + dx[i];  // move to the next cell
                y = y + dy[i];
                path[start][0] = y;          
                path[start][1] = x;          
                next_distance = maze[y][x].distance;
                start++;
                break; 
            }
        }
    }
}

void bfs() {
    int step = 0;
    enqueue(maze[0][0]); // Origin
    maze[0][0].visited = true;
    int dy[] = {-1, 0, 1, 0};
    int dx[] = {0, 1, 0, -1};
    
    while(!done && rear != front) {
        cell_t current_cell = queue[front]; // Get the current cell
        dequeue(); // Dequeue the current cell
        int y = current_cell.y; // Current cells coordinates
        int x = current_cell.x;
        step++;
        
        // Check the four directions around the current cell
        for (int i = 0; i < 4; i++) {
            if((y + dy[i] >= 0) && (y + dy[i] < 16) && (x + dx[i] >= 0) && (x + dx[i] < 16) && !maze[y + dy[i]][x + dx[i]].visited && maze[y][x].walls[i] == false) {
                // If we have reached the ending
                if ((maze[y + dy[i]][x + dx[i]].x == 7 || maze[y + dy[i]][x + dx[i]].x == 8) && (maze[y + dy[i]][x + dx[i]].y == 7 || maze[y + dy[i]][x + dx[i]].y == 8)) {
                    path[end][0] = maze[y + dy[i]][x + dx[i]].y; // This is the final cell, so we add it as the end of the path. 
                    path[end][1] = maze[y + dy[i]][x + dx[i]].x;
                    maze[y + dy[i]][x + dx[i]].distance = current_cell.distance + 1;
                    int next_distance = maze[y+ dy[i]][x+ dx[i]].distance;
                    bfs_pathfind(maze[y + dy[i]][x + dx[i]].x, maze[y + dy[i]][x + dx[i]].y, next_distance); // Now that we have reached the center, we can plot the shortest path back to the origin. 
                    done = true;
                    break;
                }
                maze[y + dy[i]][x + dx[i]].visited = true;
                maze[y + dy[i]][x + dx[i]].distance = current_cell.distance + 1;        
                enqueue(maze[y + dy[i]][x + dx[i]]);
            
            }
        }
    }
   
}


// This code is an example of a main that can calculate the distance of every square in the maze, and the path from the origin. 
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
