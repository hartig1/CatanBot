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
	void Print();
	int victoryPoints;
	vector <Resource> resourceHand;
	vector <DevelopmentCard> developmentHand;
	// contains a vector of pointers to all the board squares that the player owns
	vector <BoardSquare *> ownedSquares;
	// just in case, "enumeration" of the player
	int playerID;
};

Player::Player()
{
	victoryPoints = 0;
}
void Player::Print(){
  cout << "------------------------------------------------" << endl;
  cout << "Player: " << playerID << " vp: " << victoryPoints << endl;
  for(unsigned int i=0; i<resourceHand.size(); i++){
    if(resourceHand[i] == ore){
      cout << "ore ";
    } else if(resourceHand[i] == wheat){
      cout << "wheat ";
    } else if(resourceHand[i] == sheep){
      cout << "sheep ";
    } else if(resourceHand[i] == brick){
      cout << "brick ";
    } else if(resourceHand[i] == wood){
      cout << "wood ";
    } else if(resourceHand[i] == desert){
      cout << "Something is wrong why do you have a desert";
    }
  }
  cout << endl;
  for(unsigned int i=0; i<developmentHand.size(); i++){
    if(developmentHand[i] == monopoly){
      cout << "monopoly ";
    } else if(developmentHand[i] == yearOfPlenty){
      cout << "yearOfPlenty ";
    } else if(developmentHand[i] == roadBuilding){
      cout << "roadBuilding ";
    } else if(developmentHand[i] == knight){
      cout << "knight ";
    }
  }
  cout << endl;
  for(unsigned int i=0; i<ownedSquares.size(); i++){
    if(ownedSquares[i]->type == ore){
      cout << "Ore tile: " << ownedSquares[i]->number << endl;
    } else if(ownedSquares[i]->type == wheat){
      cout << "Wheat tile: " << ownedSquares[i]->number << endl;
    } else if(ownedSquares[i]->type == sheep){
      cout << "Sheep tile: " << ownedSquares[i]->number << endl;
    } else if(ownedSquares[i]->type == brick){
      cout << "Brick tile: " << ownedSquares[i]->number << endl;
    } else if(ownedSquares[i]->type == wood){
      cout << "Wood tile: " << ownedSquares[i]->number << endl;
    } else if(ownedSquares[i]->type == desert){
      cout << "Desert tile: ";
    }
  }
  cout << "------------------------------------------------" << endl;
  char x;
  cin >> x;
} 
#endif
