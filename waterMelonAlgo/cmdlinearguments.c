#include <stdlib.h> 
#include <math.h>
#include <stdio.h>



typedef struct aRobot {
    int markersNeeded;
    int markersCollected;
    int colOfRobot;
    int rowOfRobot;
    double angle;
    char direction;
} aRobot;

extern aRobot waterMelonBot;
extern aRobot *botPtr;



typedef struct map {
    int numberOfTiles;
    int waitTime;
    int mapSize;
    int tileSize;
    int **arrayena;
    int **visited;
} map;
extern map defaultMap;
extern map *mapPtr;

void parseArgs(int argc, char **argv)
{
    mapPtr->numberOfTiles = 15;
    mapPtr->waitTime = 40;
    botPtr->markersNeeded = 30;
    botPtr->colOfRobot = 10;
    botPtr->rowOfRobot = 10; 
    botPtr->direction = 'n';
    if (argc == 7) 
    {        
        mapPtr->numberOfTiles = atoi(argv[1]);
        mapPtr->waitTime = atoi(argv[2]);
        botPtr->markersNeeded = atoi(argv[3]);
        botPtr->colOfRobot = atoi(argv[4]) - 1; // Note: The extra '-1's convert this from 0-based array indexing to 1-based human counting
        botPtr->rowOfRobot = atoi(argv[5]) - 1; 
        botPtr->direction = argv[6][0]; 
        if (*argv[6]=='s'){botPtr->angle = 0;} 
        if (*argv[6]=='w'){botPtr->angle = M_PI/2;}
        if (*argv[6]=='n'){botPtr->angle = M_PI;}
        if (*argv[6]=='e'){botPtr->angle = 3*M_PI/2;}
    }

}
 // source: Coursework Question Sheet, page 6-7, posted on Moodle