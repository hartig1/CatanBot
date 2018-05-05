#ifndef PLAYER_H
#define PLAYER_H

#include <iostream>
#include <string>
#include <vector>
#include <assert.h>
using namespace std;

#include "Enums.h"


class Player
{
public:
	// constructor
	Player();
	void Print();
	void Remove(int, Resource);
	void AddDev();
	void UseDev(DevelopmentCard d);
	int victoryPoints;
	vector <Resource> resourceHand;
	vector <DevelopmentCard> developmentHand;
	// contains a vector of pointers to all the board squares that the player owns
	vector <BoardSquare *> ownedSquares;
	// just in case, "enumeration" of the player
	int playerID;
	int armySize;
	int roadSize;
};

Player::Player()
{
	victoryPoints = 0;
	armySize = 0;
	roadSize = 0;
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
    if(ownedSquares[i]->hasCity){
      cout << "City ";
    }
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
  //cin >> x;
}
void Player::Remove(int i, Resource r){
  int size = resourceHand.size();
  for(int x=0; x<size; x++){
    assert(x>-1 && x<(int)resourceHand.size());
    if(resourceHand[x] == r){
      resourceHand.erase(resourceHand.begin()+x);
      i--;
      if(i<=0) return;
    }
    if(size != (int)resourceHand.size()){
      x--;
      size = resourceHand.size();
    }
  }
}

void Player::AddDev(){
  int card = rand()%5;
  if(card ==0){
    developmentHand.push_back(monopoly);
  } else if(card ==1){
    developmentHand.push_back(yearOfPlenty);
  } else if(card ==2){
    developmentHand.push_back(roadBuilding);
  } else if(card ==3){
    developmentHand.push_back(knight);
  } else if(card ==4){
    victoryPoints++;
  } else {
    cout << "Error in player addDev" << endl;
  }
}

void Player::UseDev(DevelopmentCard d){
  for(unsigned int i=0; i<developmentHand.size(); i++){
    if(developmentHand[i] == d){
      developmentHand.erase(developmentHand.begin()+i);
      return;
    }
  }
  cout << "Development card did not exist, player.UserDev()" << endl;
}

#endif
