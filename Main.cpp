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
void UseMon(int currentPlayer, Resource r);
void UseYOP(int currentPlayer, Resource r1, Resource r2);
void UseRoad(int currentPlayer);
void UseKnight(int currentPlayer);
bool canBuildCity(int currentPlayer);
bool canBuildTown(int currentPlayer);
int cityProg(int currentPlayer); //returns the number of resourced the user has that can be used in building a city
int townProg(int currentPlayer);
int roadProg(int currentPlayer);
void buildCity(int currentPlayer);
void buildTown(int currentPlayer);
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
    int mon=0,year=0,build=0,knight1=0;
    int brick1=0,wood1=0,wheat1=0,sheep1=0,ore1=0; //had to rename because it mathced the enum name

    for(unsigned int j=0; j<p->developmentHand.size(); j++){
      //cout << j << endl;
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
	knight1++;
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
    cerr << "Here" <<endl;
    if(canBuildCity(i) && cityProg(i) ==5){
      buildCity(i);
    } else if(canBuildCity(i) && cityProg(i) == 4 && (tradeWo + tradeB + tradeS) >=1){
      if(tradeWo >=1){
	if(wheat1 <2){
	  board.allPlayers[i].Remove(3,wood);
	  board.allPlayers[i].resourceHand.push_back(wheat);
	  buildCity(i);
	} else if(ore1 <2){
	  board.allPlayers[i].Remove(3,wood);
	  board.allPlayers[i].resourceHand.push_back(ore);
	  buildCity(i);
	}
      } else if(tradeB >=1){
	if(wheat1 <2){
	  board.allPlayers[i].Remove(3,brick);
	  board.allPlayers[i].resourceHand.push_back(wheat);
	  buildCity(i);
	} else if(ore1 <2){
	  board.allPlayers[i].Remove(3,brick);
	  board.allPlayers[i].resourceHand.push_back(ore);
	  buildCity(i);
	}
      } else if(tradeS >=1){
	if(wheat1 <2){
	  board.allPlayers[i].Remove(3,sheep);
	  board.allPlayers[i].resourceHand.push_back(wheat);
	  buildCity(i);
	} else if(ore1 <2){
	  board.allPlayers[i].Remove(3,sheep);
	  board.allPlayers[i].resourceHand.push_back(ore);
	  buildCity(i);
	}
      }
    } else if(canBuildTown(i) && townProg(i) ==4){
      //buildTown(i);
    }
    if(knight1 >= 1){
      //if we only have 1 knight wait until the robber is on our square to use it
      if(board.board[board.robberX][board.robberY].owner == int(i) && knight1 == 1){
	//move robber
	UseKnight(i);
	//if we have multiple knights use on against the best player
      } else if(knight >= 2){
	UseKnight(i);
      }
    }
    if(mon >= 1){
      //take the most resources
      int numO=0,numWh=0,numWo=0,numS=0,numB=0;
      for(unsigned int b=0; b<board.allPlayers.size(); b++){
	if(b != i){//i is current player
	  for(unsigned int c=0; c<board.allPlayers[b].resourceHand.size(); c++){
	    if(board.allPlayers[b].resourceHand[c] == ore){
	      numO++;
	    } else if(board.allPlayers[b].resourceHand[c] == wood){
	      numWo++;
	    } else if(board.allPlayers[b].resourceHand[c] == wheat){
	      numWh++;
	    } else if(board.allPlayers[b].resourceHand[c] == brick){
	      numB++;
	    } else if(board.allPlayers[b].resourceHand[c] == sheep)
	      numS++;
	  }
	}
      }
      if(numO >= numWh && numO >= numWo && numO >= numS && numO >= numB){
	UseMon(i, ore);
      } else if(numWh >= numO && numWh >= numWo && numWh >= numS && numWh >= numB){
	UseMon(i, wheat);
      } else if(numWo >= numO && numWo >= numWh && numWo >= numS && numWo >= numB){
	UseMon(i, wood);
      } else if(numS >= numO && numS >= numWh && numS >= numWo && numS >= numB){
	UseMon(i, sheep);
      } else if(numB >= numO && numB >= numWh && numB >= numS && numB >= numWo){
	UseMon(i, brick);
      }

    }
    if(year >= 1){
      //brick1 wood1 ore1 wheat1 sheep1
      //if()
      if(canBuildCity(i) && cityProg(i) >= 3){
	
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
void UseMon(int currentPlayer, Resource r){
  int count=0; //total to add to user
  for(unsigned int i=0; i<board.allPlayers.size(); i++){
    if((int)i != currentPlayer){
      int tempCount=0; //number to take from others
      for(unsigned int j=0; j<board.allPlayers[i].resourceHand.size(); j++){
	if(board.allPlayers[i].resourceHand[j] == r){
	  count++;
	  tempCount++;
	}
      }
      board.allPlayers[i].Remove(tempCount,r);
    }
  }
  for(int i=0; i<count; i++){
    board.allPlayers[currentPlayer].resourceHand.push_back(r);
  }
}

void UseYOP(int currentPlayer, Resource r1, Resource r2){
  board.allPlayers[currentPlayer].resourceHand.push_back(r1);
  board.allPlayers[currentPlayer].resourceHand.push_back(r2);  
}

void UseRoad(int currentPlayer){

}

//dev: true if they used a dev card, false if they rolled 7
void UseKnight(int currentPlayer){
  //cout << "Using knight" << endl;
  int winning;
  int winningVP=0;
  for(unsigned int i=0; i<board.allPlayers.size(); i++){
    if((int)i != currentPlayer && board.allPlayers[i].victoryPoints > winningVP){
      winning =i;
      winningVP = board.allPlayers[i].victoryPoints;
    }
  }
  for(int i=0; i<board.size; i++){
    for(int j=0; j<board.size; j++){
      if(board.board[i][j].owner == winning){
	board.board[board.robberX][board.robberY].hasRobber = false;
	board.board[i][j].hasRobber = true;
	board.robberX = i;
	board.robberY = j;
	board.allPlayers[currentPlayer].UseDev(knight);
	board.PrintBoard();
	board.allPlayers[currentPlayer].armySize++;
	if(board.allPlayers[currentPlayer].armySize > board.biggestArmy){
	  cout << "New biggest army: " << currentPlayer <<  endl;
	  if(board.biggestArmyOwner == -1){
	    board.biggestArmyOwner = currentPlayer;
	    board.biggestArmy = board.allPlayers[currentPlayer].armySize;
	    board.allPlayers[currentPlayer].victoryPoints +=2;
	  } else {
	    board.allPlayers[board.biggestArmyOwner].victoryPoints -= 2;
	    board.allPlayers[currentPlayer].victoryPoints +=2;
	    board.biggestArmy = board.allPlayers[currentPlayer].armySize;
	    board.biggestArmyOwner = currentPlayer;
	  }
	}
	return;
      }
    }
  }
  cout << "Something went wrong in UseKnight" << endl;
}
bool canBuildCity(int currentPlayer){
  cerr << "here2" << endl;
  for(unsigned int i=0; i<board.allPlayers[currentPlayer].ownedSquares.size(); i++){
    if(!board.allPlayers[currentPlayer].ownedSquares[i]->hasCity){
      return true;
    }
  }
  return false;
}
bool canBuildTown(int currentPlayer){
  cerr << "here4" << endl;
  for(int i=0; i<board.size; i++){
    for(int j=0; j<board.size; j++){
      //non edge cases
      //if(i != 0 && i != board.size-1 && j != 0 and j != board.size-1){
      try{
	cerr << "here6" << endl;
	if(board.board[i][j].owner == currentPlayer){
	  cerr << "here7" << endl;
	  if(board.board[i][j].top.owner == currentPlayer){
	    if(board.board[i][j-1].owner == -1){
	      return true;
	    }
	  }
	  cerr << "here8" << endl;
	  if(board.board[i][j].right.owner == currentPlayer){
	    if(board.board[i+1][j].owner == -1){
	      return true;
	    }
	  }
	  cerr << "here9" << endl;
	  if(board.board[i][j].bottom.owner == currentPlayer){
	    if(board.board[i][j+1].owner == -1){
	      return true;
	    }
	  }
	  cerr << "here10" << endl;
	  if(board.board[i][j].left.owner == currentPlayer){
	    if(board.board[i-1][j].owner == -1){
	      return true;
	    }
	  }
	  cerr << "here11" << endl;
	}
	cerr << "here12" << endl;
      } catch(...){
	continue;
      }
    }
  }
  cerr << "here7" << endl;
  return false;
}
int cityProg(int currentPlayer){
  cerr << "here3" << endl;
  int brick1=0, wheat1=0;
  for(unsigned int i=0; i<board.allPlayers[currentPlayer].resourceHand.size(); i++){
    if(board.allPlayers[currentPlayer].resourceHand[i] == wheat){
      if(wheat1 < 2){
	wheat1++;
      } else {
	continue;
      }
    } else if(board.allPlayers[currentPlayer].resourceHand[i] == brick){
      if(brick1 <3){
	brick1++;
      } else {
	continue;
      }
    }
  }
  return brick1+wheat1;
}
int townProg(int currentPlayer){
  cerr << "here5" << endl;
  int brick1=0, wheat1=0, sheep1=0, wood1=0;
  for(unsigned int i=0; i<board.allPlayers[currentPlayer].resourceHand.size(); i++){
    if(board.allPlayers[currentPlayer].resourceHand[i] == wheat){
      if(wheat1 < 1){
	wheat1++;
      } else {
	continue;
      }
    } else if(board.allPlayers[currentPlayer].resourceHand[i] == brick){
      if(brick1 <1){
	brick1++;
      } else {
	continue;
      }
    } else if(board.allPlayers[currentPlayer].resourceHand[i] == sheep){
      if(sheep1 <1){
	sheep1++;
      } else {
	continue;
      }
    } else if(board.allPlayers[currentPlayer].resourceHand[i] == wood){
      if(wood1 <1){
	wood1++;
      } else {
	continue;
      }
    }    
  }
  return brick1+wheat1+wood1+sheep1;
}
int roadProg(int currentPlayer){
  int brick1=0, wood1=0;
  for(unsigned int i=0; i<board.allPlayers[currentPlayer].resourceHand.size(); i++){
    if(board.allPlayers[currentPlayer].resourceHand[i] == wood){
      if(wood1 < 1){
	wood1++;
      } else {
	continue;
      }
    } else if(board.allPlayers[currentPlayer].resourceHand[i] == brick){
      if(brick1 <1){
	brick1++;
      } else {
	continue;
      }
    }
  }
  return brick1+wood1;
}
void buildCity(int currentPlayer){
  for(unsigned int i=0; i<board.allPlayers[currentPlayer].ownedSquares.size(); i++){
    if(!board.allPlayers[currentPlayer].ownedSquares[i]->hasCity){
      board.allPlayers[currentPlayer].ownedSquares[i]->hasCity = true;
      board.allPlayers[currentPlayer].victoryPoints++;
      board.allPlayers[currentPlayer].Remove(3,ore);
      board.allPlayers[currentPlayer].Remove(2,wheat);
      return;
    }
  }
  cout << "Unable to build city no spaces available" << endl;
}
void buildTown(int currentPlayer){
  for(int i=0; i<board.size; i++){
    for(int j=0; j<board.size; j++){
      try{
	if(board.board[i][j].owner == currentPlayer){
	  if(board.board[i][j].top.owner == currentPlayer){
	    if(board.board[i][j-1].owner == -1){
	      board.board[i][j-1].owner = currentPlayer;
	      board.allPlayers[currentPlayer].ownedSquares.push_back(&board.board[i][j-1]);
	      board.allPlayers[currentPlayer].victoryPoints++;
	      board.allPlayers[currentPlayer].Remove(1,sheep);
	      board.allPlayers[currentPlayer].Remove(1,wood);
	      board.allPlayers[currentPlayer].Remove(1,brick);
	      board.allPlayers[currentPlayer].Remove(1,wheat);
	      return;
	    }
	  }
	  if(board.board[i][j].right.owner == currentPlayer){
	    if(board.board[i+1][j].owner == -1){
	      board.board[i+1][j].owner = currentPlayer;
	      board.allPlayers[currentPlayer].ownedSquares.push_back(&board.board[i+1][j]);
	      board.allPlayers[currentPlayer].victoryPoints++;
	      board.allPlayers[currentPlayer].Remove(1,sheep);
	      board.allPlayers[currentPlayer].Remove(1,wood);
	      board.allPlayers[currentPlayer].Remove(1,brick);
	      board.allPlayers[currentPlayer].Remove(1,wheat);
	      return;
	    }
	  }
	  if(board.board[i][j].bottom.owner == currentPlayer){
	    if(board.board[i][j+1].owner == -1){
	      board.board[i][j+1].owner = currentPlayer;
	      board.allPlayers[currentPlayer].ownedSquares.push_back(&board.board[i][j+1]);
	      board.allPlayers[currentPlayer].victoryPoints++;
	      board.allPlayers[currentPlayer].Remove(1,sheep);
	      board.allPlayers[currentPlayer].Remove(1,wood);
	      board.allPlayers[currentPlayer].Remove(1,brick);
	      board.allPlayers[currentPlayer].Remove(1,wheat);
	      return;
	    }
	  }
	  if(board.board[i][j].left.owner == currentPlayer){
	    if(board.board[i-1][j].owner == -1){
	      board.board[i-1][j].owner = currentPlayer;
	      board.allPlayers[currentPlayer].ownedSquares.push_back(&board.board[i-1][j]);
	      board.allPlayers[currentPlayer].victoryPoints++;
	      board.allPlayers[currentPlayer].Remove(1,sheep);
	      board.allPlayers[currentPlayer].Remove(1,wood);
	      board.allPlayers[currentPlayer].Remove(1,brick);
	      board.allPlayers[currentPlayer].Remove(1,wheat);
	      return;
	    }
	  }
	}
      } catch(...){
	continue;
      }
    }
  }
}
