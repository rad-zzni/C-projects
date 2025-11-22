
#include <stdio.h>
#include <math.h>
#include "graphics.h"
#include <stdlib.h>
#include <time.h> 
#include <string.h>

// Includes map details/sizes
typedef struct map {
    int numberOfTiles;
    int waitTime;
    int mapSize;
    int tileSize;
    int **arrayena; // Arena array. Elements store different things based on number assigned from 0 to 3: empty, obstacles, markers, robot spawn-point
    int **visited; // Separate arena array. Tracks which tiles the robot has visited (0 = unvisited, 1 = visited)
} map;

// Includes robot and marker details
typedef struct aRobot {
    int markersNeeded;
    int markersCollected;
    
    int colOfRobot;
    int rowOfRobot;
    
    // The x and y coordinates of the corner of the column-th and row-th position of the robot.
    int xTile;
    int yTile;
    
    // x and y coordinates of corners of triangle
    int x[3];
    int y[3];

    // x and y coordinates of corners of triangle before rotation
    int xOriginalAngle[3];
    int yOriginalAngle[3];
    

    double angle;
    char direction;
} aRobot;




// Making the struct instances global for cmdLineArguments.c to access
map defaultMap = {
    .numberOfTiles = 20,
    .waitTime = 10,
    .mapSize = 900 // Also size of the window, in pixels
};
map *mapPtr = &defaultMap;

aRobot waterMelonBot = {
    .markersNeeded=30,
    .colOfRobot = 10,
    .rowOfRobot = 10,
    .markersCollected = 0,
    .angle = 0,
    .direction = 's'
};
aRobot *botPtr = &waterMelonBot;


void initArrays(map *mapPtr)
{
    mapPtr->arrayena = malloc(mapPtr->numberOfTiles * sizeof(int*)); 
    mapPtr->visited = malloc(mapPtr->numberOfTiles * sizeof(int*)); 
    for (int i = 0; i < mapPtr->numberOfTiles; i++){mapPtr->arrayena[i] = malloc(mapPtr->numberOfTiles * sizeof(int));}
    for (int i = 0; i < mapPtr->numberOfTiles; i++){mapPtr->visited[i] = malloc(mapPtr->numberOfTiles * sizeof(int));}

    // Assigning the obstacle positions to make the circular-like shape
    for (int r = 0; r < mapPtr->numberOfTiles; r++)
    {
        for (int c = 0; c < mapPtr->numberOfTiles; c++)
        {
            mapPtr->arrayena[r][c]=0;
            mapPtr->visited[r][c]=0;

            if (r == 0 || r == mapPtr->numberOfTiles-1){mapPtr->arrayena[r][c]=1; } 
            if (c == 0 || c == mapPtr->numberOfTiles-1){mapPtr->arrayena[r][c]=1; }

            for (int i = 1; i <= mapPtr->numberOfTiles/3; i++) {  
                if ((r == i || r == (mapPtr->numberOfTiles-1) - i) && (c <= mapPtr->numberOfTiles/3 - i || c >= 2*(mapPtr->numberOfTiles-1)/3 + i)) {mapPtr->arrayena[r][c] = 1;} 
            }

        }
        
        
    }
    
    mapPtr->arrayena[botPtr->rowOfRobot][botPtr->colOfRobot]=3; // Marking the tile as full so no marker can be generated on it.

    // Assigning random marker positions
    int markersGenerated = 0;
    while (markersGenerated < botPtr->markersNeeded) 
    {
        int p = rand()%(mapPtr->numberOfTiles); // Assign a random row index
        int q = rand()%(mapPtr->numberOfTiles); // Assign a random column index 

        if (mapPtr->arrayena[p][q] == 0) 
        {
            mapPtr->arrayena[p][q] = 2; 
            markersGenerated++;
        }
    }
}


void drawBackground(map *mapPtr, aRobot *botPtr)
{
    background();
    // Draws grid lines
    setColour(gray);
    for (int verticalStart = mapPtr->tileSize; verticalStart < mapPtr->mapSize; verticalStart += mapPtr->tileSize)
    {
        drawLine(0, verticalStart, mapPtr->mapSize, verticalStart);
    }
    for (int horizontalStart = mapPtr->tileSize; horizontalStart < mapPtr->mapSize; horizontalStart += mapPtr->tileSize)
    {
        drawLine(horizontalStart, 0, horizontalStart, mapPtr->mapSize);
    }
    
    // Drawing the obstacles and markers based on arrayena's elements
    for (int i = 0; i < mapPtr->numberOfTiles; i++)
    {
        for (int j = 0; j < mapPtr->numberOfTiles; j++)
        {
            if (mapPtr->arrayena[i][j] == 1)
            {setColour(black); fillRect(j*mapPtr->tileSize + 0.1*mapPtr->tileSize, i*mapPtr->tileSize + 0.1*mapPtr->tileSize, 0.8*mapPtr->tileSize, 0.8*mapPtr->tileSize);} 
            else if (mapPtr->arrayena[i][j] == 2)
                {setColour(blue); fillArc(j*mapPtr->tileSize + 0.1*mapPtr->tileSize, i*mapPtr->tileSize + 0.1*mapPtr->tileSize, 0.8*mapPtr->tileSize, 0.8*mapPtr->tileSize, 0, 360); 
                int xUpDown[3] = {j*mapPtr->tileSize + 0.3*mapPtr->tileSize, j*mapPtr->tileSize + (0.3+0.2)*mapPtr->tileSize, j*mapPtr->tileSize + (0.3+0.4)*mapPtr->tileSize};
                int yUp[3] = {i*mapPtr->tileSize + 0.6*mapPtr->tileSize, i*mapPtr->tileSize + (0.3)*mapPtr->tileSize, i*mapPtr->tileSize + (0.6)*mapPtr->tileSize};
                int yDown[3] = {i*mapPtr->tileSize + 0.4*mapPtr->tileSize, i*mapPtr->tileSize + (0.7)*mapPtr->tileSize, i*mapPtr->tileSize + (0.4)*mapPtr->tileSize};
                setColour(white);
                setLineWidth(1);
                drawPolygon(3,xUpDown,yUp);
                drawPolygon(3,xUpDown,yDown); 
            }  
        }
    }    
}





void update(map *mapPtr, aRobot *botPtr) 
{
    if (botPtr->markersCollected >= botPtr->markersNeeded)
    {
        exit(0);
    }
    else{
        // Updates the robot's angle to stay within 0 to 2pi and updates its direction based on that angle.
        botPtr->angle = fmod(botPtr->angle + 2*M_PI, 2*M_PI); 
        double epsilon = 1e-3; 
        if (fabs(botPtr->angle - 0) < epsilon) botPtr->direction = 's';  // Checking if the angle is close enough to 0, in this case. This method was used because floating point arithematic wasn't precise enough
        else if (fabs(botPtr->angle - M_PI/2) < epsilon) botPtr->direction = 'w';
        else if (fabs(botPtr->angle - M_PI) < epsilon) botPtr->direction = 'n';
        else if (fabs(botPtr->angle - 3*M_PI/2) < epsilon) botPtr->direction = 'e'; 

        // Drawing the updated robot shape
        clear();
        setColour(red);
        fillPolygon(3, botPtr->x, botPtr->y);
        setColour(green);
        setLineWidth(5); 
        drawLine(botPtr->x[0],botPtr->y[0], botPtr->x[2], botPtr->y[2]); 
    }
}

int canMoveForward(map *mapPtr, aRobot *botPtr)
{

    if (botPtr->direction == 's') 
    {
        if (mapPtr->arrayena[botPtr->rowOfRobot+1][botPtr->colOfRobot] == 1 || mapPtr->visited[botPtr->rowOfRobot+1][botPtr->colOfRobot] == 1 ){return 0;}
        else{ return 1;}
    }
    if (botPtr->direction == 'e')
    {
        if (mapPtr->arrayena[botPtr->rowOfRobot][botPtr->colOfRobot+1] == 1 || mapPtr->visited[botPtr->rowOfRobot][botPtr->colOfRobot+1] == 1 ){return 0;}
        else{ return 1;}
    }
    if (botPtr->direction == 'n') 
    {
        if (mapPtr->arrayena[botPtr->rowOfRobot-1][botPtr->colOfRobot] == 1 || mapPtr->visited[botPtr->rowOfRobot-1][botPtr->colOfRobot] == 1 ) {return 0;}
        else{ return 1;}
    }
    if (botPtr->direction == 'w') 
    {
        if (mapPtr->arrayena[botPtr->rowOfRobot][botPtr->colOfRobot-1] == 1 || mapPtr->visited[botPtr->rowOfRobot][botPtr->colOfRobot-1] == 1 ) {return 0;}
        else{ return 1;}
    }
    return 0; // To make the robot stop if errors occur
}

int atMarker(map *mapPtr, aRobot *botPtr)
{
    if (mapPtr->arrayena[botPtr->rowOfRobot][botPtr->colOfRobot] == 2)
    {
        return 1;
    }
    else{ return 0;}
}

void pickUpMarker(map *mapPtr, aRobot *botPtr)
{
    // Visually removes collected marker
    background();
    setColour(white);
    fillArc(botPtr->xTile+0.1*mapPtr->tileSize, botPtr->yTile+0.1*mapPtr->tileSize, 0.8*mapPtr->tileSize, 0.8*mapPtr->tileSize, 0, 360);
    botPtr->markersCollected++;
    foreground();
}


void forward(map *mapPtr, aRobot *botPtr) 
{   
    if (botPtr->direction == 's')
    {
        for (int i = 0; i < 3; i++)
        {
            botPtr->y[i]+= mapPtr->tileSize;
        }
        botPtr->yTile += 1 * mapPtr->tileSize;
        botPtr->rowOfRobot+=1;

        mapPtr->visited[botPtr->rowOfRobot][botPtr->colOfRobot] = 1;
        sleep(mapPtr->waitTime);
    }
    if (botPtr->direction == 'w')
    {
        for (int i = 0; i < 3; i++)
        {
            botPtr->x[i]-= mapPtr->tileSize;
        }
        botPtr->xTile -= 1 * mapPtr->tileSize;
        botPtr->colOfRobot-=1;

        mapPtr->visited[botPtr->rowOfRobot][botPtr->colOfRobot] = 1;
        sleep(mapPtr->waitTime);
    }
    if (botPtr->direction == 'n')
    {
        for (int i = 0; i < 3; i++)
        {
            botPtr->y[i]-= mapPtr->tileSize;
        }
        botPtr->yTile -= 1 * mapPtr->tileSize;
        botPtr->rowOfRobot-=1;

        mapPtr->visited[botPtr->rowOfRobot][botPtr->colOfRobot] = 1;
        sleep(mapPtr->waitTime);
    }
    if (botPtr->direction == 'e')
    {
        for (int i = 0; i < 3; i++)
        {
            botPtr->x[i]+= mapPtr->tileSize;
        }
        botPtr->xTile += 1 * mapPtr->tileSize;
        botPtr->colOfRobot+=1;

        mapPtr->visited[botPtr->rowOfRobot][botPtr->colOfRobot] = 1;
        sleep(mapPtr->waitTime);
    }

    update(mapPtr, botPtr); 
    
    // Resetting the unrotated x and y coordinates to the rotated ones
    memmove(botPtr->xOriginalAngle, botPtr->x, sizeof(int) * 3);
    memmove(botPtr->yOriginalAngle, botPtr->y, sizeof(int) * 3);
}



void right(map *mapPtr, aRobot *botPtr)
{   
    botPtr->angle+=M_PI/2;

    // Resetting the unrotated x and y coordinates to the rotated ones
    memmove(botPtr->xOriginalAngle , botPtr->x, 3 * sizeof(int));
    memmove(botPtr->yOriginalAngle , botPtr->y, 3 * sizeof(int));  

    double totalRotation= M_PI/2;
    for (double stepRotation = 0; stepRotation <=  totalRotation; stepRotation+= totalRotation/20) 
    {
        if (stepRotation > totalRotation){ stepRotation = totalRotation; } // Does not let stepRotation go over total rotation needed
        
        // The coordinates of the centre of the grid that the robot is on.
        int vTileC = botPtr->xTile + mapPtr->tileSize/2;
        int hTileC = botPtr->yTile + mapPtr->tileSize/2; 

        for (int i = 0; i < 3; i++) // Source used for the mathematical formula to rotate a point: https://math.sci.ccny.cuny.edu/document/Rotation+of+Axes , page 3
        {
            botPtr->x[i] = vTileC + (botPtr->xOriginalAngle[i] - vTileC) * cos(stepRotation) - (botPtr->yOriginalAngle[i] - hTileC) * sin(stepRotation);
            botPtr->y[i] = hTileC + (botPtr->xOriginalAngle[i] - vTileC) * sin(stepRotation) + (botPtr->yOriginalAngle[i] - hTileC) * cos(stepRotation);

        }

        update(mapPtr, botPtr); 
        sleep(mapPtr->waitTime/10);
    }
}

void left(map *mapPtr, aRobot *botPtr)
{   
    botPtr->angle-=M_PI/2;
    memmove(botPtr->xOriginalAngle , botPtr->x, 3 * sizeof(int));
    memmove(botPtr->yOriginalAngle , botPtr->y, 3 * sizeof(int));  
    double totalRotation= -M_PI/2;
    for (double stepRotation = 0; stepRotation >=  totalRotation; stepRotation+= totalRotation/20) 
    {
        if (stepRotation < totalRotation) {stepRotation = totalRotation;}
        int vTileC = botPtr->xTile + mapPtr->tileSize/2, hTileC = botPtr->yTile + mapPtr->tileSize/2;
        for (int i = 0; i < 3; i++)
        {
            botPtr->x[i] = vTileC + (botPtr->xOriginalAngle[i] - vTileC) * cos(stepRotation) - (botPtr->yOriginalAngle[i] - hTileC) * sin(stepRotation);
            botPtr->y[i] = hTileC + (botPtr->xOriginalAngle[i] - vTileC) * sin(stepRotation) + (botPtr->yOriginalAngle[i] - hTileC) * cos(stepRotation);
        }
        update(mapPtr, botPtr); 
        sleep(mapPtr->waitTime/10);
    }
}

// Came up with a DFS-like patterns for the robot myself using recursion, inspired initially by "Depth First Search or DFS for a Graph,” GeeksforGeeks, 2016. Available URL source: https://www.geeksforgeeks.org/dsa/depth-first-search-or-dfs-for-a-graph/
void dfs(map *mapPtr, aRobot *botPtr) {
    foreground();
    mapPtr->visited[botPtr->rowOfRobot][botPtr->colOfRobot] = 1;
    if (atMarker(mapPtr,botPtr)) pickUpMarker(mapPtr,botPtr);
    for (int i = 0; i < 4; i++) {
        right(mapPtr, botPtr);
        if (canMoveForward(mapPtr, botPtr)) {
            forward(mapPtr, botPtr);
            dfs(mapPtr, botPtr);    // Starts recursion      
            left(mapPtr, botPtr); left(mapPtr, botPtr);  
            forward(mapPtr, botPtr);              
            left(mapPtr, botPtr); left(mapPtr, botPtr);  
        }
    }
}



// Declaring the cmdLineArguments.c function
void parseArgs(int argc, char **argv);

int main(int argc, char **argv) 
{
    parseArgs(argc, argv);

    defaultMap.tileSize = defaultMap.mapSize / defaultMap.numberOfTiles; // Width and height of a tile, in pixels
    
    initArrays(mapPtr);
    
    if (defaultMap.arrayena[botPtr->rowOfRobot][botPtr->colOfRobot] == 1 || botPtr->colOfRobot >= defaultMap.numberOfTiles || botPtr->rowOfRobot>=defaultMap.numberOfTiles) 
    {
        fprintf(stderr, "\n \n Error raised: cannot place the robot at this row and column due to obstruction by obstacles, or robot being out of bounds. Please try another row-column combination. \n \n");
        exit(EXIT_FAILURE); 
    }

    waterMelonBot.xTile = defaultMap.tileSize * waterMelonBot.colOfRobot;
    waterMelonBot.yTile = defaultMap.tileSize * waterMelonBot.rowOfRobot; 
    
    waterMelonBot.x[0] = waterMelonBot.xTile + 0.1*defaultMap.tileSize;
    waterMelonBot.x[1] = waterMelonBot.xTile + 0.1*defaultMap.tileSize + (0.8*defaultMap.tileSize/2);
    waterMelonBot.x[2] = waterMelonBot.xTile + 0.9*defaultMap.tileSize;
    waterMelonBot.y[0] = waterMelonBot.yTile + 0.1*defaultMap.tileSize;
    waterMelonBot.y[1] = waterMelonBot.yTile+0.9*defaultMap.tileSize;
    waterMelonBot.y[2] = waterMelonBot.yTile + 0.1*defaultMap.tileSize;

    

    setWindowSize(defaultMap.mapSize, defaultMap.mapSize);
    srand(time(NULL)); 
    drawBackground(mapPtr, botPtr);
    dfs(mapPtr, botPtr);
}