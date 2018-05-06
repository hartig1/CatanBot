/* Differences between this and normal catan
* each person owns a board square instead of a settlement between the squares
* player size is currently set at 4
* board is square, not hexagonal
* as many deserts as are randomly generated
*/

/* Command line arguments-
* SIZE= n, where n is the nxn size of the board
* PLAYERNUM = number of players playing the game
* pause = should the game pause after each RunTurn
    not required, 0 or blank = no pause, anything else = pause
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
bool pause1;
// definitions of functions used later
void RunFirstTurn(void);
void RunTurn(void);
int GetCurrentPlayer(unsigned int i);
void UseMon(int currentPlayer, Resource r); //monopoly development card
void UseYOP(int currentPlayer, Resource r1, Resource r2); //yearOfPlenty Dev card
void UseRoad(int currentPlayer); //road builder dev card
void UseKnight(int currentPlayer); //knight dev card
bool canBuildCity(int currentPlayer); //is there a location for building a city, player may not have the resources
bool canBuildTown(int currentPlayer); //is there a location for building a town, player may not have the resources
int cityProg(int currentPlayer); //returns the number of resourced the user has that can be used in building a city
void trade(int currentPlayer, Resource r1, Resource r2); //trades three of r1 for one of r2
int townProg(int currentPlayer); //how many out of 4 resources does a user have to build a town
int roadProg(int currentPlayer); //how many out of 2 resources does a user have to build a road
void MakeRoad(int currentPlayer); //decides where the road goes
void BuildCity(int currentPlayer); //decides where the city goes
void BuildTown(int currentPlayer); //decides where the town goes
int main(int argc, char *argv[])
{
  if (argc != 3 && argc != 4){
      cout << "\nNot enough command-line arguments\n" << endl;
      return -1;
  } else {
      // convert the size of the board to int
      // https://stackoverflow.com/questions/2797813/how-to-convert-a-command-line-argument-to-int
      istringstream ss(argv[1]);
      int size;
      pause1 = false;
      if (!(ss >> size)) cerr << "Invalid number " << argv[1] << '\n';
      // convert the player number to an int
      istringstream ss2(argv[2]);
      int playerNum;
      if (!(ss2 >> playerNum)) cerr << "Invalid number " << argv[2] << '\n';
      // need to create the board
      board.MakeBoard(size);
      // just for testing
      //board.PrintBoard();
      // create a vector of all the players (for now just 4)
      for (int i = 0; i < playerNum; i++){
	board.allPlayers.push_back(Player());
	board.allPlayers[i].playerID = i;
      }
      if(argc == 4){
	istringstream ss3(argv[3]);
	if (!(ss3 >> pause1)) cerr << "Invalid number " << argv[3] << '\n';
	if(pause1 == 0){
	  pause1 = false;
	} else if(pause1 == 1){
	  pause1 = true;
	} else {
	  cout << "Invalid arg 3, enter 1 to pause after each turn, 0 to not" << endl;
	}
      }
      // run the first turn where every player gets two settlements and two roads
      RunFirstTurn();
      // holds whether or not to end the game
      bool done = false;
      // runs a single turn, forever in the while loop until we have a winner
      while (!done){
	RunTurn();
	// iterate through the players and see if any of them won
	for (unsigned int i = 0; i < board.allPlayers.size(); i++){
	  if (board.allPlayers[i].victoryPoints >= 10){
	    done = true;
	    cout << "\nPlayer " << i << " has won!\n";
	  }
	}
      }
      //board.PrintBoard();
      // clean up any memory allocation from the board
      board.CleanupBoard();
      return 0;
  }
}

void RunFirstTurn(void)
{
  // each player puts down a settlement and a road
  for (unsigned int i = 0; i < board.allPlayers.size(); i++){
    // current player gets to place a house and a road
#pragma omp parallel sections
{
  #pragma omp section
  {
    board.PlaceHouse(i,-1,-1);
    //-1 signifies a first turn placement
    board.PlaceRoad(i,-1,-1,-1);
  }
  #pragma omp section
  {
    board.PlaceHouse(i,-1,-1);
    board.PlaceHouse(i,-1,-1);
  }
}
  }
  // now do the same thing again
  /*  for (unsigned int i = 0; i < board.allPlayers.size(); i++){
    // current player gets to place a house and a road
    board.PlaceHouse(i,-1,-1);
    board.PlaceRoad(i,-1,-1,-1);
    }*/
  //board.PrintBoard();
}

void RunTurn(void){
  // run a turn for each person in the game
  for (unsigned int i = 0; i < board.allPlayers.size(); i++){
    Player* p = &board.allPlayers[i];
    //p->Print(pause1);
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
#pragma omp parallel sections
{
#pragma omp section
  {
      if(p->developmentHand[j] == monopoly){
	turn.push_back(1);
	mon++;
      }
  }
#pragma omp section
  { 
      if(p->developmentHand[j] == yearOfPlenty){
	turn.push_back(2);
	year++;
      }
  }
#pragma omp section
  {
      if(p->developmentHand[j] == roadBuilding){
	turn.push_back(3);
	build++;
      }
  }
#pragma omp section
  {
      if(p->developmentHand[j] == knight){
	turn.push_back(4);
	knight1++;
      }
    }
 }
}
    for(unsigned int j=0; j<p->resourceHand.size(); j++){
      //cout << i << " has " << p.resourceHand[j] << endl;
#pragma omp parallel sections
{
  #pragma omp section
  {
    if(p->resourceHand[j] == ore){
      ore1++;
    }
  }
  #pragma omp section
  {
    if(p->resourceHand[j] == wheat){
      wheat1++;
    }
  } 
  #pragma omp section
  {
    if(p->resourceHand[j] == sheep){
      sheep1++;
    }
  }
  #pragma omp section
  {
    if(p->resourceHand[j] == brick){
      brick1++;
    } 
  }
  #pragma omp section
  {
    if(p->resourceHand[j] == wood){
      wood1++;
    }
  }
}
    }
#pragma omp parallel sections
{
  #pragma omp section
  {
    if(wood1 >= 3){
      tradeWo+=int(wood/3);
    }
  }
  #pragma omp section
  {
    if(ore1 >= 3){
      tradeO+=int(wood/3);
    }
  }
  #pragma omp section
  {
    if(sheep1 >= 3){
      tradeS+=int(wood/3);
    }
  }
  #pragma omp section
  {
    if(brick1 >= 3){
      tradeB+=int(wood/3);
    }
  }
  #pragma omp section
  {
    if(wood1 >= 3){
      tradeWo+=int(wood/3); //can trade wood
    }
  }  
  #pragma omp section
  {
    if(wheat1 >= 3){
      tradeWh+=int(wood/3); //can trade wheat
    }
  }
  #pragma omp section
  {
    if(wood1 >= 1 && brick1 >= 1){
      road = true;
      turn.push_back(5); //can make road
    }
  }
  #pragma omp section
  {
    if(brick1 >=1 && wood1 >= 1 && wheat1 >= 1 && sheep1 >= 1){
      city = true;
      turn.push_back(6); //can make settlement
    }
  }
  #pragma omp section
  {
    if(wheat1 >= 2 && ore1 >= 3){
      settlement = true;
      turn.push_back(7); //can make city
    }
  }
  #pragma omp section
  {
    if(sheep1 >= 1 && wheat1 >=  1 && ore1 >= 1){
      dev = true;
      turn.push_back(8); //can make dev card
    }
  }
}
#pragma omp parallel sections
{
  #pragma omp section
  {
    if(canBuildTown(i) && townProg(i) == 4){
      BuildTown(i);
    } else if(canBuildTown(i) && townProg(i) == 2 && year>=1){
      if(brick1 == 0 && wood1 == 0){
	UseYOP(i, brick, wood);
	BuildTown(i);
      } else if(brick1 == 0 && wheat1 == 0){
	UseYOP(i,brick,wheat);
	BuildTown(i);
      } else if(brick1 == 0 && sheep1 == 0){
	UseYOP(i,brick,sheep);
	BuildTown(i);
      } else if(wood1 == 0 && wheat1 == 0){
	UseYOP(i,wood,wheat);
	BuildTown(i);
      } else if(wood1 == 0 && sheep1 == 0){
	UseYOP(i,wood,sheep);
	BuildTown(i);
      } else if(wheat1 == 0 && sheep1 == 0){
	UseYOP(i,wheat,sheep);
	BuildTown(i);
      } else {
	cout << "Error in using yop to build a town" << endl;
      }
    }
  }
  #pragma omp section
  {
    if(canBuildCity(i) && cityProg(i) ==5){
      BuildCity(i);
    } else if(canBuildCity(i) && cityProg(i) == 4 && (tradeWo + tradeB + tradeS) >=1){
      if(tradeWo >=1){
	if(wheat1 <2){
	  board.allPlayers[i].Remove(3,wood);
	  board.allPlayers[i].resourceHand.push_back(wheat);
	  BuildCity(i);
	} else if(ore1 <2){
	  board.allPlayers[i].Remove(3,wood);
	  board.allPlayers[i].resourceHand.push_back(ore);
	  BuildCity(i);
	}
      } else if(tradeB >=1){
	if(wheat1 <2){
	  board.allPlayers[i].Remove(3,brick);
	  board.allPlayers[i].resourceHand.push_back(wheat);
	  BuildCity(i);
	} else if(ore1 <2){
	  board.allPlayers[i].Remove(3,brick);
	  board.allPlayers[i].resourceHand.push_back(ore);
	  BuildCity(i);
	}
      } else if(tradeS >=1){
	if(wheat1 <2){
	  board.allPlayers[i].Remove(3,sheep);
	  board.allPlayers[i].resourceHand.push_back(wheat);
	  BuildCity(i);
	} else if(ore1 <2){
	  board.allPlayers[i].Remove(3,sheep);
	  board.allPlayers[i].resourceHand.push_back(ore);
	  BuildCity(i);
	}
      }
    } else if(canBuildTown(i) && townProg(i) ==4){
      BuildTown(i);
    }
  }
  #pragma omp section
  {
    if(knight1 >= 1){
      //if we only have 1 knight wait until the robber is on our square to use it
      if(board.board[board.robberX][board.robberY].owner == int(i) && knight1 == 1){
	//move robber
	UseKnight(i);
	//if we have multiple knights use on against the winning player
      } else if(knight >= 2){
	UseKnight(i);
      }
    }
  }
  #pragma omp section
  {
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
	//} else if(numB >= numO && numB >= numWh && numB >= numS && numB >= numWo){
      } else {
	UseMon(i, brick);
      }
    }
  }
  #pragma omp section
  {
    if(build >= 1){
      UseRoad(i);
    }
  }
  #pragma omp section
  {
    if(year >= 1){
      //brick1 wood1 ore1 wheat1 sheep1
      //if()
      if(canBuildCity(i) && cityProg(i) >= 3){
	if(ore1 >= 3){
	  UseYOP(i,wheat,wheat);
	  BuildCity(i);
	} else if(ore1 >= 2){
	  UseYOP(i,wheat,ore);
	  BuildCity(i);
	} else if(ore1 >= 1){
	  UseYOP(i,ore,ore);
	  BuildCity(i);
	}
      } else {
	UseYOP(i,Resource(rand() %5), Resource(rand() %5));
      }
    }
  }
}
  if(sheep1 >=1 && wheat1 >= 1){
#pragma omp parallel sections
{
  #pragma omp section
  {
    if(tradeB > 1){
      board.allPlayers[i].Remove(3,brick);
      board.allPlayers[i].Remove(1,sheep);
      board.allPlayers[i].Remove(1,wheat);
      board.allPlayers[i].AddDev();
    }
  }
  #pragma omp section
  {
    if(tradeWo > 1){
      board.allPlayers[i].Remove(3,wood);
      board.allPlayers[i].Remove(1,sheep);
      board.allPlayers[i].Remove(1,wheat);
      board.allPlayers[i].AddDev();
    }
  }
  #pragma omp section
  {
    if(sheep1 >= 4){
      board.allPlayers[i].Remove(4,sheep);
      board.allPlayers[i].Remove(1,wheat);
      board.allPlayers[i].AddDev();
    }
  }
  #pragma omp section
  {
    if(wheat1 >= 4){
      board.allPlayers[i].Remove(4,wheat);
      board.allPlayers[i].Remove(1,sheep);
      board.allPlayers[i].AddDev();
    }
  }
}
  }
 if(sheep1 >=1 && ore1 >= 1){
#pragma omp parallel sections
{
  #pragma omp section
  {
    if(tradeB > 1){
      board.allPlayers[i].Remove(3,brick);
      board.allPlayers[i].Remove(1,sheep);
      board.allPlayers[i].Remove(1,ore);
      board.allPlayers[i].AddDev();
    } 
  }
  #pragma omp section
  {
    if(tradeWo > 1){
      board.allPlayers[i].Remove(3,wood);
      board.allPlayers[i].Remove(1,sheep);
      board.allPlayers[i].Remove(1,ore);
      board.allPlayers[i].AddDev();
    }
  }
  #pragma omp section
  {
    if(sheep1 >= 4){
      board.allPlayers[i].Remove(4,sheep);
      board.allPlayers[i].Remove(1,ore);
      board.allPlayers[i].AddDev();
    }
  }
  #pragma omp section
  {
    if(ore1 >= 4){
      board.allPlayers[i].Remove(4,ore);
      board.allPlayers[i].Remove(1,sheep);
      board.allPlayers[i].AddDev();
    }
  }
}
 }
 if(wheat1 >=1 && ore1 >= 1){
#pragma omp parallel sections
{
  #pragma omp section
  {
    if(tradeB > 1){
      board.allPlayers[i].Remove(3,brick);
      board.allPlayers[i].Remove(1,wheat);
      board.allPlayers[i].Remove(1,ore);
      board.allPlayers[i].AddDev();
    }
  }
  #pragma omp section
  {
    if(tradeWo > 1){
      board.allPlayers[i].Remove(3,wood);
      board.allPlayers[i].Remove(1,wheat);
      board.allPlayers[i].Remove(1,ore);
      board.allPlayers[i].AddDev();
    }
  }
  #pragma omp section
  {
    if(wheat1 >= 4){
      board.allPlayers[i].Remove(4,wheat);
      board.allPlayers[i].Remove(1,ore);
      board.allPlayers[i].AddDev();
    }
  }
  #pragma omp section
  {
    if(ore1 >= 4){
      board.allPlayers[i].Remove(4,ore);
      board.allPlayers[i].Remove(1,sheep);
      board.allPlayers[i].AddDev();
    }
  }
}
 }
#pragma omp parallel sections
{
  #pragma omp section
  {
    if(tradeO >= 1){
      Resource randomResource = (Resource)(rand() %5);
      while(randomResource == ore){
	randomResource = (Resource)(rand() %5);
      }
      trade(i,ore,randomResource);
    }
  }
  #pragma omp section
  {
    if(tradeWh >= 1){
      Resource randomResource = (Resource)(rand() %5);
      while(randomResource == wheat){
	randomResource = (Resource)(rand() %5);
      }
      trade(i,wheat,randomResource);
    }
  }
  #pragma omp section
  {
    if(tradeWo >= 1){
      Resource randomResource = (Resource)(rand() %5);
      while(randomResource == wood){
	randomResource = (Resource)(rand() %5);
      }
      trade(i,wood,randomResource);
    }
  }
  #pragma omp section
  {
    if(tradeS >= 1){
      Resource randomResource = (Resource)(rand() %5);
      while(randomResource == sheep){
	randomResource = (Resource)(rand() %5);
      }
      trade(i,sheep,randomResource);
    }
  }
  #pragma omp section
  {
    if(tradeB >= 1){
      Resource randomResource = (Resource)(rand() %5);
      while(randomResource == brick){
	randomResource = (Resource)(rand() %5);
      }
      trade(i,brick,randomResource);
    }
  }
}//parallel sections
}//for loop over players
}//function run turn

void UseMon(int currentPlayer, Resource r){
  board.allPlayers[currentPlayer].UseDev(monopoly);
  int count=0; //total to add to user
  #pragma omp parallel for
  for(unsigned int i=0; i<board.allPlayers.size(); i++){
    if((int)i != currentPlayer){
      int tempCount=0; //number to take from others
      for(unsigned int j=0; j<board.allPlayers[i].resourceHand.size(); j++){
	if(board.allPlayers[i].resourceHand[j] == r){
	  {
	  count++;
	  }
	  tempCount++;
	}
      }
      board.allPlayers[i].Remove(tempCount,r);
    }
  }
  #pragma omp parallel for
  for(int i=0; i<count; i++){
    board.allPlayers[currentPlayer].resourceHand.push_back(r);
  }
}

void UseYOP(int currentPlayer, Resource r1, Resource r2){
#pragma omp parallel sections
 {
  #pragma omp section
  {
    board.allPlayers[currentPlayer].UseDev(yearOfPlenty);
  }
  #pragma omp section
  {
    board.allPlayers[currentPlayer].resourceHand.push_back(r1);
  }
  #pragma omp section
  {
    board.allPlayers[currentPlayer].resourceHand.push_back(r2);
  }
 }
}

void UseRoad(int currentPlayer){
  board.allPlayers[currentPlayer].UseDev(roadBuilding);
  MakeRoad(currentPlayer);
  MakeRoad(currentPlayer);
}
void MakeRoad(int currentPlayer){
  for(int i=0; i<board.size; i++){
    for(int j=0; j<board.size; j++){
      if(board.board[i][j].owner == currentPlayer){
	if(i != 0 && i != board.size-1 && j != 0 and j != board.size-1){
	  if(board.board[i-1][j].owner == -1 && board.board[i-1][j].type != desert && !board.board[i][j].top.exists){
	    board.PlaceRoad(currentPlayer,i,j,1);
	    return;
	  }
	  if(board.board[i][j+1].owner == -1 && board.board[i][j+1].type != desert && !board.board[i][j].right.exists){
	    board.PlaceRoad(currentPlayer,i,j,2);
	    return;
	  }
	  if(board.board[i+1][j].owner == -1 && board.board[i+1][j].type != desert && !board.board[i][j].bottom.exists){
	    board.PlaceRoad(currentPlayer,i,j,3);
	    return;
	  }
	  if(board.board[i][j-1].owner == -1 && board.board[i][j-1].type != desert && !board.board[i][j].left.exists){
	    board.PlaceRoad(currentPlayer,i,j,0);
	    return;
	  }
	}
      }
    }
  }
}
//dev: true if they used a dev card, false if they rolled 7
void UseKnight(int currentPlayer){
  int winning;
  int winningVP=0;
  board.allPlayers[currentPlayer].UseDev(knight);
  #pragma omp parallel for
  for(unsigned int i=0; i<board.allPlayers.size(); i++){
    if((int)i != currentPlayer && board.allPlayers[i].victoryPoints > winningVP){
      winning =i;
      winningVP = board.allPlayers[i].victoryPoints;
    }
  }
  for(int i=0; i<board.size; i++){
    for(int j=0; j<board.size; j++){
      if(board.board[i][j].owner == winning){
#pragma omp parallel sections
{       
  #pragma omp section
  {
    board.board[board.robberX][board.robberY].hasRobber = false;
    board.board[i][j].hasRobber = true;
    board.robberX = i;
    board.robberY = j;
  }
  #pragma omp section
  {
    board.allPlayers[currentPlayer].armySize++;
    if(board.allPlayers[currentPlayer].armySize > board.biggestArmy){
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
  }
 }
 return;
      }
    }
  }
  cout << "Something went wrong in UseKnight" << endl;
}
bool canBuildCity(int currentPlayer){
  for(unsigned int i=0; i<board.allPlayers[currentPlayer].ownedSquares.size(); i++){
    if(!board.allPlayers[currentPlayer].ownedSquares[i]->hasCity){
      return true;
    }
  }
  return false;
}
bool canBuildTown(int currentPlayer){
  //giant mess of conditions to ensure we don't access memory out of the vector
  bool build = false;
  for(int i=0; i<board.size; i++){
    for(int j=0; j<board.size; j++){
      if(i != 0 && i != board.size-1 && j != 0 && j != board.size-1){
	if(board.board[i][j].owner == currentPlayer){
#pragma omp parallel sections
{
  #pragma omp section
  {
    if(board.board[i][j].top.owner == currentPlayer){
      if(board.board[i][j-1].owner == -1){
	build = true;
      }
    }
  }
  #pragma omp section
  {
    if(board.board[i][j].right.owner == currentPlayer){
      if(board.board[i+1][j].owner == -1){
	build = true;
      }
    }
  }
  #pragma omp section
  {
    if(board.board[i][j].bottom.owner == currentPlayer){
      if(board.board[i][j+1].owner == -1){
	build = true;
      }
    }
  }
  #pragma omp section
  {
    if(board.board[i][j].left.owner == currentPlayer){
      if(board.board[i-1][j].owner == -1){
	build = true;
      }
    }
  }
}
	}//if board = current player
      }
      if(build) return build;
      if(i == 0){
#pragma omp parallel sections
{
  #pragma omp section
  {
    if(j == 0){
      if(board.board[i][j].right.owner == currentPlayer){
	if(board.board[i][j+1].owner == -1){
	  build = true;
	}
      }
      if(board.board[i][j].bottom.owner == currentPlayer){
	if(board.board[i+1][j].owner == -1){
	  build = true;
	}
      }
    }
  }
  #pragma omp section
  {
    if(j == board.size-1){
      if(board.board[i][j].left.owner == currentPlayer){
	if(board.board[i][j-1].owner == -1){
	  build = true;
	}
      }
	  if(board.board[i][j].bottom.owner == currentPlayer){
	    if(board.board[i+1][j].owner == -1){
	      build = true;
	    }
	  }
	}
  }
  #pragma omp section
  {
    if(j != 0 && j != board.size-1){
      if(board.board[i][j].right.owner == currentPlayer){
	if(board.board[i][j+1].owner == -1){
	  build = true;
	}
      }
      if(board.board[i][j].left.owner == currentPlayer){
	if(board.board[i][j-1].owner == -1){
	  build = true;
	}
      }
      if(board.board[i][j].bottom.owner == currentPlayer){
	if(board.board[i+1][j].owner == -1){
	  build = true;
	}
      }
    }
  }
}
      }
      if(build) return build;
      if(i == board.size-1){
#pragma omp parallel sections
{
  #pragma omp section
  {
    if(j == 0){
      if(board.board[i][j].right.owner == currentPlayer){
	if(board.board[i][j+1].owner == -1){
	  build = true;
	}
      }
      if(board.board[i][j].top.owner == currentPlayer){
	if(board.board[i-1][j].owner == -1){
	  build = true;
	}
      }
    }
  }
  #pragma omp section
  {
    if(j == board.size-1){
      if(board.board[i][j].left.owner == currentPlayer){
	if(board.board[i][j-1].owner == -1){
	  build = true;
	}
      }
      if(board.board[i][j].top.owner == currentPlayer){
	if(board.board[i-1][j].owner == -1){
	  build = true;
	}
      }
    }
  }
  #pragma omp section
  {
    if(j != 0 && j != board.size-1){
      if(board.board[i][j].left.owner == currentPlayer){
	if(board.board[i][j-1].owner == -1){
	  build = true;
	}
      }
      if(board.board[i][j].top.owner == currentPlayer){
	if(board.board[i-1][j].owner == -1){
	  build = true;
	}
      }
      if(board.board[i][j].right.owner == currentPlayer){
	if(board.board[i][j+1].owner == -1){
	  build = true;
	}
      }
    }
  }
}
      }
#pragma omp parallel sections
{
  #pragma omp section
  {
    if(j == 0){
      if(i == 0){
	if(board.board[i][j].right.owner == currentPlayer){
	  if(board.board[i][j+1].owner == -1){
	    build = true;
	  }
	}
	if(board.board[i][j].bottom.owner == currentPlayer){
	  if(board.board[i+1][j].owner == -1){
	  build = true;
	  }
	}
      } else if(i == board.size-1){
	if(board.board[i][j].right.owner == currentPlayer){
	  if(board.board[i][j+1].owner == -1){
	    build = true;
	  }
	}
	if(board.board[i][j].top.owner == currentPlayer){
	  if(board.board[i-1][j].owner == -1){
	    build = true;
	  }
	}
      } else {
	if(board.board[i][j].right.owner == currentPlayer){
	    if(board.board[i][j+1].owner == -1){
	  build = true;
	    }
	  }
	  if(board.board[i][j].top.owner == currentPlayer){
	    if(board.board[i-1][j].owner == -1){
	  build = true;
	    }
	  }
	  if(board.board[i][j].bottom.owner == currentPlayer){
	    if(board.board[i+1][j].owner == -1){
	  build = true;
	    }
	  }
	}
    }
  }
  #pragma omp section
  {
    if(j == board.size-1){
      if(i == 0){
	if(board.board[i][j].bottom.owner == currentPlayer){
	  if(board.board[i+1][j].owner == -1){
	    build = true;
	  }
	}
	if(board.board[i][j].left.owner == currentPlayer){
	  if(board.board[i][j-1].owner == -1){
	    build = true;
	  }
	}
      } else if(i == board.size-1){
	if(board.board[i][j].top.owner == currentPlayer){
	  if(board.board[i-1][j].owner == -1){
	    build = true;
	  }
	}
	if(board.board[i][j].left.owner == currentPlayer){
	  if(board.board[i][j-1].owner == -1){
	    build = true;
	  }
	}
      } else {
	if(board.board[i][j].left.owner == currentPlayer){
	  if(board.board[i][j-1].owner == -1){
	    build = true;
	  }
	}
	if(board.board[i][j].top.owner == currentPlayer){
	  if(board.board[i-1][j].owner == -1){
	    build = true;
	  }
	}
	if(board.board[i][j].bottom.owner == currentPlayer){
	  if(board.board[i+1][j].owner == -1){
	    build = true;
	  }
	}
      }
    }
  }
}
if(build) return build;
}//inner loop
if(build) return build;
}//outer loop
 if(build) return build;
 return false;
}   
void trade(int currentPlayer, Resource r1, Resource r2){
  //trade 3 r1 for 1 r2
  board.allPlayers[currentPlayer].Remove(3,r1);
  board.allPlayers[currentPlayer].resourceHand.push_back(r2);
}
int cityProg(int currentPlayer){
  int brick1=0, wheat1=0;
  //can do this loop in parallel but it results in a slowdown
  for(unsigned int i=0; i<board.allPlayers[currentPlayer].resourceHand.size(); i++){
#pragma omp parallel sections
{
  #pragma omp section
  {
    if(board.allPlayers[currentPlayer].resourceHand[i] == wheat){
      if(wheat1 < 2){
	wheat1++;
      }
    } 
  }
  #pragma omp section
  {
    if(board.allPlayers[currentPlayer].resourceHand[i] == brick){
      if(brick1 <3){
	brick1++;
      }
    }
  }
}
  }
  return brick1+wheat1;
}
int townProg(int currentPlayer){
  int brick1=0, wheat1=0, sheep1=0, wood1=0;
  for(unsigned int i=0; i<board.allPlayers[currentPlayer].resourceHand.size(); i++){
#pragma omp parallel sections
{
  #pragma omp section
  {
    if(board.allPlayers[currentPlayer].resourceHand[i] == wheat){
      if(wheat1 < 1){
	wheat1++;
      }
    }
  }
  #pragma omp section
  {
    if(board.allPlayers[currentPlayer].resourceHand[i] == brick){
      if(brick1 <1){
	brick1++;
      }
    }
  }
  #pragma omp section
  {
    if(board.allPlayers[currentPlayer].resourceHand[i] == sheep){
      if(sheep1 <1){
	sheep1++;
      }
    }
  }
  #pragma omp section
  {
    if(board.allPlayers[currentPlayer].resourceHand[i] == wood){
      if(wood1 <1){
	wood1++;
      }
    }
  }
}
  }
  return brick1+wheat1+wood1+sheep1;
}
int roadProg(int currentPlayer){
  int brick1=0, wood1=0;
  for(unsigned int i=0; i<board.allPlayers[currentPlayer].resourceHand.size(); i++){
#pragma omp parallel sections
{
  #pragma omp section
  {
    if(board.allPlayers[currentPlayer].resourceHand[i] == wood){
      if(wood1 < 1){
	wood1++;
      }
    } 
  }
  #pragma omp section
  {
    if(board.allPlayers[currentPlayer].resourceHand[i] == brick){
      if(brick1 <1){
	brick1++;
      }
    }
  }
}
//we can build a road no need to continue loop
 if((brick1+wood1) == 2) return brick1+wood1;
  }
  return brick1+wood1;
}
void BuildCity(int currentPlayer){
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
void BuildTown(int currentPlayer){
  //giant mess of conditions to ensure we don't access memory out of the vector
  for(int i=0; i<board.size; i++){
    for(int j=0; j<board.size; j++){
      if(i != 0 && i != board.size-1 && j != 0 and j != board.size-1){
	if(board.board[i][j].owner == currentPlayer){
	  //cant make parallel or multiple theads could try and make towns
	  if(board.board[i][j].top.owner == currentPlayer){
	    if(board.board[i][j-1].owner == -1 && board.board[i][j-1].type != desert){
	      board.allPlayers[currentPlayer].ownedSquares.push_back(&board.board[i][j-1]);
	      board.board[i][j-1].owner = currentPlayer;
	      return;
	    }
	  }
	  if(board.board[i][j].right.owner == currentPlayer){
	    if(board.board[i][j+1].owner == -1 && board.board[i][j+1].type != desert){
	      board.allPlayers[currentPlayer].ownedSquares.push_back(&board.board[i][j+1]);
	      board.board[i][j+1].owner = currentPlayer;
	      return;
	    }
	  }
	  if(board.board[i][j].bottom.owner == currentPlayer){
	    if(board.board[i+1][j].owner == -1 && board.board[i+1][j].type != desert){
	      board.allPlayers[currentPlayer].ownedSquares.push_back(&board.board[i+1][j]);
	      board.board[i+1][j].owner = currentPlayer;
	      return;
	    }
	  }
	  if(board.board[i][j].left.owner == currentPlayer){
	    if(board.board[i][j-1].owner == -1 && board.board[i][j-1].type != desert){
	      board.allPlayers[currentPlayer].ownedSquares.push_back(&board.board[i][j-1]);
	      board.board[i][j-1].owner = currentPlayer;
	      return;
	    }
	  }
	}
      } else if(i == 0){
	if(j == 0){
	  if(board.board[i][j].right.owner == currentPlayer){
	    if(board.board[i][j+1].owner == -1 && board.board[i][j+1].type != desert){
	      board.allPlayers[currentPlayer].ownedSquares.push_back(&board.board[i][j+1]);
	      board.board[i][j+1].owner = currentPlayer;
	      return;
	    }
	  }
	  if(board.board[i][j].bottom.owner == currentPlayer){
	    if(board.board[i+1][j].owner == -1 && board.board[i+1][j].type != desert){
	      board.allPlayers[currentPlayer].ownedSquares.push_back(&board.board[i+1][j]);
	      board.board[i+1][j].owner = currentPlayer;
	      return;
	    }
	  }
	} else if(j == board.size-1){
	  if(board.board[i][j].left.owner == currentPlayer){
	    if(board.board[i][j-1].owner == -1 && board.board[i][j-1].type != desert){
	      board.allPlayers[currentPlayer].ownedSquares.push_back(&board.board[i][j-1]);
	      board.board[i][j-1].owner = currentPlayer;
	      return;
	    }
	  }
	  if(board.board[i][j].bottom.owner == currentPlayer){
	    if(board.board[i+1][j].owner == -1 && board.board[i+1][j].type != desert){
	      board.allPlayers[currentPlayer].ownedSquares.push_back(&board.board[i+1][j]);
	      board.board[i+1][j].owner = currentPlayer;
	      return;
	    }
	  }
	} else {
	  if(board.board[i][j].right.owner == currentPlayer){
	    if(board.board[i][j+1].owner == -1 && board.board[i][j+1].type != desert){
	      board.allPlayers[currentPlayer].ownedSquares.push_back(&board.board[i][j+1]);
	      board.board[i][j+1].owner = currentPlayer;
	      return;
	    }
	  }
	  if(board.board[i][j].left.owner == currentPlayer){
	    if(board.board[i][j-1].owner == -1 && board.board[i][j-1].type != desert){
	      board.allPlayers[currentPlayer].ownedSquares.push_back(&board.board[i][j-1]);
	      board.board[i][j-1].owner = currentPlayer;
	      return;
	    }
	  }
	  if(board.board[i][j].bottom.owner == currentPlayer){
	    if(board.board[i+1][j].owner == -1 && board.board[i+1][j].type != desert){
	      board.allPlayers[currentPlayer].ownedSquares.push_back(&board.board[i+1][j]);
	      board.board[i+1][j].owner = currentPlayer;
	      return;
	    }
	  }
	}
      } else if(i == board.size-1){
	if(j == 0){
	  if(board.board[i][j].right.owner == currentPlayer){
	    if(board.board[i][j+1].owner == -1 && board.board[i][j+1].type != desert){
	      board.allPlayers[currentPlayer].ownedSquares.push_back(&board.board[i][j+1]);
	      board.board[i][j+1].owner = currentPlayer;
	      return;
	    }
	  }
	  if(board.board[i][j].top.owner == currentPlayer){
	    if(board.board[i-1][j].owner == -1 && board.board[i-1][j].type != desert){
	      board.allPlayers[currentPlayer].ownedSquares.push_back(&board.board[i-1][j]);
	      board.board[i-1][j].owner = currentPlayer;
	      return;
	    }
	  }
	} else if(j == board.size-1){
	  if(board.board[i][j].left.owner == currentPlayer){
	    if(board.board[i][j-1].owner == -1 && board.board[i][j-1].type != desert){
	      board.allPlayers[currentPlayer].ownedSquares.push_back(&board.board[i][j-1]);
	      board.board[i][j-1].owner = currentPlayer;
	      return;
	    }
	  }
	  if(board.board[i][j].top.owner == currentPlayer){
	    if(board.board[i-1][j].owner == -1 && board.board[i-1][j].type != desert){
	      board.allPlayers[currentPlayer].ownedSquares.push_back(&board.board[i-1][j]);
	      board.board[i-1][j].owner = currentPlayer;
	      return;
	    }
	 }
	} else {
	  if(board.board[i][j].left.owner == currentPlayer){
	    if(board.board[i][j-1].owner == -1 && board.board[i][j-1].type != desert){
	      board.allPlayers[currentPlayer].ownedSquares.push_back(&board.board[i][j-1]);
	      board.board[i][j-1].owner = currentPlayer;
	      return;
	    }
	  }
	  if(board.board[i][j].top.owner == currentPlayer){
	    if(board.board[i-1][j].owner == -1 && board.board[i-1][j].type != desert){
	      board.allPlayers[currentPlayer].ownedSquares.push_back(&board.board[i-1][j]);
	      board.board[i-1][j].owner = currentPlayer;
	      return;
	    }
	  }
	  if(board.board[i][j].right.owner == currentPlayer){
	    if(board.board[i][j+1].owner == -1 && board.board[i][j+1].type != desert){
	      board.allPlayers[currentPlayer].ownedSquares.push_back(&board.board[i][j+1]);
	      board.board[i][j+1].owner = currentPlayer;
	      return;
	    }
	  }
	}
      } else if(j == 0){
	if(i == 0){
	  if(board.board[i][j].right.owner == currentPlayer){
	    if(board.board[i][j+1].owner == -1 && board.board[i][j+1].type != desert){
	      board.allPlayers[currentPlayer].ownedSquares.push_back(&board.board[i][j+1]);
	      board.board[i][j+1].owner = currentPlayer;
	      return;
	    }
	  }
	  if(board.board[i][j].bottom.owner == currentPlayer){
	    if(board.board[i+1][j].owner == -1 && board.board[i+1][j].type != desert){
	      board.allPlayers[currentPlayer].ownedSquares.push_back(&board.board[i+1][j]);
	      board.board[i+1][j].owner = currentPlayer;
	      return;
	    }
	  }
	} else if(i == board.size-1){
	  if(board.board[i][j].right.owner == currentPlayer){
	    if(board.board[i][j+1].owner == -1 && board.board[i][j+1].type != desert){
	      board.allPlayers[currentPlayer].ownedSquares.push_back(&board.board[i][j+1]);
	      board.board[i][j+1].owner = currentPlayer;
	      return;
	    }
	  }
	  if(board.board[i][j].top.owner == currentPlayer){
	    if(board.board[i-1][j].owner == -1 && board.board[i-1][j].type != desert){
	      board.allPlayers[currentPlayer].ownedSquares.push_back(&board.board[i-1][j]);
	      board.board[i-1][j].owner = currentPlayer;
	      return;
	    }
	  }
	} else {
	  if(board.board[i][j].right.owner == currentPlayer){
	    if(board.board[i][j+1].owner == -1 && board.board[i][j+1].type != desert){
	      board.allPlayers[currentPlayer].ownedSquares.push_back(&board.board[i][j+1]);
	      board.board[i][j+1].owner = currentPlayer;
	      return;
	    }
	  }
	  if(board.board[i][j].top.owner == currentPlayer){
	    if(board.board[i-1][j].owner == -1 && board.board[i-1][j].type != desert){
	      board.allPlayers[currentPlayer].ownedSquares.push_back(&board.board[i-1][j]);
	      board.board[i-1][j].owner = currentPlayer;
	      return;
	    }
	  }
	  if(board.board[i][j].bottom.owner == currentPlayer){
	    if(board.board[i+1][j].owner == -1 && board.board[i+1][j].type != desert){
	      board.allPlayers[currentPlayer].ownedSquares.push_back(&board.board[i+1][j]);
	      board.board[i+1][j].owner = currentPlayer;
	      return;
	    }
	  }
	}
      } else if(j == board.size-1){
	if(i == 0){
	  if(board.board[i][j].bottom.owner == currentPlayer){
	    if(board.board[i+1][j].owner == -1 && board.board[i+1][j].type != desert){
	      board.allPlayers[currentPlayer].ownedSquares.push_back(&board.board[i+1][j]);
	      board.board[i+1][j].owner = currentPlayer;
	      return;
	    }
	  }
	  if(board.board[i][j].left.owner == currentPlayer){
	    if(board.board[i][j-1].owner == -1 && board.board[i][j-1].type != desert){
	      board.allPlayers[currentPlayer].ownedSquares.push_back(&board.board[i][j-1]);
	      board.board[i][j-1].owner = currentPlayer;
	      return;
	    }
	  }
	} else if(i == board.size-1){
	  if(board.board[i][j].top.owner == currentPlayer){
	    if(board.board[i-1][j].owner == -1 && board.board[i-1][j].type != desert){
	      board.allPlayers[currentPlayer].ownedSquares.push_back(&board.board[i-1][j]);
	      board.board[i-1][j].owner = currentPlayer;
	      return;
	    }
	  }
	  if(board.board[i][j].left.owner == currentPlayer){
	    if(board.board[i][j-1].owner == -1 && board.board[i][j-1].type != desert){
	      board.allPlayers[currentPlayer].ownedSquares.push_back(&board.board[i][j-1]);
	      board.board[i][j-1].owner = currentPlayer;
	      return;
	    }
	  }
	} else {
	  if(board.board[i][j].left.owner == currentPlayer){
	    if(board.board[i][j-1].owner == -1 && board.board[i][j-1].type != desert){
	      board.allPlayers[currentPlayer].ownedSquares.push_back(&board.board[i][j-1]);
	      board.board[i][j-1].owner = currentPlayer;
	      return;
	    }
	  }
	  if(board.board[i][j].top.owner == currentPlayer){
	    if(board.board[i-1][j].owner == -1 && board.board[i-1][j].type != desert){
	      board.allPlayers[currentPlayer].ownedSquares.push_back(&board.board[i-1][j]);
	      board.board[i-1][j].owner = currentPlayer;
	      return;
	    }
	  }
	  if(board.board[i][j].bottom.owner == currentPlayer){
	    if(board.board[i+1][j].owner == -1 && board.board[i+1][j].type != desert){
	      board.allPlayers[currentPlayer].ownedSquares.push_back(&board.board[i+1][j]);
	      board.board[i+1][j].owner = currentPlayer;
	      return;
	    }
	  }
	}
      }
    }
  }
  return;
}
