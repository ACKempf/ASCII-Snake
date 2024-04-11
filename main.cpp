#include <iostream>
#include <vector>
#include <unistd.h>
#include <exception>
#include "TControl.hpp"

using namespace std;
//Define a type alias for vectors containing pair called "p-vector", short for "pair vector"
using pvector = vector<pair<int, int>>;

//Constants
const int ENTITY_CHAR = ' ';
const bool ENTITY_BOLD = false;
const bool ENTITY_ITALIC = false;
const bool ENTITY_UNDERLINE = false;
const bool ENTITY_BLINK = false;
const bool ENTITY_FG_COLOR = 0;
const int WALL_COLOR = 245;
const int EMPTY_COLOR = 232;
const bool MENU_TEXT_BOLD = false;
const bool MENU_TEXT_ITALIC = false;
const bool MENU_TEXT_UNDERLINE = false;
const bool MENU_TEXT_BLINK = false;
const int MENU_TEXT_FG_COLOR = 231;
const int MENU_TEXT_BG_COLOR = 232;
const int MENU_CURSOR_CHAR = '>';
const bool MENU_CURSOR_BOLD = false;
const bool MENU_CURSOR_ITALIC = false;
const bool MENU_CURSOR_UNDERLINE = false;
const bool MENU_CURSOR_BLINK = true;
const int MENU_CURSOR_FG_COLOR = 231;
const int MENU_CURSOR_BG_COLOR = 232;

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
  signed int rot_state = 0;

  //The color of the Tetromino
  //Uses ANSI escape colors
  int color = 0;

  //Advance the current state clockwise (+90 degrees)
  void rotateCWise()
  {
    rot_state = (rot_state+1)%4;
    return;
  };

  //Advance the current state counter-clockwise (-90 degrees)
  void rotateCCWise()
  {
    rot_state = (rot_state+3)%4;
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
        cerr << rot_state << endl;
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

void testMode();

//Predefined constant templates for tetrominos
const TetrominoCore I_TETRO(
  {{1, 0}, {1, 1}, {1, 2}, {1, 3}},
  {{0, 2}, {1, 2}, {2, 2}, {3, 2}},
  {{2, 0}, {2, 1}, {2, 2}, {2, 3}},
  {{0, 1}, {1, 1}, {2, 1}, {3, 1}},
  123);
const TetrominoCore J_TETRO(
  {{0, 0}, {1, 0}, {1, 1}, {1, 2}},
  {{0, 1}, {0, 2}, {1, 1}, {2, 1}},
  {{1, 0}, {1, 1}, {1, 2}, {2, 2}},
  {{2, 0}, {2, 1}, {1, 1}, {0, 1}},
  21);
const TetrominoCore L_TETRO(
  {{1, 0}, {1, 1}, {0, 2}, {1, 2}},
  {{0, 1}, {1, 1}, {2, 1}, {2, 2}},
  {{2, 0}, {1, 0}, {1, 1}, {1, 2}},
  {{0, 0}, {0, 1}, {1, 1}, {2, 1}},
  208);
const TetrominoCore O_TETRO(
  {{0, 0}, {0, 1}, {1, 0}, {1, 1}},
  {{0, 0}, {0, 1}, {1, 0}, {1, 1}},
  {{0, 0}, {0, 1}, {1, 0}, {1, 1}},
  {{0, 0}, {0, 1}, {1, 0}, {1, 1}},
  226);
const TetrominoCore S_TETRO(
  {{0, 1}, {0, 2}, {1, 0}, {1, 1}},
  {{0, 1}, {1, 1}, {1, 2}, {2, 2}},
  {{2, 0}, {2, 1}, {1, 1}, {1, 2}},
  {{0, 0}, {1, 0}, {1, 1}, {2, 1}},
  82);
const TetrominoCore T_TETRO(
  {{1, 0}, {1, 1}, {0, 1}, {1, 2}},
  {{0, 1}, {1, 1}, {2, 1}, {1, 2}},
  {{1, 0}, {1, 1}, {1, 2}, {2, 1}},
  {{1, 0}, {0, 1}, {1, 1}, {2, 1}},
  129);
const TetrominoCore Z_TETRO(
  {{0, 0}, {0, 1}, {1, 1}, {1, 2}},
  {{2, 1}, {1, 1}, {1, 2}, {0, 2}},
  {{1, 0}, {1, 1}, {2, 1}, {2, 2}},
  {{0, 1}, {1, 1}, {1, 0}, {2, 0}},
  196);

enum types {static_tetromino, active_tetromino, wall, empty_space};

//Class used to store entities for use in the game array, since arrays can only hold one type of object
class Entity
{
  public:
    //Constructor
    Entity(types t, TetrominoCore ts=O_TETRO, int c=0, Entity fcd=Entity(empty_space)) : tetromino_shape(ts), facade(fcd)
    {
      switch (t) {
        case active_tetromino:
          active = true;
          tangible = true;
          color = ts.color;
          break;
        case static_tetromino:
          active = false;
          tangible = true;
          color = ts.color;
          break;
        case wall:
          active = false;
          tangible = true;
          color = WALL_COLOR;
          break;
        case empty_space:
          active = false;
          tangible = false;
          color = EMPTY_COLOR;
        default:
          cerr << "Invalid type: " << t << endl;
          throw "Invalid type assigned to entity class.";
          break;
      };
    };

    //Decompose takes a position coordinate value and returns the positions of inactive entities of that color
    pair<Entity, pvector>decompose(pair <int, int> pos)
    {
      Entity inactive_clone(static_tetromino, O_TETRO, color);
      pair <Entity, pvector> to_return = {inactive_clone, {}};
      if (!active) {
        throw logic_error("Invalid decomposing of inactive entity");
      }
      for (pair<int, int> offset:getOffsets()) {
        to_return.second.push_back({pos.first+offset.first, pos.second+offset.second});
      }
      
      //Returns a pair, containing the inactive entities made, and their coordinates
      return to_return;
    }

    //getters
    bool getActive() {return active;}
    int getColor() {return color;}
    pvector getOffsets() {
      if (!active) { 
        throw logic_error("Invalid access of non-active entity offsets");
      }
      //Retrieves the current shape of the tetromino
      return tetromino_shape.get();
    }

  private:
    bool active;
    bool tangible;
    int color = 0;
    TetrominoCore tetromino_shape;
    //Because the entity for active tetrimino isnt really an existant thing, facade can hold a "placeholder" values
    int facade_color;
    bool facade_tangible;

};

class GameSpace 
{
  public:
    //Constructor
    GameSpace(int r, int c) : rows(r), columns(c), entity_grid(r, vector<Entity>(c, Entity(empty_space))) {};

    //Advance the active tile down by the offset pair
    void advanceActive() 
    {
      //Positions must be recorded before hand and then iterated over because otherwise chain will force pieces down to bottom immediately
      for (int r=0; r<rows; r++) {
        for (int c=0; c<columns; c++) {
          if (entity_grid[r][c].getActive()) {
            //Record position and use given offset pair
            pair<int, int> to_move = {r, c};
            pair<int, int> offset = {-1, 0};
            offsetEntity(to_move, offset);
            return;
          }
        }
      }
    }
    //Figures color for the current entity grid and prints that to console
    void renderEntityGrid() {
      Terminal to_render(rows, columns);
      to_render.setCursorVisibility(false);
      //This holds the active slot because it must be rendered after everything else
      //Because each tetrimino is technically only a single spot on the grid, rendering beforehand
      //Would just result in it being overwritten by other tiles after the fact
      //So its position is stored and it is rendered after the fact
      pair<int, int> top_render;
      for (int r=0; r<rows; r++) {
        for (int c=0; c<columns; c++) {
          Entity current_pos = entity_grid[r][c];
          if (current_pos.getActive()) {
            top_render.first = r;
            top_render.second = c;
            //Tetrimino entity block that stores tetrimino position is assigned background color, will be rendered on top of later with active tetrimino offset values
            to_render.setChar(r, c, ENTITY_CHAR, ENTITY_BOLD, ENTITY_ITALIC, ENTITY_UNDERLINE, ENTITY_BLINK, ENTITY_FG_COLOR, EMPTY_COLOR);
          } else {
            to_render.setChar(r, c, ENTITY_CHAR, ENTITY_BOLD, ENTITY_ITALIC, ENTITY_UNDERLINE, ENTITY_BLINK, ENTITY_FG_COLOR, current_pos.getColor());
          }
        }
      }
      //Go back and render tetrimino over the top of its place
      Entity to_top_render = entity_grid[top_render.first][top_render.second];
      for (pair<int, int> offset: to_top_render.getOffsets()) {
        to_render.setChar(top_render.first+offset.first, top_render.second+offset.second, ENTITY_CHAR, ENTITY_BOLD, ENTITY_ITALIC, ENTITY_UNDERLINE, ENTITY_BLINK, ENTITY_FG_COLOR, to_top_render.getColor());
      }

      to_render.draw();
    }


  private:
    //Class member values
    const int rows;
    const int columns;
    vector<vector<Entity>> entity_grid;

    //Private class methods
    void setEntity(int rpos, int cpos, Entity ent) {
      if ((rpos > rows || rpos < 0) || (cpos > columns || cpos < 0)) {
        cerr << "Attempted to assign outside of entity grid @ " << rpos << ", " << cpos << endl;
        throw logic_error("Invalid entity position");
      }
      entity_grid[rpos][cpos] = ent;
      return;
    }
    //Take an entity and move it from its current position by offsets in row and column direction
    void offsetEntity(pair<int, int> pos, pair<int, int> offset) {
      Entity to_move = entity_grid[pos.first][pos.second];
      //A new empty space object to be inserted into the previous space
      Entity empty(empty_space);
      //Empty the previous position
      setEntity(pos.first, pos.second, empty_space);
      //Populate the new poisiton based on offset
      setEntity(pos.first+offset.first, pos.second+offset.second, to_move);

      return;
    }
    //Take a position and set it to a empty entity
    void removeEntity(pair<int, int> pos) {
      //Create a new empty entity objet and assign entity at given position to empty
      Entity empty(empty_space);
      setEntity(pos.first, pos.second, empty);
      return;
    }
    //Checks for collision with the active tetrimino
    bool checkCollision() {
      //A copy of the current table to run advanceActive() on and look for intersections
    }

};

int main()
{
  //Enable raw mode
  enableRawMode();
  int rows;
  int columns;

  //Input loop for confirming what display size the user wants
  //Also for taking dev commands
  while(true) {
    static char input;

    cout << "Adjust window to desired size and press any key to continue." << endl;
    cin.ignore();
    rows = getTermSize().first-1;
    columns = getTermSize().second;
    cout << "Enter y/Y to confirm display size: " << rows << "x" << columns << endl;
    cin >> input;
    if (input=='y'||input=='Y') 
      {
      break;
      }
    //A dev code for testing tetromino rotation
    if (input=='9') {
      testMode();
    }
  }


}



/*
A function that handles the devtest mode for testing functionality of tetromino rotation and movement
*/
void testMode()
{
  TetrominoCore current_tetro = I_TETRO;
  while (true){
    //Input parsing for while loop and resizing terminal
    static char input;
    static pair<int, int> offset;
    cin >> input;
    int width = getTermSize().first-1;
    int length = getTermSize().second;
    //Switch statement for operations
    switch (input){
      case '1':
        current_tetro = I_TETRO;
        break;
      case '2':
        current_tetro = J_TETRO;
        break;
      case '3':
        current_tetro = L_TETRO;
        break;
      case '4':
        current_tetro = O_TETRO;
        break;
      case '5':
        current_tetro = S_TETRO;
        break;
      case '6':
        current_tetro = T_TETRO;
        break;
      case '7':
        current_tetro = Z_TETRO;
        break;
      case 'q':
        current_tetro.rotateCCWise();
        break;
      case 'e':
        current_tetro.rotateCWise();
        break;
      case 'w':
        offset.first--;
        break;
      case 's':
        offset.first++;
        break;
      case 'a':
        offset.second--;
        break;
      case 'd':
        offset.second++;
        break;

      
    };

    Terminal t(width, length);
    for (pair<int, int> p:current_tetro.get()) {
      t.setChar((width/2)+p.first+offset.first, (length/2)+p.second+offset.second, current_tetro.rot_state+48, false, false, false, false, 0, current_tetro.color);
    }
    t.draw();
    cout << "USE Q&E TO ROTATE" << endl << "USE W/A/S/D TO MOVE TETROMINO" << endl << "USE 1-7 TO SWITCH TETROMINO" << endl;
  }
};