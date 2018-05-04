#ifndef BOARDSQUARE_H
#define BOARDSQUARE_H

#include <iostream>
#include <string>
#include <vector>
#include <stdlib.h>
#include <time.h>
using namespace std;

#include "Road.h"
#include "Enums.h"

class BoardSquare
{
public:
	// constructor
	BoardSquare(Resource resourceType);
	// roads at the top, bottom, left, and right of square. Don't necessarily need to exist
	Road top;
	Road bottom;
	Road left;
	Road right;
	// whether or not the square contains a town
	bool hasTown;
	// whether or not the square contains a city
	bool hasCity;
	// who owns this board square
	int owner;
	//what type of resource this board square is
	Resource type;
	// what the number of the tile is
	int number;
	bool hasRobber;
};

BoardSquare::BoardSquare(Resource resourceType)
{
	type = resourceType;
	hasTown = false;
	hasCity = false;
	hasRobber = false;
	owner = -1;
	// randomly assign this a number from 2 to 12
	// but not 7 thats the robber tile only
	number = (rand() % 6) + (rand() % 6) + 2;
	while(number == 7 && type != desert){
	  number = (rand() % 6) + (rand() % 6) + 2;
	}
}


#endif
