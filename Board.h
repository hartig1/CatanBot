#ifndef BOARD_H
#define BOARD_H

#include <iostream>
#include <string>
#include <vector>
#include <stdlib.h>
#include <time.h>
using namespace std;

#include "BoardSquare.h"
#include "Player.h"

// class that will only have one instance, and that is the game board
class Board{
public:
	// constructor
	Board(int size);
	// creates the board, needed instead of a constructor
	void MakeBoard(int size);
	// prints out the board in graphical form
	void PrintBoard();
	// memory deallocation if necessary
	void CleanupBoard();
	// rolls the dice and certain players collect their resources
	void RollResourceDice(Player currentPlayer);
	// places a house and a road
	void PlaceHouse(int currentPlayer, int x, int y);
	void PlaceRoad(int currentPlayer, int x, int y, int z);
	// robber gets to steal from players with 7 or more cards
	void RobberSteal();
	// move the robber to a different square of the board
	void MoveRobber(int boardX, int boardY);

	// 2d vector that will hold all the board squares
	vector<vector<BoardSquare> > board;
	// a vector of all the players
	vector<Player> allPlayers;
	// size of one dimension of board (board is square)
	int size;
	// position of the robber on the board
	int robberX;
	int robberY;
	int biggestArmy;
	int biggestRoad;
	int biggestArmyOwner;
	int biggestRoadOwner;
};

Board::Board(int s){
  // initialize random number generator
  
  srand(time(NULL));
  size = s;
  for(int i=0; i<size; i++){
    vector<BoardSquare> temp;
    for(int j=0; j<size; j++){
      temp.push_back(BoardSquare(ore,-1));
    }
    board.push_back(temp);
  }
  biggestArmy = 2; //players must have more than 2 knights to have biggest army
  biggestRoad = 4; //players must have more than 4 roads to have biggest road
  biggestArmyOwner = -1;
  biggestRoadOwner = -1;
}

void Board::PrintBoard(void)
{
  for(int i=0; i<size*3+2; i++){
    cout << "#";
  }
  cout << endl;
  for(int i=0; i<size; i++){
    cout << "#";
    for(int j=0; j<size; j++){
      if(board[i][j].top.exists){
				cout << " | ";
      } else {
				cout << "   ";
      }
    }
    cout << "#" <<endl << "#";
    for(int j=0; j<size; j++){
      if(board[i][j].left.exists){
				cout << "-";
      } else {
				cout << " ";
      }
      if(board[i][j].hasRobber){
	cout << "R";
      } else {
	switch(board[i][j].type){
	case ore:
	  cout << "O";
	  break;
	case wheat:
	  cout << "W";
	  break;
	case sheep:
	  cout << "S";
	  break;
	case brick:
	  cout << "B";
	  break;
	case wood:
	  cout << "T";
	  break;
	case desert:
	  cout << "D";
	  break;
	default:
	  cout << "\nSomething went wrong in PrintBoard\n";
	}
      }
      if(board[i][j].right.exists){
	cout << "-";
      } else {
	cout << " ";
      }
    }
    cout << "#" << endl << "#";
    for(int j=0; j<size; j++){
      if(board[i][j].bottom.exists){
	cout << " | ";
      } else {
	cout << "   ";
      }
    }
    cout << "#" << endl;
  }
  for(int i=0; i<size*3+2; i++){
    cout << "#";
  }
  cout << endl;
}
  // creates the board full of squares
void Board::MakeBoard(int boardSize)
{
  // store the size of the board
  size = boardSize;
  int temp;
  int id=0; //squares id
  // rows of board
  for(int i=0; i<boardSize; i++){
    board[i].clear();
  }
  board.clear();//clear default board to make a real one
  for (int i = 0; i < boardSize; i++){
    // creates the row to add to the board
    vector<BoardSquare> tempRow;
    
    // columns of board
    for (int j = 0; j < boardSize; j++){
      // find out what type of square the square will be
      temp = rand() % 5;
      // put that type of square into the board matrix
      tempRow.push_back(BoardSquare((Resource)temp,id));
      id++;
    }
    board.push_back(tempRow);
  }
  int x = rand() % size;
  int y = rand() % size;
  robberX = x;
  robberY = y;
  board[x][y].type = desert;
  board[x][y].number = 7;
  board[x][y].hasRobber = true;
}

// does memory deallocation if necessary
void Board::CleanupBoard()
{

}

// rolls the resource dice and gives the proper players resources
void Board::RollResourceDice(Player currentPlayer)
{
  // for dice rolls between 2 and 12
  int diceRoll = (rand() % 11) + 2;
  cout << "Rolled: " << diceRoll << endl;
  // if the dice roll is 7, then do something different
  if (diceRoll == 7){
    // randomly discard cards from players with greater than 7 cards
    RobberSteal();
    // current player gets to move the robber
    // decide where to move the robber to
    int winning;
    int winningVP=0;
    for(unsigned int i=0; i<allPlayers.size(); i++){
      if((int)i != currentPlayer.playerID && allPlayers[i].victoryPoints > winningVP){
	winning =i;
	winningVP = allPlayers[i].victoryPoints;
      }
    }
    for(int i=0; i<size; i++){
      for(int j=0; j<size; j++){
	if(board[i][j].owner == winning){
	  board[robberX][robberY].hasRobber = false;
	  board[i][j].hasRobber = true;
	  robberX = i;
	  robberY = j;
	}
      }
    }
    //PrintBoard();
  }
  // dole out the resources based on the roll of the dice
  // iterate through each players, seeing if they have that number
  for (unsigned int i = 0; i < allPlayers.size(); i++)
    {
      // check their ownings for the proper number
      for (unsigned int j = 0; j < allPlayers[i].ownedSquares.size(); j++)
	{
	  // do the checking
	  if (allPlayers[i].ownedSquares[j]->number == diceRoll)
	    {
	      // if it does equal the dice roll, add a card to their hand
	      allPlayers[i].resourceHand.push_back(allPlayers[i].ownedSquares[j]->type);
	    }
	}
    }
}

// places a house for the given owner
void Board::PlaceHouse(int currentPlayer, int x, int y)
{
  Player p = allPlayers[currentPlayer];
  int best=13;
  if(x == -1){
    allPlayers[currentPlayer].victoryPoints++;
    vector<BoardSquare*> available;
    for(int i=0; i<size; i++){
      for(int j=0; j<size; j++){
	if(board[i][j].hasTown == false && board[i][j].type != desert){
	  if(abs(board[i][j].number-7) < abs(best-7)){
	    			best = board[i][j].number;
	  }
	  available.push_back(&board[i][j]);
	}
      }
    }
    //cout << "Best: " << best << endl;
    for(unsigned int i=0; i<available.size(); i++){
      //cout << "number: " << available[i]->number << endl;
      if(available[i]->number == best){
	if(p.ownedSquares.size() != 0){
	  if(p.ownedSquares[0]->type == available[i]->type){
	    continue;
	  } else {
	    available[i]->owner = currentPlayer;
	    available[i]->hasTown = true;
	    allPlayers[currentPlayer].ownedSquares.push_back(available[i]);
	    return;
	  }
	} else {
	  available[i]->owner = currentPlayer;
	  available[i]->hasTown = true;
	  allPlayers[currentPlayer].ownedSquares.push_back(available[i]);
	  return;
	}
      }
    }
    for(unsigned int i=0; i<available.size(); i++){
      if(p.ownedSquares.size() != 0){
	if(p.ownedSquares[0]->type == available[i]->type){
	  continue;
	} else {
	  available[i]->owner = currentPlayer;
	  available[i]->hasTown = true;
	  allPlayers[currentPlayer].ownedSquares.push_back(available[i]);
	  return;
	}
      }
    }
  } else {
    if(board[x][y].owner == -1){
      board[x][y].owner = currentPlayer;
      board[x][y].hasTown = true;
      allPlayers[currentPlayer].ownedSquares.push_back(&board[x][y]);
      allPlayers[currentPlayer].victoryPoints++;
      return;
    } else if(board[x][y].owner == currentPlayer){
      if(board[x][y].hasCity){
	cout << "Error in placeHouse, square already has a city" << endl;
      } else {
	board[x][y].hasCity = true;
	allPlayers[currentPlayer].victoryPoints++;
	return;
      }
    } else if(board[x][y].owner != currentPlayer){
      cout << "Error in placeHouse, square owned by someone else" << endl;
    }
  }
}

// places a road for the given owner
//x and y are board coords
//z is direction 0=left, 1=top, 2=right, 3=bottom
void Board::PlaceRoad(int currentPlayer, int x, int y, int z)
{
  allPlayers[currentPlayer].roadSize++;
  if(x == -1){
    if(allPlayers[currentPlayer].ownedSquares.size() == 1){
      for(signed int i=0; i<size; i++){
	for(signed int j=0; j<size; j++){
	  if(board[i][j].owner == currentPlayer){
	    if(i != size-1){
	      if(board[i+1][j].type != allPlayers[currentPlayer].ownedSquares[0]->type){
		allPlayers[currentPlayer].ownedSquares[0]->bottom.exists = true;
		allPlayers[currentPlayer].ownedSquares[0]->bottom.owner = currentPlayer;
		return;
	      }
	    }
	    if(i != 0){
	      if(board[i-1][j].type != allPlayers[currentPlayer].ownedSquares[0]->type){
		allPlayers[currentPlayer].ownedSquares[0]->top.exists = true;
		allPlayers[currentPlayer].ownedSquares[0]->top.owner = currentPlayer;
		return;
	      }
	    }
	    if(j != size-1){
	      if(board[i][j+1].type != allPlayers[currentPlayer].ownedSquares[0]->type){
		allPlayers[currentPlayer].ownedSquares[0]->right.exists = true;
		allPlayers[currentPlayer].ownedSquares[0]->right.owner = currentPlayer;
		return;
	      }
	    }
	    if(j != 0){
	      if(board[i][j-1].type != allPlayers[currentPlayer].ownedSquares[0]->type){
		allPlayers[currentPlayer].ownedSquares[0]->left.exists = true;
		allPlayers[currentPlayer].ownedSquares[0]->left.owner = currentPlayer;
		return;
	      }
	    }

	  }
	}
      }
    } if(allPlayers[currentPlayer].ownedSquares.size() == 2){
      for(signed int i=0; i<size; i++){
	for(signed int j=0; j<size; j++){
	  if(board[i][j].owner == currentPlayer){
	    if(i != size-1){
	      if(board[i+1][j].type != allPlayers[currentPlayer].ownedSquares[1]->type){
		allPlayers[currentPlayer].ownedSquares[1]->bottom.exists = true;
		allPlayers[currentPlayer].ownedSquares[1]->bottom.owner = currentPlayer;
		return;
	      }
	    }
	    if(i != 0){
	      if(board[i-1][j].type != allPlayers[currentPlayer].ownedSquares[1]->type){
		allPlayers[currentPlayer].ownedSquares[1]->top.exists = true;
		allPlayers[currentPlayer].ownedSquares[1]->top.owner = currentPlayer;
		return;
	      }
	    }
	    if(j != size-1){
	      if(board[i][j+1].type != allPlayers[currentPlayer].ownedSquares[1]->type){
		allPlayers[currentPlayer].ownedSquares[1]->right.exists = true;
		allPlayers[currentPlayer].ownedSquares[1]->right.owner = currentPlayer;
		return;
	      }
	    }
	    if(j != 0){
	      if(board[i][j-1].type != allPlayers[currentPlayer].ownedSquares[1]->type){
		allPlayers[currentPlayer].ownedSquares[1]->left.exists = true;
		allPlayers[currentPlayer].ownedSquares[1]->left.owner = currentPlayer;
		return;
	      }
	    }

	  }
	}
      }
    } else {
      cout << "Error in placeRoad" << endl;
    }
  } else {
    if(z == 0){
      board[x][y].left.exists = true;
      board[x][y].left.owner = currentPlayer;
    } else if(z == 1){
      board[x][y].top.exists = true;
      board[x][y].top.owner = currentPlayer;
    } else if(z == 2){
      board[x][y].right.exists = true;
      board[x][y].right.owner = currentPlayer;
    } else if(z == 3){
      board[x][y].bottom.exists = true;
      board[x][y].bottom.owner = currentPlayer;
    } else {
      cout << "Error in place road, invalid z(road directio)" << endl;
    }
  }
}

// robber steals from players with 7 or more cards
void Board::RobberSteal()
{
  // loop through and check every player
  for (unsigned int i = 0; i < allPlayers.size(); i++)
    {
      // check how many cards they have
      if (allPlayers[i].resourceHand.size() >= 7)
	{
	  // if they have 7 or more cards, discard half of their cards
	  for (unsigned int j = 0; j < allPlayers[i].resourceHand.size()/2; j++)
	    {
	      // discard half of their hand
	      allPlayers[i].resourceHand.erase(allPlayers[i].resourceHand.begin());
	    }
	}
    }
}

// moves the robber to a certain spot on the board
void Board::MoveRobber(int boardX, int boardY)
{
  board[robberX][robberY].hasRobber = false;
  robberX = boardX;
  robberY = boardY;
  board[robberX][robberY].hasRobber = true;
}



#endif
