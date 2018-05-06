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
#include <mpi.h> //ol reliable
using namespace std;

#include "Board.h"
#include "Player.h"

// global variables for ease of use
// the board that will be used
Board board =Board(100);
bool pause1;
int my_rank,p,q;
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
void sendBoard();
void recvBoard();
void bcastPlayers();
void bcastPlayers();
void sendPlayer(int);
void recvPlayers(int);
int main(int argc, char *argv[]){
  if (argc != 3 && argc != 4){
      cout << "\nNot enough command-line arguments\n" << endl;
      return -1;
  } else {
    MPI_Init(NULL,NULL);
    MPI_Comm_size(MPI_COMM_WORLD, &p);
    MPI_Comm_rank(MPI_COMM_WORLD, &my_rank);
    // convert the size of the board to int
    // https://stackoverflow.com/questions/2797813/how-to-convert-a-command-line-argument-to-int
    istringstream ss(argv[1]);
    int size;
    pause1 = false;
    if (!(ss >> size)) cerr << "Invalid number " << argv[1] << '\n';
    board = Board(size);
    // convert the player number to an int
    istringstream ss2(argv[2]);
    int playerNum;
    if (!(ss2 >> playerNum)) cerr << "Invalid number " << argv[2] << '\n';
    // need to create the board
    if(my_rank==0){
      board.MakeBoard(size);
      sendBoard(); //board object cant be sent at once
    } else {
      recvBoard();
    }
    // create a vector of all the players
    //all thread can create the players since there is no rng
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
    if(my_rank==0){
    // holds whether or not to end the game
    bool done = false;
    // runs a single turn, forever in the while loop until we have a winner
    while (!done){
      //cin >> rank;
      //MPI_Barrier(MPI_COMM_WORLD);
      RunTurn();
      // iterate through the players and see if any of them won
      if(my_rank==0){
	for (unsigned int i = 0; i < board.allPlayers.size(); i++){
	  if (board.allPlayers[i].victoryPoints >= 10){
	    done = true;
	    cout << "\nPlayer " << i << " has won!\n";
	    board.PrintBoard();
	  }
	}
      }
    }
    // clean up any memory allocation from the board
    }
    MPI_Barrier(MPI_COMM_WORLD);
    board.CleanupBoard();
    MPI_Finalize();
    return 0;
  }
}

void RunFirstTurn(void)
{
  // each player puts down a settlement and a road
  int rank;
  MPI_Comm_rank(MPI_COMM_WORLD, &rank);
  //cerr << rank << endl;
  for (unsigned int i = 0; i < board.allPlayers.size(); i++){
    // current player gets to place a house and a road
    board.PlaceHouse(i,-1,-1);
    //-1 signifies a first turn placement
    board.PlaceRoad(i,-1,-1,-1);
  }
  // now do the same thing again
  for (unsigned int i = 0; i < board.allPlayers.size(); i++){
    // current player gets to place a house and a road
    board.PlaceHouse(i,-1,-1);
    board.PlaceRoad(i,-1,-1,-1);
  }
  board.PrintBoard();
}

void RunTurn(void){
  int rank;
  MPI_Comm_rank(MPI_COMM_WORLD, &rank);
  // run a turn for each person in the game
  for (unsigned int i = 0; i < board.allPlayers.size(); i++){
    Player* p = &board.allPlayers[i];
    p->Print(pause1);
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
    if(build >= 1){
      UseRoad(i);
    }
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
    if(!settlement && !city && !road && dev){
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
    if(tradeO >= 1){
      Resource randomResource = (Resource)(rand() %5);
      trade(i,ore,randomResource);
    }
    if(tradeWh >= 1){
      Resource randomResource = (Resource)(rand() %5);
      trade(i,wheat,randomResource);
    }
    if(tradeWo >= 1){
      Resource randomResource = (Resource)(rand() %5);
      trade(i,wood,randomResource);
    }
    if(tradeS >= 1){
      Resource randomResource = (Resource)(rand() %5);
      trade(i,sheep,randomResource);
    }
    if(tradeB >= 1){
      Resource randomResource = (Resource)(rand() %5);
      trade(i,brick,randomResource);
    }
  }
}
void UseMon(int currentPlayer, Resource r){
  //cout << "mon" << endl;
  board.allPlayers[currentPlayer].UseDev(monopoly);
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
  board.allPlayers[currentPlayer].UseDev(yearOfPlenty);
  board.allPlayers[currentPlayer].resourceHand.push_back(r1);
  board.allPlayers[currentPlayer].resourceHand.push_back(r2);
}

void UseRoad(int currentPlayer){
  board.allPlayers[currentPlayer].UseDev(roadBuilding);
  //roadBuilding dev card makes 2 roads
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
  for(unsigned int i=0; i<board.allPlayers[currentPlayer].ownedSquares.size(); i++){
    if(!board.allPlayers[currentPlayer].ownedSquares[i]->hasCity){
      return true;
    }
  }
  return false;
}
bool canBuildTown(int currentPlayer){
  //giant mess of conditions to ensure we don't access memory out of the vector
  for(int i=0; i<board.size; i++){
    for(int j=0; j<board.size; j++){
      if(i != 0 && i != board.size-1 && j != 0 and j != board.size-1){
	if(board.board[i][j].owner == currentPlayer){
	  if(board.board[i][j].top.owner == currentPlayer){
	    if(board.board[i][j-1].owner == -1){
	      return true;
	    }
	  }
	  if(board.board[i][j].right.owner == currentPlayer){
	    if(board.board[i+1][j].owner == -1){
	      return true;
	    }
	  }
	  if(board.board[i][j].bottom.owner == currentPlayer){
	    if(board.board[i][j+1].owner == -1){
	      return true;
	    }
	  }
	  if(board.board[i][j].left.owner == currentPlayer){
	    if(board.board[i-1][j].owner == -1){
	      return true;
	    }
	  }
	}
      } else if(i == 0){
	if(j == 0){
	  if(board.board[i][j].right.owner == currentPlayer){
	    if(board.board[i][j+1].owner == -1){
	      return true;
	    }
	  }
	  if(board.board[i][j].bottom.owner == currentPlayer){
	    if(board.board[i+1][j].owner == -1){
	      return true;
	    }
	  }
	} else if(j == board.size-1){
	  if(board.board[i][j].left.owner == currentPlayer){
	    if(board.board[i][j-1].owner == -1){
	      return true;
	    }
	  }
	  if(board.board[i][j].bottom.owner == currentPlayer){
	    if(board.board[i+1][j].owner == -1){
	      return true;
	    }
	  }
	} else {
	  if(board.board[i][j].right.owner == currentPlayer){
	    if(board.board[i][j+1].owner == -1){
	      return true;
	    }
	  }
	  if(board.board[i][j].left.owner == currentPlayer){
	    if(board.board[i][j-1].owner == -1){
	      return true;
	    }
	  }
	  if(board.board[i][j].bottom.owner == currentPlayer){
	    if(board.board[i+1][j].owner == -1){
	      return true;
	    }
	  }
	}
      } else if(i == board.size-1){
	if(j == 0){
	  if(board.board[i][j].right.owner == currentPlayer){
	    if(board.board[i][j+1].owner == -1){
	      return true;
	    }
	  }
	  if(board.board[i][j].top.owner == currentPlayer){
	    if(board.board[i-1][j].owner == -1){
	      return true;
	    }
	  }
	} else if(j == board.size-1){
	  if(board.board[i][j].left.owner == currentPlayer){
	    if(board.board[i][j-1].owner == -1){
	      return true;
	    }
	  }
	  if(board.board[i][j].top.owner == currentPlayer){
	    if(board.board[i-1][j].owner == -1){
	      return true;
	    }
	  }
	} else {
	  if(board.board[i][j].left.owner == currentPlayer){
	    if(board.board[i][j-1].owner == -1){
	      return true;
	    }
	  }
	  if(board.board[i][j].top.owner == currentPlayer){
	    if(board.board[i-1][j].owner == -1){
	      return true;
	    }
	  }
	  if(board.board[i][j].right.owner == currentPlayer){
	    if(board.board[i][j+1].owner == -1){
	      return true;
	    }
	  }
	}
      } else if(j == 0){
	if(i == 0){
	  if(board.board[i][j].right.owner == currentPlayer){
	    if(board.board[i][j+1].owner == -1){
	      return true;
	    }
	  }
	  if(board.board[i][j].bottom.owner == currentPlayer){
	    if(board.board[i+1][j].owner == -1){
	      return true;
	    }
	  }
	} else if(i == board.size-1){
	  if(board.board[i][j].right.owner == currentPlayer){
	    if(board.board[i][j+1].owner == -1){
	      return true;
	    }
	  }
	  if(board.board[i][j].top.owner == currentPlayer){
	    if(board.board[i-1][j].owner == -1){
	      return true;
	    }
	  }
	} else {
	  if(board.board[i][j].right.owner == currentPlayer){
	    if(board.board[i][j+1].owner == -1){
	      return true;
	    }
	  }
	  if(board.board[i][j].top.owner == currentPlayer){
	    if(board.board[i-1][j].owner == -1){
	      return true;
	    }
	  }
	  if(board.board[i][j].bottom.owner == currentPlayer){
	    if(board.board[i+1][j].owner == -1){
	      return true;
	    }
	  }
	}
      } else if(j == board.size-1){
	if(i == 0){
	  if(board.board[i][j].bottom.owner == currentPlayer){
	    if(board.board[i+1][j].owner == -1){
	      return true;
	    }
	  }
	  if(board.board[i][j].left.owner == currentPlayer){
	    if(board.board[i][j-1].owner == -1){
	      return true;
	    }
	  }
	} else if(i == board.size-1){
	  if(board.board[i][j].top.owner == currentPlayer){
	    if(board.board[i-1][j].owner == -1){
	      return true;
	    }
	  }
	  if(board.board[i][j].left.owner == currentPlayer){
	    if(board.board[i][j-1].owner == -1){
	      return true;
	    }
	  }
	} else {
	  if(board.board[i][j].left.owner == currentPlayer){
	    if(board.board[i][j-1].owner == -1){
	      return true;
	    }
	  }
	  if(board.board[i][j].top.owner == currentPlayer){
	    if(board.board[i-1][j].owner == -1){
	      return true;
	    }
	  }
	  if(board.board[i][j].bottom.owner == currentPlayer){
	    if(board.board[i+1][j].owner == -1){
	      return true;
	    }
	  }
	}
      }
    }
  }
  return false;
}
void trade(int currentPlayer, Resource r1, Resource r2){
  //trade 3 r1 for 1 r2
  board.allPlayers[currentPlayer].Remove(3,r1);
  board.allPlayers[currentPlayer].resourceHand.push_back(r2);
}
int cityProg(int currentPlayer){
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
  //cout << "Unable to build city no spaces available" << endl;
}
void BuildTown(int currentPlayer){
  //giant mess of conditions to ensure we don't access memory out of the vector
  for(int i=0; i<board.size; i++){
    for(int j=0; j<board.size; j++){
      if(i != 0 && i != board.size-1 && j != 0 and j != board.size-1){
	if(board.board[i][j].owner == currentPlayer){
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
void sendBoard(){
  int tempInt=0;
  for(int i=0; i<board.size; i++){
    for(int j=0; j<board.size; j++){
      tempInt = (int)board.board[i][j].top.exists;
      MPI_Bcast(&tempInt,1, MPI_INT, 0, MPI_COMM_WORLD);
      tempInt = board.board[i][j].top.owner;
      MPI_Bcast(&tempInt,1, MPI_INT, 0, MPI_COMM_WORLD);
      tempInt = (int)board.board[i][j].right.exists;
      MPI_Bcast(&tempInt,1, MPI_INT, 0, MPI_COMM_WORLD);
      tempInt = board.board[i][j].right.owner;
      MPI_Bcast(&tempInt,1, MPI_INT, 0, MPI_COMM_WORLD);
      tempInt = (int)board.board[i][j].bottom.exists;
      MPI_Bcast(&tempInt,1, MPI_INT, 0, MPI_COMM_WORLD);
      tempInt = board.board[i][j].bottom.owner;
      MPI_Bcast(&tempInt,1, MPI_INT, 0, MPI_COMM_WORLD);
      tempInt = (int)board.board[i][j].left.exists;
      MPI_Bcast(&tempInt,1, MPI_INT, 0, MPI_COMM_WORLD);
      tempInt = board.board[i][j].left.owner;
      MPI_Bcast(&tempInt,1, MPI_INT, 0, MPI_COMM_WORLD);
      tempInt = (int)board.board[i][j].hasTown;
      MPI_Bcast(&tempInt,1, MPI_INT, 0, MPI_COMM_WORLD);
      tempInt = (int)board.board[i][j].hasCity;
      MPI_Bcast(&tempInt,1, MPI_INT, 0, MPI_COMM_WORLD);
      tempInt = board.board[i][j].type;
      MPI_Bcast(&tempInt,1, MPI_INT, 0, MPI_COMM_WORLD);
      tempInt = board.board[i][j].number;
      MPI_Bcast(&tempInt,1, MPI_INT, 0, MPI_COMM_WORLD);
      tempInt = (int)board.board[i][j].hasRobber;
      MPI_Bcast(&tempInt,1, MPI_INT, 0, MPI_COMM_WORLD);
    }
  }
  tempInt = board.size;
  MPI_Bcast(&tempInt,1, MPI_INT, 0, MPI_COMM_WORLD);
  tempInt = board.robberX;
  MPI_Bcast(&tempInt,1, MPI_INT, 0, MPI_COMM_WORLD);
  tempInt = board.robberY;
  MPI_Bcast(&tempInt,1, MPI_INT, 0, MPI_COMM_WORLD);
  tempInt = board.biggestArmy;
  MPI_Bcast(&tempInt,1, MPI_INT, 0, MPI_COMM_WORLD);
  tempInt = board.biggestRoad;
  MPI_Bcast(&tempInt,1, MPI_INT, 0, MPI_COMM_WORLD);
  tempInt = board.biggestArmyOwner;
  MPI_Bcast(&tempInt,1, MPI_INT, 0, MPI_COMM_WORLD);
  tempInt = board.biggestRoadOwner;
  MPI_Bcast(&tempInt,1, MPI_INT, 0, MPI_COMM_WORLD);

}
void recvBoard(){
  int tempInt=0;
  for(int i=0; i<board.size; i++){
    for(int j=0; j<board.size; j++){
      MPI_Bcast(&tempInt,1, MPI_INT, 0, MPI_COMM_WORLD);
      board.board[i][j].top.exists = (bool)tempInt;
      MPI_Bcast(&tempInt,1, MPI_INT, 0, MPI_COMM_WORLD);
      board.board[i][j].top.owner = tempInt;
      MPI_Bcast(&tempInt,1, MPI_INT, 0, MPI_COMM_WORLD);
      board.board[i][j].right.exists = (bool)tempInt;
      MPI_Bcast(&tempInt,1, MPI_INT, 0, MPI_COMM_WORLD);
      board.board[i][j].right.owner = tempInt;
      MPI_Bcast(&tempInt,1, MPI_INT, 0, MPI_COMM_WORLD);
      board.board[i][j].bottom.exists = (bool)tempInt;
      MPI_Bcast(&tempInt,1, MPI_INT, 0, MPI_COMM_WORLD);
      board.board[i][j].bottom.owner = tempInt;
      MPI_Bcast(&tempInt,1, MPI_INT, 0, MPI_COMM_WORLD);
      board.board[i][j].left.exists = (bool)tempInt;
      MPI_Bcast(&tempInt,1, MPI_INT, 0, MPI_COMM_WORLD);
      board.board[i][j].left.owner = tempInt;
      MPI_Bcast(&tempInt,1, MPI_INT, 0, MPI_COMM_WORLD);
      board.board[i][j].hasTown = (bool)tempInt;
      MPI_Bcast(&tempInt,1, MPI_INT, 0, MPI_COMM_WORLD);
      board.board[i][j].hasCity = (bool)tempInt;
      MPI_Bcast(&tempInt,1, MPI_INT, 0, MPI_COMM_WORLD);
      board.board[i][j].type = (Resource)tempInt;
      MPI_Bcast(&tempInt,1, MPI_INT, 0, MPI_COMM_WORLD);
      board.board[i][j].number = tempInt;
      MPI_Bcast(&tempInt,1, MPI_INT, 0, MPI_COMM_WORLD);
      board.board[i][j].hasRobber = (bool)tempInt;
    }
  }
  MPI_Bcast(&tempInt,1, MPI_INT, 0, MPI_COMM_WORLD);
  tempInt = board.size;
  MPI_Bcast(&tempInt,1, MPI_INT, 0, MPI_COMM_WORLD);
  tempInt = board.robberX;
  MPI_Bcast(&tempInt,1, MPI_INT, 0, MPI_COMM_WORLD);
  tempInt = board.robberY;
  MPI_Bcast(&tempInt,1, MPI_INT, 0, MPI_COMM_WORLD);
  tempInt = board.biggestArmy;
  MPI_Bcast(&tempInt,1, MPI_INT, 0, MPI_COMM_WORLD);
  tempInt = board.biggestRoad;
  MPI_Bcast(&tempInt,1, MPI_INT, 0, MPI_COMM_WORLD);
  tempInt = board.biggestArmyOwner;
  MPI_Bcast(&tempInt,1, MPI_INT, 0, MPI_COMM_WORLD);
  tempInt = board.biggestRoadOwner;
}
void bcastPlayersS(){
  int tempInt=0;
  for(unsigned int i=0; i<board.allPlayers.size(); i++){
    tempInt = board.allPlayers[i].resourceHand.size();
    MPI_Bcast(&tempInt,1, MPI_INT, 0, MPI_COMM_WORLD);
    for(unsigned int j=0; j<board.allPlayers[i].resourceHand.size(); j++){
      tempInt = board.allPlayers[i].resourceHand[j];
      MPI_Bcast(&tempInt,1, MPI_INT, 0, MPI_COMM_WORLD);
    }
    tempInt = board.allPlayers[i].developmentHand.size();
    MPI_Bcast(&tempInt,1, MPI_INT, 0, MPI_COMM_WORLD);
    for(unsigned int j=0; j<board.allPlayers[i].developmentHand.size(); j++){
      tempInt = board.allPlayers[i].developmentHand[j];
      MPI_Bcast(&tempInt,1, MPI_INT, 0, MPI_COMM_WORLD);
    }
    tempInt = board.allPlayers[i].ownedSquares.size();
    MPI_Bcast(&tempInt,1, MPI_INT, 0, MPI_COMM_WORLD);
    for(unsigned int j=0; j<board.allPlayers[i].ownedSquares.size(); j++){
      tempInt = board.allPlayers[i].ownedSquares[j]->id;
      MPI_Bcast(&tempInt,1, MPI_INT, 0, MPI_COMM_WORLD);
    }
    tempInt = board.allPlayers[i].playerID;
    MPI_Bcast(&tempInt,1, MPI_INT, 0, MPI_COMM_WORLD);
    tempInt = board.allPlayers[i].armySize;
    MPI_Bcast(&tempInt,1, MPI_INT, 0, MPI_COMM_WORLD);
    tempInt = board.allPlayers[i].roadSize;
    MPI_Bcast(&tempInt,1, MPI_INT, 0, MPI_COMM_WORLD);
    tempInt = board.allPlayers[i].victoryPoints;
    MPI_Bcast(&tempInt,1, MPI_INT, 0, MPI_COMM_WORLD);
  }
}
void bcastPlayersR(){
  int tempInt1=0;
  int tempInt2=0;
  for(unsigned int i=0; i<board.allPlayers.size(); i++){
    MPI_Bcast(&tempInt1,1, MPI_INT, 0, MPI_COMM_WORLD);
    board.allPlayers[i].resourceHand.clear();
    for(int j=0; j<tempInt1; j++){
      MPI_Bcast(&tempInt2,1, MPI_INT, 0, MPI_COMM_WORLD);
      board.allPlayers[i].resourceHand.push_back((Resource)tempInt2);
    }
    MPI_Bcast(&tempInt1,1, MPI_INT, 0, MPI_COMM_WORLD);
    board.allPlayers[i].developmentHand.clear();
    for(int j=0; j<tempInt1; j++){
      MPI_Bcast(&tempInt2,1, MPI_INT, 0, MPI_COMM_WORLD);
      board.allPlayers[i].developmentHand.push_back((DevelopmentCard)tempInt2);
    }
    MPI_Bcast(&tempInt1,1, MPI_INT, 0, MPI_COMM_WORLD);
    board.allPlayers[i].ownedSquares.clear();
    for(int j=0; j<tempInt1; j++){
      MPI_Bcast(&tempInt2,1, MPI_INT, 0, MPI_COMM_WORLD);
      for(int a=0; a<board.size; a++){
	for(int b=0; b<board.size; b++){
	  if(board.board[a][b].id == tempInt2){
	    board.allPlayers[i].ownedSquares.push_back(&board.board[a][b]);
	  }
	}
      }
    }    
    MPI_Bcast(&tempInt1,1, MPI_INT, 0, MPI_COMM_WORLD);
    board.allPlayers[i].playerID = tempInt1;
    MPI_Bcast(&tempInt1,1, MPI_INT, 0, MPI_COMM_WORLD);
    board.allPlayers[i].armySize = tempInt1;
    MPI_Bcast(&tempInt1,1, MPI_INT, 0, MPI_COMM_WORLD);
    board.allPlayers[i].roadSize = tempInt1;
    MPI_Bcast(&tempInt1,1, MPI_INT, 0, MPI_COMM_WORLD);
    board.allPlayers[i].victoryPoints = tempInt1;
  }
}
void sendPlayers(int rank1){
  int tempInt=0;
  for(unsigned int i=0; i<board.allPlayers.size(); i++){
    tempInt = board.allPlayers[i].resourceHand.size();
    MPI_Send(&tempInt,1, MPI_INT, rank1, 1, MPI_COMM_WORLD);
    for(unsigned int j=0; j<board.allPlayers[i].resourceHand.size(); j++){
      tempInt = board.allPlayers[i].resourceHand[j];
      MPI_Send(&tempInt,1, MPI_INT, rank1, 1, MPI_COMM_WORLD);
    }
    tempInt = board.allPlayers[i].developmentHand.size();
    MPI_Send(&tempInt,1, MPI_INT, rank1, 1, MPI_COMM_WORLD);
    for(unsigned int j=0; j<board.allPlayers[i].developmentHand.size(); j++){
      tempInt = board.allPlayers[i].developmentHand[j];
      MPI_Send(&tempInt,1, MPI_INT, rank1, 1, MPI_COMM_WORLD);
    }
    tempInt = board.allPlayers[i].ownedSquares.size();
    MPI_Send(&tempInt,1, MPI_INT, rank1, 1, MPI_COMM_WORLD);
    for(unsigned int j=0; j<board.allPlayers[i].ownedSquares.size(); j++){
      tempInt = board.allPlayers[i].ownedSquares[j]->id;
      MPI_Send(&tempInt,1, MPI_INT, rank1, 1, MPI_COMM_WORLD);
    }
    tempInt = board.allPlayers[i].playerID;
    MPI_Send(&tempInt,1, MPI_INT, rank1, 1, MPI_COMM_WORLD);
    tempInt = board.allPlayers[i].armySize;
    MPI_Send(&tempInt,1, MPI_INT, rank1, 1, MPI_COMM_WORLD);
    tempInt = board.allPlayers[i].roadSize;
    MPI_Send(&tempInt,1, MPI_INT, rank1, 1, MPI_COMM_WORLD);
    tempInt = board.allPlayers[i].victoryPoints;
    MPI_Send(&tempInt,1, MPI_INT, rank1, 1, MPI_COMM_WORLD);
  }
}
void recvPlayer(int rank1){
  int tempInt1=0;
  int tempInt2=0;
  for(unsigned int i=0; i<board.allPlayers.size(); i++){
    MPI_Recv(&tempInt1,1, MPI_INT, rank1, 1, MPI_COMM_WORLD, MPI_STATUS_IGNORE);
    board.allPlayers[i].resourceHand.clear();
    for(int j=0; j<tempInt1; j++){
      MPI_Recv(&tempInt2,1, MPI_INT, rank1, 1, MPI_COMM_WORLD, MPI_STATUS_IGNORE);
      board.allPlayers[i].resourceHand.push_back((Resource)tempInt2);
    }
    MPI_Recv(&tempInt1,1, MPI_INT, rank1, 1, MPI_COMM_WORLD, MPI_STATUS_IGNORE);
    board.allPlayers[i].developmentHand.clear();
    for(int j=0; j<tempInt1; j++){
      MPI_Recv(&tempInt2,1, MPI_INT, rank1, 1, MPI_COMM_WORLD, MPI_STATUS_IGNORE);
      board.allPlayers[i].developmentHand.push_back((DevelopmentCard)tempInt2);
    }
    MPI_Recv(&tempInt1,1, MPI_INT, rank1, 1, MPI_COMM_WORLD,MPI_STATUS_IGNORE);
    board.allPlayers[i].ownedSquares.clear();
    for(int j=0; j<tempInt1; j++){
      MPI_Recv(&tempInt2,1, MPI_INT, rank1, 1, MPI_COMM_WORLD,MPI_STATUS_IGNORE);
      for(int a=0; a<board.size; a++){
	for(int b=0; b<board.size; b++){
	  if(board.board[a][b].id == tempInt2){
	    board.allPlayers[i].ownedSquares.push_back(&board.board[a][b]);
	  }
	}
      }
    }    
    MPI_Recv(&tempInt1,1, MPI_INT, rank1, 1, MPI_COMM_WORLD,MPI_STATUS_IGNORE);
    board.allPlayers[i].playerID = tempInt1;
    MPI_Recv(&tempInt1,1, MPI_INT, rank1, 1, MPI_COMM_WORLD,MPI_STATUS_IGNORE);
    board.allPlayers[i].armySize = tempInt1;
    MPI_Recv(&tempInt1,1, MPI_INT, rank1, 1, MPI_COMM_WORLD,MPI_STATUS_IGNORE);
    board.allPlayers[i].roadSize = tempInt1;
    MPI_Recv(&tempInt1,1, MPI_INT, rank1, 1, MPI_COMM_WORLD,MPI_STATUS_IGNORE);
    board.allPlayers[i].victoryPoints = tempInt1;
  }
}
