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
	void PlaceHouse(int currentPlayer);
	void PlaceRoad(int currentPlayer);
	
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
	// iterate through every row of the board
	for (int i = 0; i < size*2+3; i++)
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
	}
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
  vector<BoardSquare*> available;
  for(int i=0; i<size; i++){
    for(int j=0; j<size; j++){
      if(board[i][j].hasTown == false){
	available.push_back(&board[i][j]);
      }
    }
  }
  available[1]->owner = currentPlayer;
  allPlayers[currentPlayer].ownedSquares.push_back(available[1]);
	// place the house
}

// places a road for the given owner
void Board::PlaceRoad(int currentPlayer)
{
	// decide where to place the road
	/* Ryans stuff */
  vector<BoardSquare*> available;
  for(int i=0; i<size; i++){
    for(int j=0; j<size; j++){
      if(board[i][j].owner == currentPlayer){
	available.push_back(&board[i][j]);
      }
    }
  }
  for(unsigned int i=0; i<available.size(); i++){
    if(available[i]->top.exists != true){
      available[i]->top.exists = true;
      available[i]->owner = currentPlayer;
    }
  }
	// place the road
}



#endif
