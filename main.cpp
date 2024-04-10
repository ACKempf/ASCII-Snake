#include <iostream>
#include <vector>
#include <unistd.h>
#include "TControl.hpp"

using namespace std;

/*
struct that stores tetrominos as relative coordinates from an abritrary "center block"
the center block will be the point of rotation based on the SRS standard, or in cases where it lies on an edge
an arbtrary block is chosen.
*/
struct tetrominoCoords {
  //Relative coordinates sets for individual rotation states
  vector<pair<int, int>> rot0;
  vector<pair<int, int>> rot90;
  vector<pair<int, int>> rot180;
  vector<pair<int, int>> rot270;

  //Advance the current state clockwise (+90 degrees)
  void rotateCWise();

  //Advance the current state counter-clockwise (-90 degrees)
  void rotateCCWise();

  //Returns the current rotation coordinates of the coordinates
  void get();
};

/*
class for active tetromino object here
*/

/*
class for active game grid here
*/

int main()
{
  //Enable raw mode
  enableRawMode();

  //Input loop for confirming what display size the user wants
  while(true) {
  char input = 0;
  cout << "Adjust window to desired size and press any key to continue." << endl;
  cin.ignore();

  cout << "Enter y/Y to confirm display size: " << getTermSize().second << "x" << getTermSize().first << endl;
  cin >> input;
  if (input=='y'||input=='Y') break;
  cout << "\n\n\n";
  }

  /*
  game menu loop here
  */
 
}