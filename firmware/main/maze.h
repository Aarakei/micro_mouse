#include <stdio.h>
#include <stdbool.h>
bool finish = 0;

int maze[16][16] ={ // Remember that because of how arrays work, the first index is the row and the second index is the column, so it's (y, x) not (x, y). 
//   0, 1, 2, 3, 4, 5, 6, 7, 8, 9, 10,11,12,13,14,15
    {0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0}, //0
    {0, 1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0}, //1
    {0, 0, 1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0}, //2
    {0, 0, 0, 1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0}, //3
    {0, 0, 0, 0, 1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0}, //4
    {0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0}, //5
    {0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0}, //6
    {0, 0, 0, 0, 0, 0, 0, 2, 2, 0, 0, 0, 0, 0, 0, 0}, //7
    {0, 0, 0, 0, 0, 0, 0, 2, 2, 0, 0, 0, 0, 0, 0, 0}, //8
    {0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0}, //9
    {0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0}, //10
    {0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0}, //11
    {0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0}, //12
    {0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0}, //13
    {0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0}, //14
    {0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0}  //15
};


int memory[16][16] ={ /*This is the robots memory. It is also a 16x16 grid (duh) and 
    it will fill in with ones to match the maze as it goes through using the see() function.*/ 
//   0, 1, 2, 3, 4, 5, 6, 7, 8, 9, 10,11,12,13,14,15
    {0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0}, //0
    {0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0}, //1
    {0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0}, //2
    {0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0}, //3
    {0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0}, //4
    {0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0}, //5
    {0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0}, //6
    {0, 0, 0, 0, 0, 0, 0, 2, 2, 0, 0, 0, 0, 0, 0, 0}, //7
    {0, 0, 0, 0, 0, 0, 0, 2, 2, 0, 0, 0, 0, 0, 0, 0}, //8
    {0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0}, //9
    {0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0}, //10
    {0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0}, //11
    {0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0}, //12
    {0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0}, //13
    {0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0}, //14
    {0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0}  //15
};


// Directions
int current_direction = 0; // Start facing North
int i_direction = 0;
    // 0:North
    // 1:East
    // 2:South
    // 3:West

// Locations
int location[2] = {
    0, // Row
    0  // Column
};
int i_location[2]; /*Declare an imaginary location for an imaginary robot.
Why? Because this imaginary robot will solve the memory maze, charting a path for the real robot.*/

// Targets
int target[2];
int i_target[2]; // Imaginary target for the imaginary robot.

// Distancess
int distance[2]; // Rirst index is row, second it column, so it's (y, x) not (x, y).
int i_distance[2];


// Functions
void move(int distance){ //This function takes a distance and moves the robot forward that distance.
    if (current_direction == 0) { // North
        for (int i = 0; i < distance; i++) {
            location[0]--; // Subtract from the row subsection of location, moving it up the maze.
        }
    }
    else if (current_direction == 1) { // East
        for (int i = 0; i < distance; i++) {
            location[1]++; // Add to the column subsection of location, moving it right in the maze.
        }
    }
    else if (current_direction == 2) { // South
        for (int i = 0; i < distance; i++) {
            location[0]++; // Add to the row subsection of location, moving it down the maze.
        }
    }
    else if (current_direction == 3) { // West
        for (int i = 0; i < distance; i++) {
            location[1]--; // Subtract from the column subsection of location, moving it left in the maze.
        }
    }
};

void overshoot(){ // This function will probably be unneccesary with the final design, but for now it ensures that your direction is always 0-3. I.E 4 is North, 5 is East, etc. 
    if (current_direction > 3) {
        current_direction = current_direction % 4;
    }
};

void see() /* This function updates memory with the 3 spaces in front of the robot, and
the space the robot is on. 
This function will likely not be in the robot in its current form. */ 
{
    if (current_direction == 0) { // North
        memory[location[0]-1][location[1]-1] = maze[location[0]-1][location[1]-1];
        memory[location[0]-1][location[1]] = maze[location[0]-1][location[1]];
        memory[location[0]-1][location[1]+1] = maze[location[0]-1][location[1]+1];
    }
    else if (current_direction == 1){ // East
        memory[location[0]-1][location[1]+1] = maze[location[0]-1][location[1]+1];
        memory[location[0]][location[1]+1] = maze[location[0]][location[1]+1];
        memory[location[0]+1][location[1]+1] = maze[location[0]+1][location[1]+1];
    }
    else if (current_direction == 2){ // South
        memory[location[0]+1][location[1]-1] = maze[location[0]+1][location[1]-1];
        memory[location[0]+1][location[1]] = maze[location[0]+1][location[1]];
        memory[location[0]+1][location[1]+1] = maze[location[0]+1][location[1]+1];
    }
    else if (current_direction == 3){ // West
        memory[location[0]-1][location[1]-1] = maze[location[0]-1][location[1]-1];
        memory[location[0]][location[1]-1] = maze[location[0]][location[1]-1];
        memory[location[0]+1][location[1]-1] = maze[location[0]+1][location[1]-1];
    }
    memory[location[0]][location[1]] = maze[location[0]][location[1]];
}

void get_quadrant(int coords[2], int ending[2]) // This function determines the quadrant of the robot. 
{
    // These if statements determine which quadrant we are in, and then determines the correct ending. 
    if (coords[0] <= 7 && coords[1] >= 8) { //Then we are in Quadrant 1
        ending[0] = 7;
        ending[1] = 8;
    }
    else if (coords[0] <= 7 && coords[1] <= 7){ //Then we are in Quadrant 2
        ending[0] = 7;
        ending[1] = 7;
    }
    else if (coords[0] >= 8 && coords[1] <= 7) { //Then we are in Quadrant 3
        ending[0] = 8;
        ending[1] = 7  ;
    }
    else if (coords[0] >= 8 && coords[1] >= 8) { //Then we are in Quadrant 4
        ending[0] = 8;
        ending[1] = 8;
    }
}

void get_distance(int distance, int location, int ending) // This function calculates the distance from the robot to the target, and stores it in the distance array. 
{
        distance[0] = location[0] - ending[0]; 
        distance[1] = location[1] - ending[1];
}

void travel(int distance, int direction){
        /* In the current model, I am not using the pythagorean theorem to optimize travel distance
    this is because in incremental steps like this current model is, the pythagorean theorem does 
    not reduce the number of steps. */

        if (distance[0] < 0 && distance[1] > 0){ //Quadrant 1
            direction = 3; // West
            move(1);
            see();
            direction = 2; // South
            move(1);
            see();
        }
        else if (distance[0] < 0 && distance[1] == 0) { // Quadrant 1 column 8
            direction = 2; // South
            move(1);
            see();
        }
        else if (distance[0] == 0 && distance[1] > 0) { // Quadrant 1 row 7
            direction = 3; // West
            move(1);
            see();
        }
        else if (distance[0] < 0 && distance[1] < 0){ //Quadrant 2
            direction = 1; // East
            move(1);
            see();
            direction = 2; // South
            move(1);
            see();
        }
        else if (distance[0] == 0 && distance[1] < 0) { // Quadrant 2 column 7
            direction = 1; // East
            move(1);
            see();
        }
        else if (distance[0] < 0 && distance[1] == 0) { // Quadrant 2 row 7
            direction = 2; // South
            move(1);
            see();
        }
        else if (distance[0] > 0 && distance[1] < 0) //Quadrant 3
        {
            direction = 1; // East
            move(1);
            see();
            direction = 0; // North
            move(1);
            see();
        }
        else if (distance[0] > 0 && distance[1] == 0) { // Quadrant 3 column 7
            direction = 0; // North
            move(1);
            see();
        }
        else if (distance[0] == 0 && distance[1] < 0) { // Quadrant 3 row 8
            direction = 1; // East
            move(1);
            see();
        }
        else if (distance[0] > 0 && distance[1] > 0) //Quadrant 4
        {
            direction = 3; // West
            move(1);
            see();
            direction = 0; // North
            move(1);
            see();
        }
        else if (distance[0] > 0 && distance[1] == 0) { // Quadrant 4 column 8
            direction = 0; // North 
            move(1);
            see();
        }
        else if (distance[0] == 0 && distance[1] > 0) { // Quadrant 4 row 8
            direction = 3; // West
            move(1);
            see();
        }
        else if (distance[0] == 0 && distance[1] == 0){
            finish = 1;
            printf("Finished :)\n");
            for (int i = 0; i < 16; i++){
                printf("{");
                for (int i2 = 0; i2 < 16; i2++){
                    printf("%d, ", memory[i][i2]);
                } 
                printf("}\n");
            }
        }
};

void optimistic_algorithm()
{
    see();
    get_quadrant(location, target);
    
    // Calculate Distance
    /* Example of how this works:
        Suppose we are at (0, 0), which is the top left quarter.    
        By this point in the code, we have already calculated that we are in Quadrant 2, 
        so the ending the robot will gun for is (7, 8). (Again, remember that X and Y are swapped)
        Now, as a result of the above code ^^ int distance[] is set to {-7, -7}.
        Because both numbers are negative, we know that we need to move south and east to get to the ending. 
        The robot will go one unit east, and then one unit south, until it hits a wall. 
    */
    printf("Current Location: (%d, %d)\n", location[0], location[1]);
    while (finish == 0){
        get_distance(distance, location, target);
        travel(distance, current_direction);
        if (finish == 0){
            break
        };
        printf("Current Location: (%d, %d)\n", location[0], location[1]);  
    }
}

// void pathfinder(){
//     int shortest_path = 256;
//     get_quadrant(i_location, i_target);
//     while (i_finish == 0){
//         for(int i; i < 4; i++){

//         }
//     }

// }