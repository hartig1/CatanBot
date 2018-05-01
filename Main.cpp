/* Differences between this and normal catan
* each person owns a board square instead of a settlement between the squares
* player size is currently set at 4
* board is square, not hexagonal
* as many deserts as are randomly generated
*/

#include <iostream>
#include <string>
#include <vector>
#include <sstream>

using namespace std;

#include "Board.h"
#include "Player.h"

// global variables for ease of use
// the board that will be used
Board board;

// definitions of functions used later
void RunFirstTurn(void);
void RunTurn(void);
Owner GetCurrentPlayer(unsigned int i);

int main(int argc, char *argv[])
{
	if (argc != 2)
	{
		cout << "\nNot enough command-line arguments\n";
		// return error?
		return -1;
	}
	
	else
	{
		// convert the size of the board to int
		// https://stackoverflow.com/questions/2797813/how-to-convert-a-command-line-argument-to-int
		istringstream ss(argv[1]);
		int size;
		if (!(ss >> size)) cerr << "Invalid number " << argv[1] << '\n';
		
		// need to create the board
		board.MakeBoard(size);
		
		// just for testing
		board.PrintBoard();
		
		// create a vector of all the players (for now just 4)
		for (int i = 0; i < 4; i++)
		{
			board.allPlayers.push_back(Player());
		}
		
		// run the first turn where every player gets two settlements and two roads
		RunFirstTurn();
		
		// holds whether or not to end the game
		bool done = false;
		// runs a single turn, forever in the while loop
		while (!done)
		{
			RunTurn();
			// iterate through the players and see if any of them won
			for (unsigned int i = 0; i < board.allPlayers.size(); i++)
			{
				if (board.allPlayers[i].victoryPoints >= 10)
				{
					done = true;
					cout << "\nPlayer " << i << " has won!\n";
				}
			}
		}
		
		// clean up any memory allocation from the board
		board.CleanupBoard();
		return 0;
	}
}

void RunFirstTurn(void)
{
	Owner currentPlayer;
	// each player puts down a settlement and a road
	for (unsigned int i = 0; i < board.allPlayers.size(); i++)
	{
		// setup who is the current player
		currentPlayer = GetCurrentPlayer(i);
		
		// current player gets to place a house and a road
		board.PlaceHouse(currentPlayer);
		board.PlaceRoad(currentPlayer);
	}
	
	// now do the same thing again
	for (unsigned int i = 0; i < board.allPlayers.size(); i++)
	{
		// set up who is the current player
		currentPlayer = GetCurrentPlayer(i);
		
		// current player gets to place a house and a road
		board.PlaceHouse(currentPlayer);
		board.PlaceRoad(currentPlayer);
	}
	
}

void RunTurn(void)
{
	// run a turn for each person in the game
	for (unsigned int i = 0; i < board.allPlayers.size(); i++)
	{
	// first, roll the die and see who gets what
	board.RollResourceDice();
	
	// next, the current player gets to place or buy what they want
		/* Ryan stuff goes here */
	}
}

Owner GetCurrentPlayer(unsigned int i)
{
	// set up who is the current player
	switch(i)
	{
		case 0:
			return player1;
		case 1:
			return player2;
		case 2:
			return player3;
		case 3:
			return player4;
		default:
			cout << "\nSomething went wrong in GetCurrentPlayer\n";
			return nobody;
	}
}