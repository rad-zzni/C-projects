# Overview

This program is a satirically political visual algorithm. The algorithm can be described as a robot who must use DFS to collect all 'markers' within a closed 2D map. The political satire part is in the robot being a watermelon and the markers looking like Israel flags. 

# Details and Features
I have added extra features to the original program. These extra features include command line arguments for customising certain parameters, like map-size, animation speed (wait time), number of markers on the map, spawn-point of robot, and the direction the robot initially faces.

# How to use

### running without command line arguments
To run the program **without** command line arguments, input in the terminal inside the directory the file is:

```
gcc -o waterMelonAlgo waterMelonAlgo.c cmdlinearguments.c graphics.c
./waterMelonAlgo | java -jar drawapp-4.5.jar
```
This should by default make a 15-by-15 tile map with circular-like obstacles around it; wait time of 40; 30 markers generated; robot spawn-point in the approximate middle; and facing north.


### running with command line arguments
To run the program **with** command line arguments and thus custom parameters, input something like:
```
gcc -o waterMelonAlgo waterMelonAlgo.c cmdlinearguments.c graphics.c
./waterMelonAlgo 30 0 40 10 10 e| java -jar drawapp-4.5.jar
```
- The 1st argument given is the number of tiles in a fixed window size
- The 2nd argument is the wait time (how fast the animation will run)
- The 3rd argument is the number of markers you want on the map
- The 4th and 5th are the column and row the robot spawns in, respectively
- The 6th argument is the robot's initial direction
Thus, for example the command line argument above makes the map 30-by-30, wait time 0 (maximum speed), 40 markers spawned, and spawn point of column 10 and row 10, facing east


**Recommended parameter boundaries**: Recommended grid sizes are 13-by-13 to 50-by-50, wait time of 40 for smaller maps and 0 for bigger maps, marker count of less than grid size * 6 and column/row of half the map size. If the robot is by accident placed outside the empty grids, the program will throw an error and let you know.