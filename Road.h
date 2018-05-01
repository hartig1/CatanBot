#ifndef ROAD_H
#define ROAD_H

#include <iostream>
#include <string>
#include <vector>
using namespace std;

#include "Enums.h"

class Road
{
public:
	// constructor
	Road();
	// whether this road exists or not
	bool exists;
	// who owns this road
	Owner owner;
};

// constructor
Road::Road(void)
{
	// originally, this road does not exist
	exists = false;
}


#endif