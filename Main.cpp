/* Differences between this and normal catan
* each person owns a board square instead of a settlement between the squares
* player size is currently set at 4
* board is square, not hexagonal
* as many deserts as are randomly generated
*/

/* Command line arguments-
* SIZE= n, where n is the nxn size of the board
* PLAYERNUM = number of players playing the game
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
int GetCurrentPlayer(unsigned int i);
void UseMon(int currentPlayer);
void UseYOP(int currentPlayer);
void UserRoad(int currentPlayer);
void UserKnight(int currentPlayer);
int main(int argc, char *argv[])
{
  if (argc != 3)
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
	    

	    // convert the player number to an int
		istringstream ss2(argv[2]);
		int playerNum;
		if (!(ss2 >> playerNum)) cerr << "Invalid number " << argv[2] << '\n';
		
		// need to create the board
		board.MakeBoard(size);
		
		// just for testing
		board.PrintBoard();
		
		// create a vector of all the players (for now just 4)
		for (int i = 0; i < playerNum; i++)
		  {
			board.allPlayers.push_back(Player());
			board.allPlayers[i].playerID = i;
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
  // each player puts down a settlement and a road
  for (unsigned int i = 0; i < board.allPlayers.size(); i++)
    {
      // current player gets to place a house and a road
		board.PlaceHouse(i,-1,-1);
		//-1 signifies a first turn placement
		board.PlaceRoad(i,-1,-1,-1);
    }
  
  // now do the same thing again
  for (unsigned int i = 0; i < board.allPlayers.size(); i++)
    {
      // current player gets to place a house and a road
      board.PlaceHouse(i,-1,-1);
      board.PlaceRoad(i,-1,-1,-1);
    }
  board.PrintBoard();
}

void RunTurn(void)
{
  // run a turn for each person in the game
  for (unsigned int i = 0; i < board.allPlayers.size(); i++){
    Player* p = &board.allPlayers[i];
    p->Print();
    // first, roll the die and see who gets what
    board.RollResourceDice(board.allPlayers[i]);
    vector<int> turn;
    bool road = false;
    bool settlement = false;
    bool city = false;
    bool dev = false;
    int tradeO=0, tradeWh=0, tradeWo=0, tradeS=0, tradeB=0;
    int mon=0,year=0,build=0,knight=0;
    int brick1=0,wood1=0,wheat1=0,sheep1=0,ore1=0; //had to rename because it mathced the enum name
    
    for(unsigned int j=0; j<p->developmentHand.size(); j++){
      if(p->developmentHand[j] == monopoly){
	turn.push_back(1);
	mon++;
      } else if(p->developmentHand[j] == yearOfPlenty){
	turn.push_back(2);
	year++;
      } else if(p->developmentHand[j] == roadBuilding){
	turn.push_back(3);
	build++;
      } else if(p->developmentHand[j] == knight){
	turn.push_back(4);
	knight++;
      }
    }
    for(unsigned int j=0; j<p->resourceHand.size(); j++){
      //cout << i << " has " << p.resourceHand[j] << endl;
      if(p->resourceHand[j] == ore){
	ore1++;
      } else if(p->resourceHand[j] == wheat){
	wheat1++;
      } else if(p->resourceHand[j] == sheep){
	sheep1++;
      } else if(p->resourceHand[j] == brick){
	brick1++;
      } else if(p->resourceHand[j] == wood){
	wood1++;
      } else {
	cout << "error player has an invalid resource" << endl;
      }
    }
    if(wood1 >= 3){
      tradeWo+=int(wood/3);
    }
    if(ore1 >= 3){
      tradeO+=int(wood/3);
    }
    if(sheep1 >= 3){
      tradeS+=int(wood/3);
    }
    if(brick1 >= 3){
      tradeB+=int(wood/3);
    }
    if(wood1 >= 3){
      tradeWo+=int(wood/3); //can trade wood
    }
    if(wheat1 >= 3){
      tradeWh+=int(wood/3); //can trade wheat
    }
    if(wood1 >= 1 && brick1 >= 1){
      road = true;
      turn.push_back(5); //can make road
    }
    if(brick1 >=1 && wood1 >= 1 && wheat1 >= 1 && sheep1 >= 1){
      city = true;
      turn.push_back(6); //can make settlement
    }
    if(wheat1 >= 2 && ore1 >= 3){
      settlement = true;
      turn.push_back(7); //can make city
    }
    if(sheep1 >= 1 && wheat1 >=  1 && ore1 >= 1){
      dev = true;
      turn.push_back(8); //can make dev card
    }
    if(knight >= 1){
      if(board.board[board.robberX][board.robberY].owner == int(i)){
	//move robber
	cout << "Ryan implement move robber" << endl;
      }
    }
    if(not settlement && not city && not road && dev){
      //aquire a development car
    }
    if(true){
    //if(turn.size() == 0){
      //no possible move, try to trade for other resources
      if(sheep1 >=1 && wheat1 >= 1){
	if(tradeB > 1){
	  board.allPlayers[i].Remove(3,brick);
	  board.allPlayers[i].Remove(1,sheep);
	  board.allPlayers[i].Remove(1,wheat);
	  board.allPlayers[i].AddDev();
	} else if(tradeWo > 1){
	  board.allPlayers[i].Remove(3,wood);
	  board.allPlayers[i].Remove(1,sheep);
	  board.allPlayers[i].Remove(1,wheat);	  
	  board.allPlayers[i].AddDev();
	} else if(sheep1 >= 4){
	  board.allPlayers[i].Remove(4,sheep);
	  board.allPlayers[i].Remove(1,wheat);
	  board.allPlayers[i].AddDev();
	} else if(wheat1 >= 4){
	  board.allPlayers[i].Remove(4,wheat);
	  board.allPlayers[i].Remove(1,sheep);
	  board.allPlayers[i].AddDev();
	}
      } else if(sheep1 >=1 && ore1 >= 1){
	if(tradeB > 1){
	  board.allPlayers[i].Remove(3,brick);
	  board.allPlayers[i].Remove(1,sheep);
	  board.allPlayers[i].Remove(1,ore);
	  board.allPlayers[i].AddDev();
	} else if(tradeWo > 1){
	  board.allPlayers[i].Remove(3,wood);
	  board.allPlayers[i].Remove(1,sheep);
	  board.allPlayers[i].Remove(1,ore);	  
	  board.allPlayers[i].AddDev();
	} else if(sheep1 >= 4){
	  board.allPlayers[i].Remove(4,sheep);
	  board.allPlayers[i].Remove(1,ore);
	  board.allPlayers[i].AddDev();
	} else if(ore1 >= 4){
	  board.allPlayers[i].Remove(4,ore);
	  board.allPlayers[i].Remove(1,sheep);
	  board.allPlayers[i].AddDev();
	}
      } else if(wheat1 >=1 && ore1 >= 1){
	if(tradeB > 1){
	  board.allPlayers[i].Remove(3,brick);
	  board.allPlayers[i].Remove(1,wheat);
	  board.allPlayers[i].Remove(1,ore);
	  board.allPlayers[i].AddDev();
	} else if(tradeWo > 1){
	  board.allPlayers[i].Remove(3,wood);
	  board.allPlayers[i].Remove(1,wheat);
	  board.allPlayers[i].Remove(1,ore);	  
	  board.allPlayers[i].AddDev();
	} else if(wheat1 >= 4){
	  board.allPlayers[i].Remove(4,wheat);
	  board.allPlayers[i].Remove(1,ore);
	  board.allPlayers[i].AddDev();
	} else if(ore1 >= 4){
	  board.allPlayers[i].Remove(4,ore);
	  board.allPlayers[i].Remove(1,sheep);
	  board.allPlayers[i].AddDev();
	}
      }
    }
  }
}
void UseMon(int currentPlayer){

}

void UseYOP(int currentPlayer){

}

void UserRoad(int currentPlayer){

}

void UserKnight(int currentPlayer){

}
