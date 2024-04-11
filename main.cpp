#include <iostream>
#include <vector>
#include <unistd.h>
#include <exception>
#include "TControl.hpp"

using namespace std;
//Define a type alias for vectors containing pair called "p-vector", short for "pair vector"
using pvector = vector<pair<int, int>>;

/*
struct that stores tetrominos as relative coordinates from an abritrary "center block"
the center block will be the point of rotation based on the SRS standard, or in cases where it lies on an edge
an arbtrary block is chosen.
*/
struct TetrominoCore {
  //Relative coordinates sets for individual rotation states
  pvector rot0;
  pvector rot90;
  pvector rot180;
  pvector rot270;

  //The current rotation state, 0-3, where 3 is 3*90 or 270 degrees
  int rot_state = 0;

  //The color of the Tetromino
  //Uses ANSI escape colors
  int color = 0;

  //Advance the current state clockwise (+90 degrees)
  void rotateCWise()
  {
    rot_state = (++rot_state)%4;
    return;
  };

  //Advance the current state counter-clockwise (-90 degrees)
  void rotateCCWise()
  {
    rot_state = (--rot_state)%4;
    return;
  };

  //Returns the current rotation coordinates of the coordinates
  pvector get() {
    switch (rot_state) {
      case 0:
        return rot0;
      case 1:
        return rot90;
      case 2:
        return rot180;
      case 3:
        return rot270;
      default:
        throw invalid_argument("Invalid rotation state.");
    }
  }

  //Constructor
  TetrominoCore(pvector r0, pvector r90, pvector r180, pvector r270, int c)
  {
    color=c;
    rot0 = r0;
    rot90 = r90;
    rot180 = r180;
    rot270 = r270;
  };
};

//Predefined constant templates for tetrominos and their constants
const TetrominoCore I_TETRO(
  {{0, 2}, {1, 2}, {2, 2}, {3, 2}}, 
  {{2, 0}, {2, 1}, {2, 2}, {2, 3}}, 
  {{0, 1}, {1, 1}, {2, 1}, {3, 1}}, 
  {{1, 0}, {1, 1}, {1, 2}, {1, 3}},
  123);
const TetrominoCore J_TETRO(
  {{0, 1}, {0, 2}, {1, 1}, {1, 2}},
  {{1, 0}, {1, 1}, {1, 2}, {2, 2}},
  {{0, 1}, {1, 1}, {2, 1}, {2, 0}},
  {{0, 0}, {1, 0}, {1, 1}, {1, 2}},
  21);
const TetrominoCore L_TETRO(
  {{0, 1}, {1, 1}, {2, 1}, {2, 2}},
  {{1, 0}, {2, 0}, {1, 1}, {1, 2}},
  {{0, 0}, {0, 1}, {1, 1}, {2, 1}},
  {{0, 2}, {1, 2}, {1, 1}, {1, 0}},
  208);
const TetrominoCore O_TETRO(
  {{0, 0}, {0, 1}, {1, 0}, {1, 1}},
  {{0, 0}, {0, 1}, {1, 0}, {1, 1}},
  {{0, 0}, {0, 1}, {1, 0}, {1, 1}},
  {{0, 0}, {0, 1}, {1, 0}, {1, 1}},
  226);
const TetrominoCore S_TETRO(
  {{0, 1}, {1, 1}, {1, 2}, {2, 2}},
  {{1, 1}, {1, 2}, {2, 0}, {2, 1}},
  {{0, 0}, {1, 0}, {1, 1}, {2, 1}},
  {{0, 2}, {0, 1}, {1, 1}, {1, 0}},
  82);
const TetrominoCore T_TETRO(
  {{0, 1}, {1, 1}, {1, 2}, {2, 1}},
  {{1, 1}, {1, 0}, {1, 2}, {2, 1}},
  {{0, 1}, {1, 1}, {1, 0}, {2, 1}},
  {{0, 1}, {1, 0}, {1, 1}, {1, 2}},
  129);
const TetrominoCore Z_TETRO(
  {{0, 2}, {1, 2}, {1, 2}, {2, 1}},
  {{1, 0}, {1, 1}, {2, 1}, {2, 2}},
  {{0, 1}, {1, 1}, {1, 0}, {2, 0}},
  {{0, 0}, {0, 1}, {1, 1}, {1, 2}},
  196);

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
  //Also for taking dev commands
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