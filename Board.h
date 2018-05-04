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
class Board
{
public:
	// constructor
	Board();
	// 2d vector that will hold all the board squares
	vector<vector<BoardSquare> > board;
	// a vector of all the players
	vector<Player> allPlayers;
	// creates the board, needed instead of a constructor
	void MakeBoard(int size);
	// prints out the board in graphical form
	void PrintBoard();
	// memory deallocation if necessary
	void CleanupBoard();
	// rolls the dice and certain players collect their resources
	void RollResourceDice();
	// places a house and a road
	void PlaceHouse(int currentPlayer, int x, int y);
	void PlaceRoad(int currentPlayer, int x, int y, int z);
	
	// size of one dimension of board (board is square)
	int size;
};

Board::Board()
{
	// initialize random number generator
	srand(time(NULL));
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
      //cout << "O";
      switch(board[i][j].type)
	{
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
      //}

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
	// iterate through every row of the board
	/*for (int i = 0; i < size*2+3; i++)
	{
		// iterate through every square of the board
		for (int j = 0; j < size*2+3; j++)
		{
			// formattinng, only on the top and bottom row
			if (i == 0 || i == size*2 + 2)
			{
				cout << "#";
			}
			// formatting in the right and left column
			else if (j == 0 || j == size*2 + 2)
			{
				cout << "#";
			}
			
			// add in the left most column that is not sea
			else if (j == 1)
			{
				// only check the squares that have a tile next to them
				if (i % 2 == 0)
				{
					// this if has to be here to avoid segfaulting
					if (board[i/2-1][j/2-1].left.exists)
					{
						cout << "-";
					}
					// if it doesnt exist, use the empty identifier " "
					else
					{
						cout << " ";
					}
				}
				
				// if it does not have a tile next to it, just print " "
				else
				{
					cout << " ";
				}
			}
			
			// add in the bottom most row that is not sea
			else if (i == size*2 + 1)
			{
				// only check the squares that have a tile above them
				if (j % 2 == 0)
				{
					// this if has to be here to avoid segfaulting
					if (board[i/2-1][j/2-1].bottom.exists)
					{
						cout << "|";
					}
					// if it doesnt exist, use the empty identifier " "
					else
					{
						cout << " ";
					}
				}
				
				// if it does not have a tile next to it, just print " "
				else
				{
					cout << " ";
				}
			}

			// just the tiles and the other roads
			else
			{
				// add in the squares
				if (i % 2 == 0 && j % 2 == 0)
				{
					// case switch depending on what the tile type is
					switch(board[(i/2-1)][(j/2-1)].type)
					{
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
				else
				{
					// This will check for the vertical roads. Check to see if a road exists here
					if ((j % 2 == 0) && (board[i/2][j/2].top.exists))
					{
						cout << "|";
					}
					
					// this will check for the horizontal roads
					else if ((i % 2 == 0) && (board[i/2-1][j/2-1].right.exists))
					{
						cout << "-";
					}
					
					// no road is possible, so put the default space here
					else
					{
						cout << " ";
					}
				}
			}
		}

		// finish the row
		cout << endl;
	}*/
}

// creates the board full of squares
void Board::MakeBoard(int boardSize)
{
	// store the size of the board
	size = boardSize;
	
	int temp;
	// rows of board
	for (int i = 0; i < boardSize; i++)
	{
		// creates the row to add to the board
		vector<BoardSquare> tempRow;
		
		// columns of board
		for (int j = 0; j < boardSize; j++)
		{
			// find out what type of square the square will be
			temp = rand() % 6;

			// put that type of square into the board matrix
			switch(temp)
			{
				case 0:
					tempRow.push_back(BoardSquare(wheat));
					break;
				case 1:
					tempRow.push_back(BoardSquare(ore));
					break;
				case 2:
					tempRow.push_back(BoardSquare(brick));
					break;
				case 3:
					tempRow.push_back(BoardSquare(sheep));
					break;
				case 4:
					tempRow.push_back(BoardSquare(wood));
					break;
				case 5:
					tempRow.push_back(BoardSquare(desert));
					break;
				default:
					cout << "\nSoemthing went wrong in MakeBoard\n";

			}
		}
		
		board.push_back(tempRow);
	}
}

// does memory deallocation if necessary
void Board::CleanupBoard()
{

}

// rolls the resource dice and gives the proper players resources
void Board::RollResourceDice()
{
	// for dice rolls between 2 and 12
	int diceRoll = (rand() % 11) + 2;

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
	// decide where to place the house
	/* Ryans stuff */
  Player p = allPlayers[currentPlayer];
  int best=13;
  if(x == -1){
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
    // place the house
  } else {
    if(board[x][y].owner == -1){
      board[x][y].owner = currentPlayer;
      board[x][y].hasTown = true;
      allPlayers[currentPlayer].ownedSquares.push_back(&board[x][y]);
    } else if(board[x][y].owner == currentPlayer){
      if(board[x][y].hasCity){
	cout << "Error in placeHouse, square already has a city" << endl;
      } else {
	board[x][y].hasCity = true;
      }
    } else if(board[x][y].owner != currentPlayer){
      cout << "Error in placeHouse, square owned by someone else" << endl;
    }
  }
}

// places a road for the given owner
void Board::PlaceRoad(int currentPlayer, int x, int y, int z)
{
	// decide where to place the road
	/* Ryans stuff */
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
    }
    if(allPlayers[currentPlayer].ownedSquares.size() == 2){
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



#endif
