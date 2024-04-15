/*
Menu.hpp is only a haeder file used to keep the clutter of the following

*/

#ifndef MENU_HPP //Redundancy protection
#define MENU_HPP

#include <iostream>
#include <vector>
#include <unistd.h>
#include <exception>

using namespace std;

using ipair = pair<int, int>; //Type alias for "integer pair"
using pvector = vector<pair<int, int>>; //Type alias for "pair vector"

char CURSOR_CHAR = '>';

//Default gameplay parameters
int INITIAL_SPEED = 125;
int MAX_SPEED = 50;
int SPEED_MULTIPLIER = 90;
bool SELF_COLLISION = true;
bool ENABLE_POWERUPS = true;
int POWERUP_TIME = 10;
int POWERUP_SPAWN_TIME = 30;
char PAUSE_KEY = 'p';

//Default characters
char CURSOR_UP = 'w';
char CURSOR_DOWN = 's';
char SNAKE_HEAD_UP = '^';
char SNAKE_HEAD_DOWN = 'v';
char SNAKE_HEAD_RIGHT = '>';
char SNAKE_HEAD_LEFT = '<';
char FOOD_CHAR = '*';
char SNAKE_BODY_CHAR = '*';
char GRID_BORDER = '#';
char POWERUP_1_CHAR = '+';
char POWERUP_2_CHAR = 'x';

//Struct used to organize style presets for characters
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

//Character style presets
CharStyle MENU_TEXT(false, false, false, false, 232, 231);
CharStyle MENU_OPTION(false, false, false, false, 232, 231);
CharStyle CURSOR(false, false, false, true, 220, 232);
CharStyle SNAKE_BODY(false, false, false, false, 35, 118);
CharStyle SNAKE_HEAD(false, false, false, false, 160, 40);
CharStyle SNAKE_FOOD(false, false, false, false, 231, 232);
CharStyle BARRIER(false, false, false, false, 231, 232);
CharStyle POWERUP1(false, false, false, false, 231, 232);
CharStyle POWERUP2(false, false, false, false, 231, 232);
CharStyle SCOREBOARD(false, false, false, false, 231, 232);

/*
  setBoundary takes the size of the screen and clips 1 off of both dimensions
*/
ipair setBoundary(ipair screen_size)
{
  int rows = screen_size.first - 1;
  int columns = screen_size.second - 1;
  return {rows, columns};
}

/*
    Snake Class:
    This class represents the snake in the terminal Grid. The snake can move up, down, left, right, 
    and grows whenever it 'eats' (collides with) food. The Snake class provides functions that can control
    the snake's movement, change its direction, retrieve its body, and increase its size.
*/
class Snake
{
private:
  pvector body;  //Vector to store the coordinates of the snake's body bodySegments
  char direction;  //Current direction of the snake ('a' for left, 'd' for right, 'w' for up, 's' for down)
  ipair prevTailPosition;  //Previous position of the tail segment

public:
  /*
    Constructor for Snake class

    Params: 2 integers, 1 character
    int init_row : initial row for spawning
    int init_column : initial column for spawning
    char init_direction : initial direction of the snake
    */
  Snake(int init_row, int init_column, char init_direction)
  {
    //Initialize the snake with a single body segment at the starting position
    body.push_back({init_row, init_column});
    direction = init_direction;
    prevTailPosition = {init_row, init_column};
  }

  /*
    //move definition here
  */
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

    //Update the previous position of the end/tail segment of the body
    prevTailPosition = body.back();

    //Update the coordinates of the body segments to follow the head
    for (size_t i = body.size() - 1; i > 0; --i)
    {
      body[i] = body[i - 1];
    }

    //Calculate the new position of the head
    ipair &head = body.front();
    head.first += row_offset;
    head.second += column_offset;
  }

  /*
    //changeDirection definition here
  */
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

  //Function to retrieve the snake's body
  const pvector &getBody() const
  {
    return body;
  }

  //Function to retrieve the previous position of the tail bodySegment
  ipair getPrevTailPosition() const
  {
    return prevTailPosition;
  }

  //Function to retrieve the current direction of the snake
  char getDirection() const
  {
    return direction;
  }

  //Function to make the snake grow by adding a new bodySegment
  void grow()
  {
    //last/tail segment of the body.
    const ipair &tailbodySegment = body.back();

    //Calculate the coordinates for the new body segment
    int new_row = tailbodySegment.first;
    int new_column = tailbodySegment.second;

    //Add the new body segment in a position adjacent to the tail bodySegment, depending on the direction
    switch (direction)
    {
    case 'a':    
      new_column += 1; //Move the new body segment one column to the left
      break;
    case 'd':    
      new_column -= 1; //Move the new body segment one column to the right
      break;
    case 'w':   
      new_row += 1; //Move the new body segment one row up
      break;
    case 's':   
      new_row -= 1; //Move the new body segment one row down
      break;
    }

    //Add the new body segment to the end of the snake body vector, effectively extending the tail/end.
    body.push_back({new_row, new_column});
  }
};

/*
Food Struct:
  Represents 'food' in the snake game. It has a position defined by row and column coordinates.
  The spawn method randomly places the food within the grid boundaries. checkCollision method determines if the
  food collides with the snake by comparing its coordinates with the coordinates of the snake's body segments.
*/
struct Food
{
  int row = -1;
  int col = -1;
  //Method to spawn food at a random location on the grid
  void spawn(ipair boundary, Snake &snake)
  {
    //Generate random row and column within the grid boundaries
    do{
      row = rand() % (boundary.first - 4) + 4;
      col = rand() % (boundary.second - 1) + 1;
    }while(checkCollision(snake));
  }

  //Method to check the food collision with the snake
  bool checkCollision(const Snake &snake) const
  {
    //Iterate over the snake's body bodySegments and check if any bodySegment matches the food's position
    for (const ipair &bodySegment : snake.getBody())
    {
      if (row == bodySegment.first && col == bodySegment.second)
      {
        return true; //Collision detected
      }
    }
    return false; //No collision
  }
};

/*
Powerups Structs:
  Represents unique powerups in the game. Has a spawn method and checkCollision to check for viable
  spawn positions where it does not collide with the existing snake.
*/
struct Powerups
{
  int row = -1;
  int col =-1;
  int type;

  void spawn(ipair boundary, Snake &snake){
    do{
      row = rand() % (boundary.first - 4) + 4;
      col = rand() % (boundary.second - 1) + 1;
      type = rand() % 2 + 1;
    }while(checkCollision(snake));
  }
  bool checkCollision(const Snake &snake) const
  {
    //Iterate over the snake's body bodySegments and check if any bodySegment matches the Powerup's position
    for (const ipair &bodySegment : snake.getBody())
    {
      if (row == bodySegment.first && col == bodySegment.second)
      {
        return true; //Collision detected
      }
    }
    return false; //No collision
  }
};

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
    Terminal& t;  //Terminal reference
    const vector<string> menu_options; //Menu selections
    const vector<string> menu_text; //Menu header text
    const ipair center;  //A constant with the coordinates of the current center point
    const int cursor_column_offset = 2;  //The horizontal distance between the cursor and the first letter of an option
    const int text_option_seperation = 1;  //The amount of new lines between last header line and first option line
    const int left_justification_column;  //The column coordinates where left justified text begins printing
    const int max_cursor_position;  //The maximum position the cursor can occupy
    int cursor_position = 1;  //The current position of the cursor

    /*
    Exports the current state of the menu display to the active Terminal class display grid
    note: Does not draw the current terminal state, .draw() must be called after this method is called

    Params: None

    Returns: Void
    */
    void pushToTerminal(){
      //Zero out display grid to build menu display
      t.clearGrid();
      //Vertically centered row for header lines to begin
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


class ScoreBoard
{
  public:
    ScoreBoard(Terminal& tin) : t(tin) {};

    void updateTerminal() {
      pushToTerminal();
      return;
    }

    void scoreEvent(){
      current_score++;
      return;
    }

    void setSpeed(int spd){
      current_speed = spd;
      return;
    }

  private:
    int current_score = 0;
    int current_speed = 0;
    Terminal& t;

    //coordinates of the upper left corner and bottom right corner of the scoreboard
    const string score_prefix = "SCORE / TOTAL-FOOD: ";
    const string speed_prefix = "SPEED   (TILE/SEC): ";
    const int center_line = t.findCenter().second;
    //Roughly centers the text and finds the left justification line
    const int left_justification_offset = -1*((score_prefix.size()+speed_prefix.size())/4);

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

//Creates the barrier surrounding the playable area according to the size of the screen
void createGrid(ipair screen_size, Terminal &t)
{
  ipair boundary = setBoundary(screen_size);

  for (int i = 3; i <= boundary.first; i++)
  {
    t.setChar(i, 0, GRID_BORDER, BARRIER.bold, BARRIER.italic, BARRIER.underline, BARRIER.blinking, BARRIER.fg_color, BARRIER.bg_color);
    t.setChar(i, boundary.second, GRID_BORDER, BARRIER.bold, BARRIER.italic, BARRIER.underline, BARRIER.blinking, BARRIER.fg_color, BARRIER.bg_color);
  }
  for (int j = 0; j <= boundary.second; j++)
  {
    t.setChar(3, j, GRID_BORDER);
    t.setChar(boundary.first, j, GRID_BORDER, BARRIER.bold, BARRIER.italic, BARRIER.underline, BARRIER.blinking, BARRIER.fg_color, BARRIER.bg_color);
  }

  for (int i = 4; i < boundary.first; i++)
  {
    for (int j = 1; j < boundary.second; j++)
    {
      t.setChar(i, j, ' ');
    }
  }

  t.draw();
}

/*
    drawSnake Function:
    This function draws the Snake onto the grid. Params: Snake and Terminal objects.
    Iterates through the body bodySegments of snake, and draws each bodySegment using the specified body character.
    The function erases the previous position of the snake's tail bodySegment and then draws the updated grid with the snake.
*/
void drawSnake(const Snake &snake, Terminal &t)
{
  //Get the body of the snake
  const pvector &body = snake.getBody();
  //Define the characters to represent the snake's body
  char headChar;
  switch (snake.getDirection())
  {
  case 'a': 
    headChar = SNAKE_HEAD_LEFT;
    break;
  case 'd': 
    headChar = SNAKE_HEAD_RIGHT;
    break;
  case 'w': 
    headChar = SNAKE_HEAD_UP;
    break;
  case 's':
    headChar = SNAKE_HEAD_DOWN;
    break;
  default: //Raise error if invalid head direction is found
    cerr << "Current head direction: " << snake.getDirection();
    throw logic_error("Invalid head direction while drawing snake");
  }

  //Loop to print body body segments (if there are any)
  for (const ipair &bodySegment:body) {
    t.setChar(bodySegment.first, bodySegment.second, SNAKE_BODY_CHAR, SNAKE_BODY.bold, SNAKE_BODY.italic, SNAKE_BODY.underline, SNAKE_BODY.blinking, SNAKE_BODY.fg_color, SNAKE_BODY.bg_color);
  }
  
  //Draw the snake's head
  const ipair& headPosition = body.front();
  t.setChar(headPosition.first, headPosition.second, headChar, SNAKE_HEAD.bold, SNAKE_HEAD.italic, SNAKE_HEAD.underline, SNAKE_HEAD.blinking, SNAKE_HEAD.fg_color, SNAKE_HEAD.bg_color);

  //Erase the previous position of the tail bodySegment
  ipair prevTailPosition = snake.getPrevTailPosition();
  t.setChar(prevTailPosition.first, prevTailPosition.second, ' ');

  //Draw the updated grid with the snake
  t.draw();
}

/*
    checkBodyCollision Function:
    This function checks to see if the snake head collides with its body. It takes the snake object as a 
    parameter, and iterates through the body bodySegments (starting from the second bodySegment) then compares the coordinates 
    with the head's coordinates. If any of them match (indicating collision at those coordinates), it returns true. 
    Otherwise, it returns false, indicating there is no collision.
*/
bool checkBodyCollision(const Snake &snake)
{
  //Get body of the snake using getBody
  const pvector &body = snake.getBody();

  //Get the coordinates of the head
  ipair head = body.front();

  //Iterate over the body bodySegments starting from the third bodySegment
  for (auto i = body.begin() + 1; i != body.end(); i++)
  {
    //If the coordinates of the current bodySegment match the head's coordinates, return true
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
bool checkBoundaryCollision(const Snake &snake, ipair screen_size)
{
  //Get the coordinates of the head
  ipair head = snake.getBody().front();
  ipair boundary = setBoundary(screen_size);
  //Check if the head's coordinates exceed the boundaries
  if (head.first == 3 || head.first == boundary.first || head.second == 0 || head.second == boundary.second)
  {
    return true; //Collision detected
  }
  //No collision detected
  return false;
}

/*
Menu Functions:
  These all function almost identically but serve different categories or data, types.
  They take in a prompt to display, a terminal reference, and a reference to a variable to be modified.
*/
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
  vector<string> menu_options = {"SNAKE HEAD STYLE", "SNAKE BODY STYLE", "FOOD STYLE", "BARRIER STYLE", "MENU SELECTIONS STYLE", "MENU TEXT STYLE", "CURSOR STYLE", "SPEED POWERUP", "COLLISION POWERUP", "SCOREBOARD STYLE"};
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
            styleInputMenu("EDITING SPEED POWERUP STYLE", t, POWERUP1);
            break;
          case 9:
            styleInputMenu("EDITING COLLISION POWERUP STYLE", t, POWERUP2);
            break;
          case 10:
            styleInputMenu("EDITING SCOREBOARD STYLE", t, SCOREBOARD);
        }
    }
  }
}

void charEditorMenu(Terminal &t)
{
  bool force_menu = true;
  bool active = true;
  vector<string> menu_text = {"CHARACTER EDITOR", "NAVIGATE UP & DOWN WITH 'w' & 's'", "PRESS ENTER TO SELECT AN OPTION TO EDIT & C TO CANCEL"};
  vector<string> menu_options = {"SNAKE HEAD (UP)", "SNAKE HEAD (DOWN)", "SNAKE HEAD (RIGHT)", "SNAKE HEAD (LEFT)", "SNAKE BODY", "FOOD", "BARRIER", "CURSOR",
                                 "SPEED POWERUP", "COLLISION POWERUP"};
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
            charInputMenu("EDITING SPEED POWERUP", t, POWERUP_1_CHAR);
          case 10:
            charInputMenu("EDITING COLLISION POWERUP", t, POWERUP_2_CHAR);
        }
    }
  }
}

void gameplayEditorMenu(Terminal &t)
{
  bool force_menu = true;
  bool active = true;
  vector<string> menu_text = {"GAMEPLAY EDITOR", "NAVIGATE UP & DOWN WITH 'w' & 's'", "PRESS ENTER TO SELECT AN OPTION TO EDIT & C TO CANCEL"};
  vector<string> menu_options = {"INITIAL GAME SPEED", "MAX GAME SPEED", "FOOD SPEED MULTIPLIER", "SELF COLLISION", "POWERUPS OVERRIDE", "POWERUP DURATION", 
                                 "POWERUP SPAWN COOLDOWN"};
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
          case 5:
            boolInputMenu("ENABLE OR DISABLE POWERUP SPAWNING", t, ENABLE_POWERUPS);
            break;
          case 6:
            intInputMenu("ADJUST POWERUP DURATION (SECONDS)", t, POWERUP_TIME);
            break;
          case 7:
            intInputMenu("ADJUST POWERUP SPAWN COOLDOWN", t, POWERUP_SPAWN_TIME);
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
  vector<string> menu_options = {"STYLE EDITOR", "CHAR EDITOR", "GAMEPLAY EDITOR"};
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
        }
    }
  }
}

/*
  Playgame is the primary loop that manages gameplay

  Parameters: 1 Snake Reference, 1 Terminal Reference, And 1 pair representing the size of the screen
*/
void playGame(Snake &snake, Terminal &t, ipair screen_size, ScoreBoard &sb)
{
  
  //Initialize random seed
  srand(time(NULL));
  Food food;
  Powerups powerup;
  int game_speed = INITIAL_SPEED;
  ipair boundary = setBoundary(screen_size);
  int prevRow = -1, prevCol = -1; //Initialize previous position
  createGrid(screen_size, t);
  bool alive = true; 
  char input = getInput();
  bool activePowerup = false;
  bool powerupActive = false;
  int powerupStartTime;
  int powerupCurrentTime;
  int elaspedPowerUpTime;
  int powerUpCompleteTime;
  int currentTime;
  bool powerUpSpawned=false;
  while (alive)
  {

    sb.updateTerminal();

    //Move the snake
    snake.move();

    //Check if the food needs to be respawned
    if (food.row == -1)
    {
      food.spawn(boundary, snake);
    }

    if (ENABLE_POWERUPS == true) {
    //Check if the power-up needs to be respawned
      if((powerup.row == -1) || (((currentTime - powerUpCompleteTime) > POWERUP_SPAWN_TIME) && (!powerUpSpawned))){
        powerup.spawn(boundary, snake);
        powerUpSpawned = true;
        //Draw the power-up
        char powerupChar;
        if (powerup.type == 1) {
          powerupChar = POWERUP_1_CHAR; //Speed power-up symbol
          t.setChar(powerup.row, powerup.col, powerupChar, POWERUP1.bold, POWERUP1.italic, POWERUP1.underline, POWERUP1.blinking, POWERUP1.fg_color, POWERUP1.bg_color);
        } 
        else if (powerup.type == 2) {
          powerupChar = POWERUP_2_CHAR; //No-collision power-up symbol
          t.setChar(powerup.row, powerup.col, powerupChar, POWERUP2.bold, POWERUP2.italic, POWERUP2.underline, POWERUP2.blinking, POWERUP2.fg_color, POWERUP2.bg_color);
        }
      }

      //Check for collision with power-up
      if (powerup.checkCollision(snake)) {
        if(!activePowerup){
          if (powerup.type == 1) {
            game_speed *= 2;
            activePowerup = true;
            powerupStartTime = time(NULL);
            powerUpSpawned = false;
          } 
          else if(powerup.type == 2) {
            SELF_COLLISION = false;
            activePowerup = true;
            powerupStartTime = time(NULL);
            powerUpSpawned = false;
          }
        }
      t.setChar(powerup.row, powerup.col, ' ');
      }
    }

    //Clear the previous position of the snake's head
    if (prevRow != -1 && prevCol != -1)
    {
      t.setChar(prevRow, prevCol, ' ');
    }
    //draw food
    t.setChar(food.row, food.col, FOOD_CHAR, SNAKE_FOOD.bold, SNAKE_FOOD.italic, SNAKE_FOOD.underline, SNAKE_FOOD.blinking, SNAKE_FOOD.fg_color, SNAKE_FOOD.bg_color);

    //Draw the snake
    drawSnake(snake, t);

    //Get the new position of the snake's head
    ipair headPosition = snake.getBody().front();
    prevRow = headPosition.first;
    prevCol = headPosition.second;

    //Check if the snake has eaten the food
    if (food.checkCollision(snake))
    {
      //Tell scoreboard to update
      sb.scoreEvent();
      sb.setSpeed((1000/game_speed));
      sb.updateTerminal();
      //Snake eats food, grow the snake
      //Add a new bodySegment to the snake's body at the position of the food
      snake.grow();
      //Respawn food
      food.spawn(boundary, snake);
      //Increase game speed & check if game speed is below max speed.
      if (game_speed>MAX_SPEED){
      game_speed = int(game_speed*(float(SPEED_MULTIPLIER)/100.0));
      } else game_speed=MAX_SPEED;
    }
    if(activePowerup)
    {
      powerupCurrentTime = time(NULL);
      elaspedPowerUpTime = powerupCurrentTime - powerupStartTime;
      if(elaspedPowerUpTime > POWERUP_TIME){
        activePowerup = false;
        if(powerup.type == 1) game_speed /= 2;
        else if(powerup.type == 2) SELF_COLLISION = true;
        powerUpCompleteTime = time(NULL);
      }
    }
    //Check for collisions after the snake has moved
    if (checkBodyCollision(snake) || checkBoundaryCollision(snake, screen_size))
    {
      //Handle collision (e.g., end the game)
      alive=false;
    }
    currentTime = time(NULL);
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
        if (alive){ //Prevents flicker when quitting to homescreen
        t.clearGrid();
        createGrid(screen_size, t);
        }
      }
      clock++;
      usleep(100);
    }
  }
}

#endif