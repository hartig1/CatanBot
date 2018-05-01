#ifndef PLAYER_H
#define PLAYER_H

#include <iostream>
#include <string>
#include <vector>
using namespace std;

#include "Enums.h"


class Player
{
public:
	// constructor
	Player();
	int victoryPoints;
	vector <Resource> resourceHand;
	vector <DevelopmentCard> developmentHand;
	// contains a vector of pointers to all the board squares that the player owns
	vector <BoardSquare *> ownedSquares;
};

Player::Player()
{
	victoryPoints = 0;
}


#endif