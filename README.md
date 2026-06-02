Starting Program Flowchart
https://lucid.app/lucidchart/6eb8c024-2453-4e61-b1ac-233efe1e1f8c/edit

RULES:
https://attend.ieee.org/r2sac-2020/wp-content/uploads/sites/175/2020/01/MicroMouse_Rules_2020.pdf

Mouse rules:
 - self-contained (no remote control)

Maze Specifications:
 - 16 x 16 grid of cells that are each 18cm x 18cm
 - Walls are 5cm high and 1.2cm thick (5 % tolerance)
 - Outer walls are guaranteed
 - Mouse starts at one corner surrounded by 3 walls, with the next cell being clockwise relative to the maze
 - Every lattice point (where 4 walls would intersect) is guaranteed to have at least 1 wall attached
 - There may be more than one path to the center
 - Mazes are designed such that wall-hugging will not reach the center

Competition Rules:
 - 10 minutes to map out the maze and go for as many runs as you can. This time is never paused.
 - Can't reprogram after the maze is revealed
 - Time starts when the mouse exits the first cell



Qualifications: Autonomously navigate and map the maze, zoom to center of maze



Mapping the maze:

observe - read from ir sensors, motor encoders, maybe from an IMU
orient  - calculate position in maze / maze structure
decide  - choose between movement options
act     - driving motors




Speedrunning the maze:

observe - read from ir sensors, motor encoders, maybe from an IMU
orient  - determine position in map, error correct with IR distances/motor encoders
decide  - determine when to switch to next motion
act     - determine motor speeds



Design considerations:
Critical operations (priority):
 1. Update Position
 2. Motor PIDs
 3. Read Sensors
