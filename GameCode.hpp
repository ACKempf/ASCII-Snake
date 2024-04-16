/*
* File: GameCode.hpp
* Date: 03/28/2024
*
* Description:
* Header file that contains all of the classes and structures 
* That are required to run the Snake game, the purpose of this
* header file is to increase the readability and modularity of 
* the snake game code. 
*/

//Redundancy safety check
#ifndef GAMEC_H
#define GAMEC_H

#include <iostream>
#include <vector>
#include <unistd.h>
#include <exception>
#include <random>
#include "TControl.hpp"

using namespace std;
using ipair = pair<int, int>; //Type alias for integer pairs
using pvector = vector<pair<int, int>>; //Type alias for vectors containing integer pairs

//Character defaults
char CURSOR_UP = 'w';
char CURSOR_DOWN = 's';
char SNAKE_HEAD_UP = '^';
char SNAKE_HEAD_DOWN = 'v';
char SNAKE_HEAD_RIGHT = '>';
char SNAKE_HEAD_LEFT = '<';
char FOOD_CHAR = '*';
char SNAKE_BODY_CHAR = '*';
char GRID_BORDER = '#';
char CURSOR_CHAR = '>';
char PAUSE_KEY = 'p';
char POWERUP_1_CHAR = '+';
char POWERUP_2_CHAR = 'x';

//Gameplay defaults
int INITIAL_SPEED = 200;
int MAX_SPEED = 50;
int SPEED_MULTIPLIER = 85;
bool SELF_COLLISION = true;
int HIGHEST_SCORE = 0; //Initialize highest score w/value 0
bool ENABLE_POWERUPS = true;
int POWERUP_TIME = 10;
int POWERUP_SPAWN_TIME = 15;
int SLOW_DOWN_POWERUP_INCREMENT = 2;
/*
The CharStyle struct presents a cleaner way to store format presets
*/
struct CharStyle
{
  //Boolean style values
  bool bold;
  bool italic;
  bool underline;
  bool blinking;

  //integer style values
  int fg_color;
  int bg_color;

  CharStyle(bool bold, bool ital, bool undr, bool blnk, int fgcl, int bgcl) : bold(bold), italic(ital), underline(undr), blinking(blnk), fg_color(fgcl), bg_color(bgcl) {};
};

//Menu style presets
CharStyle MENU_TEXT(false, false, false, false, 184, 234);
CharStyle MENU_OPTION(false, false, false, false, 214, 234);
CharStyle CURSOR(false, false, false, false, 202, 234);
CharStyle SCOREBOARD(false, false, false, false, 231, 23);
//Snake game style presets
CharStyle SNAKE_BODY(false, false, false, false, 231, 233);
CharStyle SNAKE_HEAD(false, false, false, false, 231, 233);
CharStyle SNAKE_FOOD(false, false, false, false, 231, 233);
CharStyle BARRIER(false, false, false, false, 231, 23);
CharStyle POWERUP1(false, false, false, false, 231, 232);
CharStyle POWERUP2(false, false, false, false, 231, 232);
CharStyle BACKGROUND(false, false, false, false, 231, 232);
//************************************************************************************//

/*
Menu Class:
  The menu class allows the programmer to define any number of header lines, and any number of option lines, 
  and allows the user to manipulate a cursor to retrieve a selection from the option lines.
*/
class Menu
{
  public:
    /*
    Constructor for Menu class

    Params: 2 string vectors, 1 Terminal reference
    vector<string>, m_t: The header lines to be presented
    vector<string>, m_o: The number of option lines to be presented
    */
    Menu(vector<string> m_t, vector<string> m_o, Terminal& term) :
     t(term), menu_text(m_t), menu_options(m_o), center(term.findCenter()), left_justification_column(center.second-(getLongestStringLength()/2)),
     max_cursor_position(menu_options.size()) {};

    /*
    Moves the cursor by a given offset, positive advances the cursor, negative does otherwise

    Params: 1 integer
    int, offset: The amount to offset the cursor by

    Returns: Void
    */
    void moveCursor(int offset)
    {
      int to_position = cursor_position + offset;
      if (to_position>0 && to_position<=max_cursor_position) cursor_position = to_position;
      return;
    };

    /*
    Updates the data inside of the Terminal array to reflect the current menu

    Params: None

    Returns: Void
    */
    void updateTerminal() 
    {
      pushToTerminal();
      return;
    }

    /*
    Returns the current cursor selection (Corresponds to a certain option on the menu)
    Note: Because the order of the options is always first to last, if the order isnt changed, the cursor value for that option will not change

    Params: None

    Returns: An int representing the current position of the cursor
    */
    int getSelection() {return cursor_position;}


  private:
    //Terminal reference used by the Menu class to modify an active Terminal instance
    Terminal& t;
    //Constant list of menu header text and option lines
    const vector<string> menu_options;
    const vector<string> menu_text;
    //A constant with the coordinates of the current center point
    const ipair center;
    //The horizontal distance between the cursor and the first letter of an option
    const int cursor_column_offset = 2;
    //The amount of new lines between last header line and first option line
    const int text_option_seperation = 1;
    //The column coordinates where left justified text begins printing
    const int left_justification_column;
    //The maximum position the cursor can occupy
    const int max_cursor_position;
    //The current position of the cursor
    int cursor_position = 1;

    /*
    Exports the current state of the menu display to the active Terminal class display grid
    note: Does not draw the current terminal state, .draw() must be called after this method is called

    Params: None

    Returns: Void
    */
    void pushToTerminal(){
      //Zero out display grid to build menu display
      t.clearGrid();
      //The row to begin printing the first header line at
      //Expression ensures this is vertically centered
      int current_row = center.first-((menu_options.size()+menu_text.size())/2)-text_option_seperation;
      //For loop iterating over individual character of each string in menu_text
      for (string s:menu_text) {
        //Current offset used to ensure characters are printed to the right of previous
        int current_column_offset=0;
        for (char c:s) {
          //Set current character with MENU_TEXT constant style
          t.setChar(current_row, left_justification_column+current_column_offset, c, MENU_TEXT.bold, MENU_TEXT.italic, MENU_TEXT.underline, MENU_TEXT.blinking, MENU_TEXT.fg_color, MENU_TEXT.bg_color);
          //Advance the column to the right
          current_column_offset++;
        }
        //Moves down to the next row before printing next line
        current_row++;
      }

      //Adds blank space between last header line and first option line
      current_row += text_option_seperation;

      //Used to compare current cursor position to line being printed
      int selection_cursor_index = 1;

      for (string s:menu_options) {
        int current_column_offset=0;
        for (char c:s) {
          //MENU_OPTION style
          t.setChar(current_row, left_justification_column+current_column_offset, c, MENU_OPTION.bold, MENU_OPTION.italic, MENU_OPTION.underline, MENU_OPTION.blinking, MENU_OPTION.fg_color, MENU_OPTION.bg_color);
          current_column_offset++;
        }
        //Adds visual cursor if current line cursor position matches active cursor position
        if (selection_cursor_index == cursor_position) t.setChar(current_row, left_justification_column-cursor_column_offset, CURSOR_CHAR, CURSOR.bold, CURSOR.italic, CURSOR.underline, CURSOR.blinking, CURSOR.fg_color, CURSOR.bg_color);
        current_row++;
        //Advances the cursor position representation of the current option
        selection_cursor_index++;
      }
      return;
    }

    /*
    Returns the size of the longest string between the menu_options and menu_text vectors

    Params: None

    Returns: An integer representing the largest amount of characters in the longest string
    */    
   int getLongestStringLength()
    {
      //current longest string length
      int s_l=0;
      //Longest from menu_options
      for (string s:menu_options) {
        if (s.size() > s_l) s_l = s.size();
      } 
      //Longest from menu_text
      for (string s:menu_text) {
        if (s.size() > s_l) s_l = s.size();
      }
      return s_l;
    }
};

//Function prototypes for menu navigation
void intInputMenu(string text, Terminal& t, int& to_set);
void boolInputMenu(string text, Terminal& t, bool& to_set);
void charInputMenu(string text, Terminal& t, char& to_set);
void styleInputMenu(string text, Terminal& t, CharStyle& to_edit);
void pauseMenu(string text, Terminal& t, bool& game_state);
void styleEditorMenu(Terminal &t);
void settingsEditorMenu(Terminal &t);

//************************************************************************************//

/*
The scoreboard class only maintains the thin 2 tall space above the game
it keeps it populated with the current score and velocity of the snake
*/
class ScoreBoard
{
  public:
    /*
    Constructor for scoreboard

    Params: 1 Terminal Reference, 1 Highschore Reference
    */
    ScoreBoard(Terminal& tin, int& hs) : t(tin), high_score(hs) {};

    void updateTerminal() {
      pushToTerminal();
      return;
    }

    /*
    Increment the current score on the score board

    Params: None

    Returns: Void
    */
    void scoreEvent(){
      current_score++;
      //A quick line that keeps track of the highest score in a session
      if (current_score>high_score) {high_score = current_score;}
      return;
    }

    /*
    Sets the current speed to be displayed under the current speed section

    Params: None

    Returns: Void
    */
    void setSpeed(int spd){
      current_speed = spd;
      return;
    }

  private:
    int current_score = 0;
    int current_speed = 0;
    int& high_score;
    Terminal& t;

    //coordinates of the upper left corner and bottom right corner of the scoreboard
    const string score_prefix = "SCORE / TOTAL-FOOD: ";
    const string speed_prefix = "SPEED   (TILE/SEC): ";
    const int center_line = t.findCenter().second;
    //Roughly centers the text and finds the left justification line
    const int left_justification_offset = -1*((score_prefix.size()+speed_prefix.size())/4);

    /*
    Pushed display array changes to the active Terminal

    Params: None

    Returns: Void
    */
    void pushToTerminal()
    {
      string score_print = score_prefix+to_string(current_score);
      string speed_print = speed_prefix+to_string(current_speed);
      int current_column = center_line+left_justification_offset;

      for (char c:score_print){
        t.setChar(0, current_column, c, SCOREBOARD.bold, SCOREBOARD.italic, SCOREBOARD.underline, SCOREBOARD.blinking, SCOREBOARD.fg_color, SCOREBOARD.bg_color);
        current_column++;
      }

      current_column = center_line+left_justification_offset;

      for (char c:speed_print){
        t.setChar(1, current_column, c, SCOREBOARD.bold, SCOREBOARD.italic, SCOREBOARD.underline, SCOREBOARD.blinking, SCOREBOARD.fg_color, SCOREBOARD.bg_color);
        current_column++;
      }

      return;
    };
    
};

//************************************************************************************//

/*
    Snake Class:
    represents the snake and all of its functions. Responsible for: moving (changing the snake coordinates) up, down, left, and right;
    displaying (drawing) the snake onto the 'field' (grid). Detecting collision for the snake (both with itself and with the borders);
    and growing the snake (increasing its body size). 
*/
class Snake
{
private:
  pvector body; //Vector of pairs to store the coordinates of the snake's body segments
  char direction; //Current direction of the snake ('a' for left, 'd' for right, 'w' for up, 's' for down)
  ipair prevTailPosition; //Previous position of the tail segment
  Terminal &t; //Terminal reference
  ipair screen_size; //screensize reference

public:
   /*
    Constructor for Snake class
    Params: 1 reference, 1 pair, 2 integers, 1 character
    Terminal &terminal : reference to terminal object
    ipair display_size : pair that contains the terminal/display size
    int init_row : initial row for spawning
    int init_column : initial column for spawning
    char init_direction : initial direction of the snake
    */
  Snake(Terminal &terminal, ipair display_size, int init_row, int init_column, char init_direction)
  : t(terminal), screen_size(display_size)
  {
    //Initialize the snake with a single body segment at the starting position
    body.push_back({init_row, init_column});
    direction = init_direction;
    prevTailPosition = {init_row, init_column};
  }

  //Function to move the snake in the current direction
  void move()
  {
    //Calculate the offset for the new head position based on the current direction
    int row_offset = 0, column_offset = 0;
    switch (direction)
    {
    case 'a': //Left
      row_offset = 0;
      column_offset = -1;
      break;
    case 'd': //Right
      row_offset = 0;
      column_offset = 1;
      break;
    case 'w': //Up
      row_offset = -1;
      column_offset = 0;
      break;
    case 's': //Down
      row_offset = 1;
      column_offset = 0;
      break;
    }

    //Update the previous position of the tail segment
    prevTailPosition = body.back();

    //Update the positions of the body segments to follow the head
    for (size_t i = body.size() - 1; i > 0; --i)
    {
      body[i] = body[i - 1];
    }

    //Calculate updated position of the head.
    ipair &head = body.front();
    head.first += row_offset;
    head.second += column_offset;
  }

  //Function to change the direction of the snake
  void changeDirection(char new_direction)
  {
    //Messy ifs, Only change direction is it is not opposite to the current direciton
    if (!((new_direction == 'w' && direction == 's')||(new_direction == 's' && direction == 'w'))){
      if (!((new_direction == 'a' && direction == 'd')||(new_direction == 'd' && direction == 'a'))) {
        //Update the direction
        direction = new_direction;
      }
    }
  }
  /*
    drawSnake Function:
    This function is responsible for drawing the snake on a terminal screen. It takes the snake object and a terminal object as parameters.
    It iterates through the snake's body segments, drawing each segment using a specified character for the body.
    It also draws the head of the snake according to its direction, using different characters for each direction.
    The function erases the previous position of the snake's tail segment and then draws the updated grid with the snake.
  */
  void drawSnake()
  {
    //Define the characters to represent the snake's body
    char headChar;
    switch (getDirection())
    {
    case 'a': //Left
      headChar = SNAKE_HEAD_LEFT;
      break;
    case 'd': //Right
      headChar = SNAKE_HEAD_RIGHT;
      break;
    case 'w': //Up
      headChar = SNAKE_HEAD_UP;
      break;
    case 's': //Down
      headChar = SNAKE_HEAD_DOWN;
      break;
    default: //Raise error if invalid head direction is found
      cerr << "Current head direction: " << getDirection();
      throw logic_error("Invalid head direction while drawing snake");
    }

    //Draw body segments (if there are any)
    for (const ipair &segment:body) {
      t.setChar(segment.first, segment.second, SNAKE_BODY_CHAR, SNAKE_BODY.bold, SNAKE_BODY.italic, SNAKE_BODY.underline, SNAKE_BODY.blinking, SNAKE_BODY.fg_color, SNAKE_BODY.bg_color);
    }
    
    //Draw the snake's head
    const ipair& headPos = body.front();
    t.setChar(headPos.first, headPos.second, headChar, SNAKE_HEAD.bold, SNAKE_HEAD.italic, SNAKE_HEAD.underline, SNAKE_HEAD.blinking, SNAKE_HEAD.fg_color, SNAKE_HEAD.bg_color);

    //Erase the previous position of the tail segment
    ipair prevTailPosition = getPrevTailPos();
    t.setChar(prevTailPosition.first, prevTailPosition.second, ' ', false, false, false, false, 0, BACKGROUND.bg_color);

    //Draw the updated grid with the snake
    t.draw();
  }
  /*
    checkSelfCollision Function:
    This function checks for self-collision of the snake, meaning if the snake's head collides with any
    part of its body. It takes the snake object as a parameter. It iterates through the snake's body
    segments starting from the second segment and compares their coordinates with the head's coordinates.
    If a match is found, indicating a collision, it returns true. Otherwise, it returns false, indicating
    no collision.
  */
  bool checkSelfCollision() const
  {
    //Get the coordinates of the head
    ipair head = body.front();
    //Iterate over the body segments starting from the third segment
    for (auto i = body.begin() + 1; i != body.end(); i++)
    {
      //If the coordinates of the current segment match the head's coordinates, return true
      if (*i == head)
      {
        //Only report self collision if SELF_COLLISION flag is true
        if (SELF_COLLISION){
          return true;
        } else return false;
      }
    }

    //No collision detected
    return false;
  }

  /*
    checkBoundaryCollision Function:
    This function checks for boundary collision of the snake, meaning if the snake's head moves beyond
    the boundaries of the game screen. It takes the snake object and the screen size as parameters.
    It retrieves the coordinates of the snake's head and calculates the boundary based on the screen size.
    It then checks if the head's coordinates exceed the boundaries. If a collision is detected, it returns
    true; otherwise, it returns false, indicating no collision.
  */
  bool checkBoundaryCollision()
  {
    //Get the coordinates of the head
    ipair head = body.front();
    ipair boundary = screen_size;
    //Check if the head's coordinates exceed the boundaries
    if (head.first == 3 || head.first == boundary.first || head.second == 0 || head.second == boundary.second)
    {
      return true; //Collision detected
    }
    //No collision detected
    return false;
  }
  //Function to retrieve the snake's body
  const pvector &getBody() const
  {
    return body;
  }

  //Function to retrieve the previous position of the tail segment
  ipair getPrevTailPos() const
  {
    return prevTailPosition;
  }

  //Function to retrieve the current direction of the snake
  char getDirection() const
  {
    return direction;
  }

  //Function to make the snake grow by adding a new segment
  void grow()
  {
    //Get the last segment of the snake's body
    const ipair &tailSegment = body.back();

    //Calculate the position for the new segment
    int new_row = tailSegment.first;
    int new_column = tailSegment.second;

    //Add the new segment in a position adjacent to the tail segment, depending on the direction
    switch (direction)
    {
    case 'a':    //Left
      new_column += 1; //Move the new segment one column to the left
      break;
    case 'd':    //Right
      new_column -= 1; //Move the new segment one column to the right
      break;
    case 'w':    //Up
      new_row += 1; //Move the new segment one row up
      break;
    case 's':    //Down
      new_row -= 1; //Move the new segment one row down
      break;
    }

    ///Add the new body segment to the end of the snake body vector, effectively extending the tail/end.
    body.push_back({new_row, new_column});
  }
};

/*
  sets the display size for the game itself, used to create the boundaries
  and properly set the boundary collision.
*/
ipair setBoundary(ipair screen_size)
{
  int rows = screen_size.first - 1;
  int columns = screen_size.second - 1;
  return {rows, columns};
}

//Function prototypes for snake game logic
void playGame(Snake &snake, Terminal &t, ipair screensize, ScoreBoard& sb);
void displayHeader(Snake &snake, Terminal&t, ipair screensize);
void createGrid(ipair screensize, Terminal &t);

/*
    Food Struct:
    Represents 'food' in the snake game. It has a position defined by row and column coordinates.
    The spawn method randomly selects and draws the food within the grid boundaries, making sure that the coordinates of the food 
    do not collide with the coordinates of the snake. checkCollision method determines if the food collides with the snake by comparing 
    its coordinates with the coordinates of the snake's body segments.
*/
struct Food
{
  int row = -1;
  int col = -1;
  Snake &snake;
  Terminal &t;

  //constructor for Food structure
  Food(Snake &s, Terminal &terminal) : snake(s), t(terminal){}

  //Method to spawn food at a random location on the grid
  void spawn(ipair boundary)
  {
    do{
      row = rand() % (boundary.first - 4) + 4;
      col = rand() % (boundary.second - 1) + 1;
    }while(checkCollision());
    t.setChar(row, col, FOOD_CHAR, SNAKE_FOOD.bold, SNAKE_FOOD.italic, SNAKE_FOOD.underline, SNAKE_FOOD.blinking, SNAKE_FOOD.fg_color, SNAKE_FOOD.bg_color);
  }

  //Method to check the food collision with the snake
  bool checkCollision() const
  {
    //Iterate over the snake's body and check if any segment matches the food's position
    for (const ipair &segment : snake.getBody())
    {
      if (row == segment.first && col == segment.second)
      {
        return true; //Collision detected
      }
    }
    return false; //No collision
  }
};
/*
    Powerups Struct:
    Represents powerups in the snake game. It has a position defined by row and column coordinates.
    checkCollision function is identical to the food struct checkCollision function. activatePowerUp
*/
struct Powerups
{
  int row = -1;
  int col =-1;
  int type;
  bool powerUpActive;
  bool initialPowerUpSpawned = false;
  bool &isPowerUpSpawned;
  time_t powerUpActivationTime;
  Snake &snake;
  Terminal &t;
  
  //Constructor for Powerups structure
  Powerups(Snake &s, Terminal &terminal, bool &powerUpSpawned) : snake(s), t(terminal), isPowerUpSpawned(powerUpSpawned){}

  /*
      checkCollision: iterates through the body of the snake and checks to see if any of body coordinates intersect with the coordinates of the powerup
      returns true if so, returns false otherwise.
  */
  bool checkCollision() const
  {
    //Iterate over the snake's body and check if any segment matches the Powerup's position
    for (const ipair &bodySegment : snake.getBody())
    {
      if (row == bodySegment.first && col == bodySegment.second)
      {
        return true; //Collision detected
      }
    }
    return false; //No collision
  }
  /*
      activatePowerUp: takes two reference variables (the game speed and the self collision flag) as parameters. 
      Applies the effects of each power up depending on the integer variable type (can be 1 or 2, indicating which power up is on the board).
      Then, it flags powerUpActive (indicating that a power up effect is currently being applied) and stores the time that the power up was activated.
  */
  void activatePowerUp(int &game_speed, bool &SELF_COLLISION)
  {
    if(type == 1) game_speed *= SLOW_DOWN_POWERUP_INCREMENT;
    else if(type == 2) SELF_COLLISION = false;
    powerUpActive = true;
    powerUpActivationTime = time(NULL);
  }
  /*
      deactivatePowerUp: takes two reference variables (the game speed and self collision flag) as parameters.
      Reverses the effects of the active power up (defined as variable type). For the Slow-mo powerup (type = 1),
      it first checks to make sure that reverting the speed won't cause it to go past the maximum speed. If so, it sets
      the game speed to the maximum speed. Then, it sets the active power up flag (powerUpActive) to false, indicating that a power up is no
      longer being applied.
  */
  void deactivatePowerUp(int &game_speed, bool &SELF_COLLISION)
  {
    if(type == 1){
      if(game_speed / SLOW_DOWN_POWERUP_INCREMENT > MAX_SPEED){
        game_speed = int(game_speed / SLOW_DOWN_POWERUP_INCREMENT);
      }
      else game_speed = MAX_SPEED;
    }
    else if(type == 2) SELF_COLLISION = true;
    powerUpActive = false;
  }
  /*
    powerUpExpired: subtracts the time that the power up was activated (powerUpActivationTime) from the current time, checking to see if 
    the result is greater than (or equal to) the amount of time the power up effects last. If so, then it returns true, meaning the power up has been applied for 
    the designated amount of seconds. Otherwise, returns false.
  */
  bool powerUpExpired() const {
     return time(NULL) - powerUpActivationTime >= POWERUP_TIME;
  }
  /*
    respawnPowerUpTimer: similar to powerUpExpired, subtracts time that power up was activated from the current time, checking to see if 
    the result is grater than the amount of time the power up effect lasts, plus the designated amount of time in between respawns (so that way power ups spawn a certain amount of 
    time after the power up effect has worn off). Returns true if so, indicating that enough time has passed for a power up to spawn.
  */
  bool respawnPowerUpTimer() const {
    return time(NULL) - powerUpActivationTime >= POWERUP_TIME + POWERUP_SPAWN_TIME;
  }
  /*
    shouldPowerUpSpawn: uses multiple methods to determine whether a powerup is able to spawn. First checks to make sure the user has enabled powerups in-game. If so, it checks
    to see if the initial power up has been spawned. If it has, it then checks to see: if there a power up is currently applied to the user and if enough time has passed for a respawn
    to occur and if a power up is already spawned on the board (and hasn't been consumed by the user). If none of these are true, returns false, indicating that a power up should not spawn.
  */
  bool shouldPowerUpSpawn() const{
    //Checks to see if the user has enabled powerups in the game
    if(!ENABLE_POWERUPS) return false;
    //Checks to see if a powerup has been spawned (start of game).
    else if(col == -1 && !initialPowerUpSpawned) return true;
    //checks to see if a power up is currently active and if the time between spawns has been reached
    else if(!powerUpActive && respawnPowerUpTimer() && !isPowerUpSpawned) return true;
    //if none of these conditions are true, returns false.
    else return false;
  }
  /*
    spawn: checks to see if a powerup should spawn using the shouldPowerUpSpawn function. If so, it randomly chooces a row, column, and type of powerup to spawn, making sure
    that the coordinates of the powerup does not intersect with the snake body. It then checks to see if the flag for the first power up spawn is true, if not, it sets it to true (indicating
    that this is the first powerup spawned). It then checks the type to see which powerup 'graphic' (char) to spawn.
  */
  void spawn(ipair boundary){
    if(shouldPowerUpSpawn()){
      do{
        row = rand() % (boundary.first - 4) + 4;
        col = rand() % (boundary.second - 1) + 1;
        type = rand() % 2 + 1;
      }while(checkCollision());
      if(!initialPowerUpSpawned) initialPowerUpSpawned = true;
      if(type == 1) t.setChar(row, col, POWERUP_1_CHAR, POWERUP1.bold, POWERUP1.italic, POWERUP1.underline, POWERUP1.blinking, POWERUP1.fg_color, POWERUP1.bg_color);
      else if(type == 2) t.setChar(row, col, POWERUP_2_CHAR, POWERUP2.bold, POWERUP2.italic, POWERUP2.underline, POWERUP2.blinking, POWERUP2.fg_color, POWERUP2.bg_color);
      isPowerUpSpawned = true;
    }
  }
};

//Snake game logic
//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~//

/*
    createGrid: gets the game boundaries using the setBoundary function. Then, uses for loops to go through and set the borders to their specific 'graphic' (char).
    After looping through the border, it loops through the 'field' space (space in between the border lines), and sets each position to a whitespace. 
    Then, draws the playing grid.
*/
void createGrid(ipair screen_size, Terminal &t)
{
  ipair boundary = setBoundary(screen_size);
  
  //Sets the boundary lines that go across horizontally
  for (int i = 3; i <= boundary.first; i++)
  {
    t.setChar(i, 0, GRID_BORDER, BARRIER.bold, BARRIER.italic, BARRIER.underline, BARRIER.blinking, BARRIER.fg_color, BARRIER.bg_color);
    t.setChar(i, boundary.second, GRID_BORDER, BARRIER.bold, BARRIER.italic, BARRIER.underline, BARRIER.blinking, BARRIER.fg_color, BARRIER.bg_color);
  }
  //Sets the vertical boundary lines
  for (int j = 0; j <= boundary.second; j++)
  {
    t.setChar(3, j, GRID_BORDER, false, false, false, false, BARRIER.fg_color, BARRIER.bg_color);
    t.setChar(boundary.first, j, GRID_BORDER, BARRIER.bold, BARRIER.italic, BARRIER.underline, BARRIER.blinking, BARRIER.fg_color, BARRIER.bg_color);
  }
  //nested loop to initialize the playable space.
  for (int i = 4; i < boundary.first; i++)
  {
    for (int j = 1; j < boundary.second; j++)
    {
      t.setChar(i, j, ' ', false, false, false, false, 0, BACKGROUND.bg_color);
    }
  }

  t.draw();
}
/*
    playGame: Essentially puts all of the pieces together to run the game. 
*/
void playGame(Snake &snake, Terminal &t, ipair screen_size, ScoreBoard &sb)
{
  
  //Initialize random seed
  srand(time(NULL));
  //initializes the game speed to the customizable INITIAL_SPEED
  int game_speed = INITIAL_SPEED;
  //Fnitialies the grid boundaries
  ipair boundary = setBoundary(screen_size);
  //Flag to check whether the snake is alive, or if the game should be over.
  bool alive = true;
  //Flag to check whether the first (initial) powerup of the game has spawned.
  bool initialPowerUpSpawned = false;
  //User input to change the direction of the snake.
  char input = getInput();
  //Flag to check whether there is a powerup on the 'field' or not
  bool isPowerUpSpawned = false;
  //Initializes food struct
  Food food(snake, t);
  //Initializes powerup struct
  Powerups powerup(snake, t, isPowerUpSpawned);
  //Draws the grid
  createGrid(screen_size, t);
  //Initial Speed
  sb.setSpeed((1000/game_speed));
  sb.updateTerminal();

  //Game loop, continually loops as long as the snake is alive
  while (alive)
  {
    //Move the snake
    snake.move();

    //Draw the snake
    snake.drawSnake();

    //Check if the food needs to be spawned (initially)
    if (food.row == -1 || food.col == -1) food.spawn(boundary);

    //Run the powerup spawn function
    powerup.spawn(boundary);

    //Check if the snake has eaten the food
    if (food.checkCollision())
    {
      //Tell scoreboard to update
      sb.scoreEvent();
      sb.setSpeed((1000/game_speed));
      sb.updateTerminal();
      //Add a new segment to the snake's body, at the position of the food.
      snake.grow();
      //Respawn food
      food.spawn(boundary);
      //Increase game speed & check if game speed is below max speed.
      if (game_speed>MAX_SPEED){
      game_speed = int(game_speed*(float(SPEED_MULTIPLIER)/100.0));
      } else game_speed=MAX_SPEED;
    }
    //check if the snake has eaten the powerup
    if(powerup.checkCollision()){
      //activates powerup effects
      powerup.activatePowerUp(game_speed, SELF_COLLISION);
      if(powerup.type == 1){
        //updates scoreboard to reflect the new speed if slow-mo powerup was consumed.
        sb.setSpeed(1000/game_speed);
        sb.updateTerminal();
      }
      powerup.row = -1;
      isPowerUpSpawned = false;
    }

    //check if the power up should be deactivated
    if(powerup.powerUpExpired() && powerup.powerUpActive){
      //deactivates powerup effects
      powerup.deactivatePowerUp(game_speed, SELF_COLLISION);
      if(powerup.type == 1){
        //updates scoreboard to show new speed if the slow-mo powerup was used.
        sb.setSpeed(1000/game_speed);
        sb.updateTerminal();
      }
    }
    //Check for collisions after the snake has moved
    if (snake.checkSelfCollision() || snake.checkBoundaryCollision())
    {
      //Handle collision (End the game)
      alive=false;
    }

    //Seperate loop for parsing input irrespective of whether we are waiting for the next frame
    //Sleep to control the speed of the game
    int clock = 0;
    while(clock<=game_speed*10){
      input=getInput();
      if (input == 'w' || input == 'a' || input == 's' || input == 'd')
      {
        //Change snake direction based on user input
        snake.changeDirection(input);
      }
      if(input==PAUSE_KEY) 
      {
        pauseMenu("", t, alive);
        //Prevents snake grid flicker before main menu
        if (alive){
        t.clearGrid();
        createGrid(screen_size, t);
        }
      }
      clock++;
      usleep(100);
    }
  }
}

//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~//

//Function definitions used for menu navigation
//Most of these are functionally identical
void pauseMenu(string text, Terminal& t, bool& game_state)
{
  vector<string> ts = {"GAME CURRENTLY PAUSED"};
  vector<string> ps = {"RESUME", "QUIT"};

  Menu m(ts, ps, t);

  t.clearGrid();
  bool init = true;
  while(true)
  {
    if (init)
    {
      m.updateTerminal();
      t.draw();
      init=false;
    }
    switch (getInput()) {
      case 'w':
        m.moveCursor(-1);
        m.updateTerminal();
        t.draw();
        break;
      case 's':
        m.moveCursor(1);
        m.updateTerminal();
        t.draw();
        break;
      case '\n':
        //Nested switch statement for when a selection is chosen in the above menu
        //Prompts a menu to change the selected individual value
        switch(m.getSelection())
        {
          case 1:
            return;
          case 2:
            game_state=false;
            return;
        }
    }

  }
}

void intInputMenu(string text, Terminal& t, int& to_set)
{
  vector<string> ts = {text, "where the currently displayed number goes"};
  vector<string> ps = {"PRESS ENTER TO CONFIRM (C TO CANCEL)"};
  string current_value = "";

  bool active = true;
  bool init = true;
  while(active)
  {
    
    //Initially populates the screen
    if (init) 
    {
      ts[1] = "Current Value: " + current_value;
      Menu m(ts, ps, t);
      m.updateTerminal();
      t.draw();
      //Keep this specific block from running
      init=false;
    }

    char input = getInput();
    if (input>='0' && input<='9') 
    {
      current_value += input;
      ts[1] = "Current Value: " + current_value;
      Menu m(ts, ps, t);
      m.updateTerminal();
      t.draw();
    }
    switch (input) 
    {
      case '\n':
        to_set = stoi(current_value);
        t.clearGrid();
        return;
      case 'c':
        t.clearGrid();
        return;
    }
  }
}

void boolInputMenu(string text, Terminal& t, bool& to_set)
{
  //Menu printouts
  vector<string> ts = {text};
  vector<string> ps = {"TRUE", "FALSE"};
  //Initiate menu
  Menu m(ts, ps, t);
  
  //Flags for while loop
  bool active = true;
  bool force_menu = true;
  while(active)
  {
    //Force menu to print, this clears the screen so only the menu shows up properly
    if (force_menu) {
      m.updateTerminal();
      t.draw();
      force_menu = false;
    }
    //Switch for moving cursor around based on input
    switch (getInput())
    {
      case 'w':
        m.moveCursor(-1);
        m.updateTerminal();
        t.draw();
        break;
      case 's':
        m.moveCursor(1);
        m.updateTerminal();
        t.draw();
        break;
      //If enter is pressed, grab input and change value accordingly
      case '\n':
        switch(m.getSelection())
        {
          case 1:
            to_set=true;
            return;
          case 2:
            to_set=false;
            return;
        }
    }
  }   
}

void charInputMenu(string text, Terminal& t, char& to_set)
{
  vector<string> ts = {text, "where the currently displayed number goes"};
  vector<string> ps = {"PRESS ENTER TO CONFIRM (C TO CANCEL)"};
  char current_value;

  bool active = true;
  bool init = true;
  while(active)
  {
    
    //Initially populates the screen
    if (init) 
    {
      ts[1] = "Current Value: " + string(1, current_value);
      Menu m(ts, ps, t);
      m.updateTerminal();
      t.draw();
      //Keep this specific block from running
      init=false;
    }

    char input = getInput();
    if (input != '\n' &&  input != 0) 
    {

      current_value = input;
      ts[1] = "Current Value: " + string(1, current_value);
      Menu m(ts, ps, t);
      m.updateTerminal();
      t.draw();
    }
    switch (input) 
    {
      case '\n':
        to_set = current_value;
        t.clearGrid();
        return;
      case 'c':
        t.clearGrid();
        return;
    }
  }
}

void styleInputMenu(string text, Terminal& t, CharStyle& to_edit)
{
  t.clearGrid();

  bool force_menu = true;
  vector<string> menu_text = {text, "NAVIGATE UP & DOWN WITH 'w' & 's'", "PRESS ENTER TO SELECT AN OPTION TO EDIT & C TO CANCEL"};
  vector<string> menu_options = {"BOLD", "ITALIC", "UNDERLINE", "BLINKING", "FOREGROUND COLOR", "BACKGROUND COLOR"};
  Menu m(menu_text, menu_options, t);

  while (true)
  {
    if (force_menu)
    {
      m.updateTerminal();
      t.draw();
    }

    switch (getInput())
    {
      case 's':
        m.moveCursor(1);
        m.updateTerminal();
        t.draw();
        break;
      case 'w':
        m.moveCursor(-1);
        m.updateTerminal();
        t.draw();
        break;
      case 'c':
        return;
      case '\n':
        //Nested switch to find out what the user pressed enter on and show the according menu item
        switch(m.getSelection()) {
          case 1:
            boolInputMenu("SET BOLD STYLE PREFERENCE", t, to_edit.bold);
            break;
          case 2:
            boolInputMenu("SET ITALIC STYLE PREFERENCE", t, to_edit.italic);
            break;
          case 3:
            boolInputMenu("SET UNDERLINE STYLE PREFERENCE", t, to_edit.underline);
            break;
          case 4:
            boolInputMenu("SET BLINK STYLE PREFERENCE", t, to_edit.blinking);
            break;
          case 5:
            intInputMenu("SET FOREGROUND COLOR", t, to_edit.fg_color);
            break;
          case 6:
            intInputMenu("SET BACKGROUND COLOR", t, to_edit.bg_color);
            break;
        }
    }
  }
}

void styleEditorMenu(Terminal &t)
{
  bool force_menu = true;
  bool active = true;
  vector<string> menu_text = {"STYLE EDITOR", "NAVIGATE UP & DOWN WITH 'w' & 's'", "PRESS ENTER TO SELECT AN OPTION TO EDIT & C TO CANCEL"};
  vector<string> menu_options = {"SNAKE HEAD STYLE", "SNAKE BODY STYLE", "FOOD STYLE", "BARRIER STYLE", "MENU SELECTIONS STYLE", "MENU TEXT STYLE", "CURSOR STYLE", "SCOREBOARD STYLE"};
  Menu m(menu_text, menu_options, t);

  while(active)
  {
    if (force_menu) 
    {
      m.updateTerminal();
      t.draw();
    }
    
    switch (getInput()) {
      case 'w':
        m.moveCursor(-1);
        m.updateTerminal();
        t.draw();
        break;
      case 's':
        m.moveCursor(1);
        m.updateTerminal();
        t.draw();
        break;
      case 'c':
        return;
      case '\n':
        //Nested switch statement for when a selection is chosen in the above menu
        //Prompts a menu to change the selected individual value
        switch(m.getSelection())
        {
          case 1:
            styleInputMenu("EDITING SNAKE HEAD STYLE", t, SNAKE_HEAD);
            break;
          case 2:
            styleInputMenu("EDITING SNAKE BODY STYLE", t, SNAKE_BODY);
            break;
          case 3:
            styleInputMenu("EDITING SNAKE FOOD STYLE", t, SNAKE_FOOD);
            break;
          case 4:
            styleInputMenu("EDITING BARRIER STYLE", t, BARRIER);
            break;
          case 5:
            styleInputMenu("EDITING MENU SELECTIONS STYLE", t, MENU_OPTION);
            break;
          case 6:
            styleInputMenu("EDITING MENU TEXT STYLE", t, MENU_TEXT);
            break;
          case 7:
            styleInputMenu("EDITING CURSOR STYLE", t, CURSOR);
            break;
          case 8:
            styleInputMenu("EDITING SCOREBOARD STYLE", t, SCOREBOARD);
            break;
          case 9:
            styleInputMenu("EDITING GRID STYLE", t, BACKGROUND);
            break;
        }
    }
  }
}

void charEditorMenu(Terminal &t)
{
  bool force_menu = true;
  bool active = true;
  vector<string> menu_text = {"CHARACTER EDITOR", "NAVIGATE UP & DOWN WITH 'w' & 's'", "PRESS ENTER TO SELECT AN OPTION TO EDIT & C TO CANCEL"};
  vector<string> menu_options = {"SNAKE HEAD (UP)", "SNAKE HEAD (DOWN)", "SNAKE HEAD (RIGHT)", "SNAKE HEAD (LEFT)", "SNAKE BODY", "FOOD", "BARRIER", "CURSOR","POWERUP 1 CHAR", "POWERUP 2 CHAR"};
  Menu m(menu_text, menu_options, t);

  while(active)
  {
    if (force_menu) 
    {
      m.updateTerminal();
      t.draw();
    }
    
    switch (getInput()) {
      case 'w':
        m.moveCursor(-1);
        m.updateTerminal();
        t.draw();
        break;
      case 's':
        m.moveCursor(1);
        m.updateTerminal();
        t.draw();
        break;
      case 'c':
        return;
      case '\n':
        //Nested switch statement for when a selection is chosen in the above menu
        //Prompts a menu to change the selected individual value
        switch(m.getSelection())
        {
          case 1:
            charInputMenu("EDITING SNAKE HEAD (UP)", t, SNAKE_HEAD_UP);
            break;
          case 2:
            charInputMenu("EDITING SNAKE HEAD (DOWN)", t, SNAKE_HEAD_DOWN);
            break;
          case 3:
            charInputMenu("EDITING SNAKE HEAD (RIGHT)", t, SNAKE_HEAD_RIGHT);
            break;
          case 4:
            charInputMenu("EDITING SNAKE HEAD (LEFT)", t, SNAKE_HEAD_LEFT);
            break;
          case 5:
            charInputMenu("EDITING SNAKE BODY", t, SNAKE_BODY_CHAR);
            break;
          case 6:
            charInputMenu("EDITING FOOD CHAR", t, FOOD_CHAR);
            break;
          case 7:
            charInputMenu("EDITING BARRIER", t, GRID_BORDER);
            break;
          case 8:
            charInputMenu("EDITING CURSOR", t, CURSOR_CHAR);
            break;
          case 9:
            charInputMenu("EDITING SLOW-MO POWERUP CHAR", t, POWERUP_1_CHAR);
            break;
          case 10:
            charInputMenu("EDITING SELF-COLLISION POWERUP CHAR", t, POWERUP_2_CHAR);
        }
    }
  }
}

void gameplayEditorMenu(Terminal &t)
{
  bool force_menu = true;
  bool active = true;
  vector<string> menu_text = {"GAMEPLAY EDITOR", "NAVIGATE UP & DOWN WITH 'w' & 's'", "PRESS ENTER TO SELECT AN OPTION TO EDIT & C TO CANCEL"};
  vector<string> menu_options = {"INITIAL GAME SPEED", "MAX GAME SPEED", "FOOD SPEED MULTIPLIER", "SELF COLLISION"};
  Menu m(menu_text, menu_options, t);

  while(active)
  {
    if (force_menu) 
    {
      m.updateTerminal();
      t.draw();
    }
    
    switch (getInput()) {
      case 'w':
        m.moveCursor(-1);
        m.updateTerminal();
        t.draw();
        break;
      case 's':
        m.moveCursor(1);
        m.updateTerminal();
        t.draw();
        break;
      case 'c':
        return;
      case '\n':
        //Nested switch statement for when a selection is chosen in the above menu
        //Prompts a menu to change the selected individual value
        switch(m.getSelection())
        {
          case 1:
            intInputMenu("INITIAL GAME SPEED IN MILLISECONDS/FRAME (1s=1000msec)", t, INITIAL_SPEED);
            break;
          case 2:
            intInputMenu("MAX POSSIBLE SPEED OF GAME IN MILLISECONDS/FRAME", t, MAX_SPEED);
            break;
          case 3:
            intInputMenu("MULTIPLIES THE INITIAL GAME SPEED BY THIS VALUE % WHEN FOOD IS GATHERED", t, SPEED_MULTIPLIER);
            break;
          case 4:
            boolInputMenu("ENABLE OR DISABLE SELF COLLISION", t, SELF_COLLISION);
            break;
        }
    }
  }
}
void powerupEditorMenu(Terminal &t)
{
  bool force_menu = true;
  bool active = true;
  vector<string> menu_text = {"POWERUP EDITOR", "NAVIGATE UP & DOWN WITH 'w' & 's'", "PRESS ENTER TO SELECT AN OPTION TO EDIT & C TO CANCEL"};
  vector<string> menu_options = {"ENABLE POWERUPS", "POWERUP EFFECT TIME", "POWERUP SPAWN TIME", "SLOW-MO POWERUP TIME"};
  Menu m(menu_text, menu_options, t);

  while(active)
  {
    if (force_menu) 
    {
      m.updateTerminal();
      t.draw();
    }
    
    switch (getInput()) {
      case 'w':
        m.moveCursor(-1);
        m.updateTerminal();
        t.draw();
        break;
      case 's':
        m.moveCursor(1);
        m.updateTerminal();
        t.draw();
        break;
      case 'c':
        return;
      case '\n':
        //Nested switch statement for when a selection is chosen in the above menu
        //Prompts a menu to change the selected individual value
        switch(m.getSelection())
        {
          case 1:
            boolInputMenu("ENABLE OR DISABLE POWERUPS SPAWNING IN THE GAME", t, ENABLE_POWERUPS);
            break;
          case 2:
            intInputMenu("AMOUNT OF TIME (IN SECONDS) A POWERUP'S EFFECTS LAST", t, POWERUP_TIME);
          case 3:
            intInputMenu("AMOUNT OF TIME (IN SECONDS) IT TAKES FOR ANOTHER POWERUP TO SPAWN (AFTER THE PREVIOUS POWERUP EFFECTS ARE OVER)", t, SPEED_MULTIPLIER);
            break;
          case 4:
            boolInputMenu("#EXPERIMENTAL# RATIO OF CHANGE IN GAMESPEED FOR THE SLO-MO POWER UP (DEFAULT (2) HALVES THE GAMESPEED)", t, SELF_COLLISION);
            break;
        }
    }
  }
}
void settingsEditorMenu(Terminal &t)
{
  bool force_menu = true;
  bool active = true;
  vector<string> menu_text = {"SETTINGS EDITOR", "NAVIGATE UP & DOWN WITH 'w' & 's'", "PRESS ENTER TO SELECT AN OPTION TO EDIT & C TO CANCEL"};
  vector<string> menu_options = {"STYLE EDITOR", "CHAR EDITOR", "GAMEPLAY EDITOR", "POWERUP EDITOR"};
  Menu m(menu_text, menu_options, t);

  while(active)
  {
    if (force_menu) 
    {
      m.updateTerminal();
      t.draw();
    }
    
    switch (getInput()) {
      case 'w':
        m.moveCursor(-1);
        m.updateTerminal();
        t.draw();
        break;
      case 's':
        m.moveCursor(1);
        m.updateTerminal();
        t.draw();
        break;
      case 'c':
        return;
      case '\n':
        //Nested switch statement for when a selection is chosen in the above menu
        //Prompts a menu to change the selected individual value
        switch(m.getSelection())
        {
          case 1:
            styleEditorMenu(t);
            break;
          case 2:
            charEditorMenu(t);
            break;
          case 3:
            gameplayEditorMenu(t);
            break;
          case 4:
            powerupEditorMenu(t);
        }
    }
  }
}
#endif