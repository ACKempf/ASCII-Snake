#include <iostream>
#include <vector>
#include <unistd.h>
#include <exception>
#include "TControl.hpp"

using namespace std;
//Some type aliases for readability
using ipair = pair<int, int>;
using pvector = vector<pair<int, int>>;

//Pseudo-constants for various things
bool MENU_OPTION_BOLD = false;
bool MENU_OPTION_ITALIC = false;
bool MENU_OPTION_UNDERLINE = false;
bool MENU_OPTION_BLINK = false;
int MENU_OPTION_FG_COLOR = 231;
int MENU_OPTION_BG_COLOR = 232;
bool MENU_TEXT_BOLD = false;
bool MENU_TEXT_ITALIC = false;
bool MENU_TEXT_UNDERLINE = false;
bool MENU_TEXT_BLINK = false;
int MENU_TEXT_FG_COLOR = 231;
int MENU_TEXT_BG_COLOR = 232;
char CURSOR_CHAR = '>';
bool CURSOR_BOLD = true;
bool CURSOR_ITALIC = true;
bool CURSOR_UNDERLINE = false;
bool CURSOR_BLINK = true;
int CURSOR_FG_COLOR = 220;
int CURSOR_BG_COLOR = 232;
char CURSOR_UP = 'w';
char CURSOR_DOWN = 's';

//Pseudo-constants for snake
int SNAKE_BODY_COLOR = 230;
int SNAKE_HEAD_COLOR = 230;
int SNAKE_HEAD_BG = 230;
int SNAKE_BODY_BG = 230;
char FOOD_CHAR = '*';
int FOOD_COLOR = 214;
char SNAKE_HEAD_UP = '^';
char SNAKE_HEAD_DOWN = 'v';
char SNAKE_HEAD_RIGHT = '>';
char SNAKE_HEAD_LEFT = '<';
char SNAKE_BODY_CHAR = '*';

//Pseudo-constants for gameplay
int GAME_SPEED = 90000;
bool SELF_COLLISION = true;

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
          t.setChar(current_row, left_justification_column+current_column_offset, c, MENU_TEXT_BOLD, MENU_TEXT_ITALIC, MENU_TEXT_UNDERLINE, MENU_TEXT_BLINK, MENU_TEXT_FG_COLOR, MENU_TEXT_BG_COLOR);
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
          t.setChar(current_row, left_justification_column+current_column_offset, c, MENU_OPTION_BOLD, MENU_OPTION_ITALIC, MENU_OPTION_UNDERLINE, MENU_OPTION_BLINK, MENU_OPTION_FG_COLOR, MENU_OPTION_BG_COLOR);
          current_column_offset++;
        }
        //Adds visual cursor if current line cursor position matches active cursor position
        if (selection_cursor_index == cursor_position) t.setChar(current_row, left_justification_column-cursor_column_offset, CURSOR_CHAR, CURSOR_BOLD, CURSOR_ITALIC, CURSOR_UNDERLINE, CURSOR_BLINK, CURSOR_FG_COLOR, CURSOR_BG_COLOR);

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

//Creates a menu to gather user input for settings purposes, prints a prompt
void intInputMenu(string text, Terminal& t, int& to_set);
void boolInputMenu(string text, Terminal& t, bool& to_set);
void charInputMenu(string text, Terminal& t, char& to_set);
void settingsMenu(Terminal &t);

/*
    Snake Class:
    This class represents a snake in a 2D grid-based game. The snake can move in four directions: up, down, left, and right.
    It grows by adding segments to its body when it consumes food. The class provides functions to control
    the snake's movement, change its direction, retrieve its body segments, and make it grow.
*/
class Snake
{
private:
  pvector body;      // Vector to store the coordinates of the snake's body segments
  char direction;    // Current direction of the snake ('a' for left, 'd' for right, 'w' for up, 's' for down)
  ipair prevTailPos; // Previous position of the tail segment

public:
  // Constructor to initialize the snake with a starting position and direction
  Snake(int init_row, int init_column, char init_direction)
  {
    // Initialize the snake with a single segment at the starting position
    body.push_back({init_row, init_column});
    direction = init_direction;
    prevTailPos = {init_row, init_column};
  }

  // Function to move the snake in the current direction
  void move()
  {
    // Calculate the offset for the new head position based on the current direction
    int row_offset = 0, column_offset = 0;
    switch (direction)
    {
    case 'a': // Left
      row_offset = 0;
      column_offset = -1;
      break;
    case 'd': // Right
      row_offset = 0;
      column_offset = 1;
      break;
    case 'w': // Up
      row_offset = -1;
      column_offset = 0;
      break;
    case 's': // Down
      row_offset = 1;
      column_offset = 0;
      break;
    }

    // Update the previous position of the tail segment
    prevTailPos = body.back();

    // Update the positions of the body segments to follow the head
    for (size_t i = body.size() - 1; i > 0; --i)
    {
      body[i] = body[i - 1];
    }

    // Calculate the new head position
    ipair &head = body.front();
    head.first += row_offset;
    head.second += column_offset;
  }

  // Function to change the direction of the snake
  void changeDirection(char new_direction)
  {
    //Messy ifs, Only change direction is it is not opposite to the current direciton
    if (!((new_direction == 'w' && direction == 's')||(new_direction == 's' && direction == 'w'))){
      if (!((new_direction == 'a' && direction == 'd')||(new_direction == 'd' && direction == 'a'))) {
        // Update the direction
        direction = new_direction;
      }
    }
    
  }

  // Getter function to retrieve the snake's body
  const pvector &getBody() const
  {
    return body;
  }

  // Getter function to retrieve the previous position of the tail segment
  ipair getPrevTailPos() const
  {
    return prevTailPos;
  }

  // Getter function to retrieve the current direction of the snake
  char getDirection() const
  {
    return direction;
  }

  // Function to make the snake grow by adding a new segment
  void grow()
  {
    // Get the last segment of the snake's body
    const ipair &tailSegment = body.back();

    // Calculate the position for the new segment
    int new_row = tailSegment.first;
    int new_column = tailSegment.second;

    // Add the new segment in a position adjacent to the tail segment, depending on the direction
    switch (direction)
    {
    case 'a':    // Left
      new_column += 1; // Move the new segment one column to the left
      break;
    case 'd':    // Right
      new_column -= 1; // Move the new segment one column to the right
      break;
    case 'w':    // Up
      new_row += 1; // Move the new segment one row up
      break;
    case 's':    // Down
      new_row -= 1; // Move the new segment one row down
      break;
    }

    // Add the new segment to the end of the body vector, extending the tail
    body.push_back({new_row, new_column});
  }
};

int setBoundary(ipair screen_size)
{
  int rows = screen_size.first;
  int columns = screen_size.second;
  int boundary;
  if (rows > columns)
    boundary = columns - 1;
  else
    boundary = rows - 1;
  return boundary;
}

void playGame(Snake &snake, Terminal &t, ipair screensize);
void drawSnake();
void createGrid(ipair screensize);
bool checkBoundaryCollision(const Snake &snake, ipair screensize);
bool checkSelfCollision(const Snake &snake);

/*
    Food Struct:
    represents the food in the snake game. It has a position defined by row and column coordinates.
    The spawn method randomly places the food within the grid boundaries. The checkCollision method is used to determine if the
    food collides with the snake by comparing its position with the positions of the snake's body segments.
*/
struct Food
{
  int row = -1;
  int col = -1;
  // Method to spawn food at a random location on the grid
  void spawn(int boundary)
  {
    // Generate random row and column within the grid boundaries
    row = rand() % (boundary - 1) + 1;
    col = rand() % ((boundary * 2) - 1) + 1;
  }

  // Method to check the food collision with the snake
  bool checkCollision(const Snake &snake) const
  {
    // Iterate over the snake's body segments and check if any segment matches the food's position
    for (const ipair &segment : snake.getBody())
    {
      if (row == segment.first && col == segment.second)
      {
        return true; // Collision detected
      }
    }
    return false; // No collision
  }
};


int main()
{
  //Enable raw mode & clear
  enableRawMode();
  clear();

  //Input loop for confirming what display size the user wants
  ipair screen_size;
  while(true) {
    clear();
    static char input;

    //Simple printout instructing user
    cout << "Adjust window to desired size and press any key to continue." << endl;
    //A "press any key to continue" break
    cin.ignore();

    //Reassign screen_size to current screen size
    screen_size = getTermSize();
    //Trim off the unusable but represented top row of the screen space
    screen_size.first--;

    //Gets the closest screensize to the selected that is odd
    //Used because only odd screen dimensions have a true "center character"
    if (screen_size.first%2 == 0) screen_size.first--;
    if (screen_size.second%2 == 0) screen_size.second--;

    //Prompts the user to confirm the current display dimensions
    cout << "Enter y/Y to confirm display size: " << screen_size.first << "x" << screen_size.second << endl;
    cin >> input;
    //Break user-dimension input loop with current screen dimensions set
    if (input=='y'||input=='Y') break;
  }

  //Create Terminal object of the correct size
  Terminal t(screen_size.first, screen_size.second);
  t.setCursorVisibility(false);

  //Menu loop, after running it returns the selection as the number presented on the menu

  vector<string> menu_options = {"PLAY", "SETTINGS", "EXIT"};

  string cursor_control_line = "CURSOR CONTROLS: " + string(1, CURSOR_UP) + " TO MOVE UP AND " + string(1, CURSOR_DOWN) + " FOR DOWN";
  vector<string> menu_text = {"", "USE ENTER TO CONFIRM SELECTION"};
  menu_text[0] = cursor_control_line;
  Menu m(menu_text, menu_options, t);

  while(true){
  //Signals the menu to run once regardless of whether there is input available
  bool init_run = true;
  int user_decision;
  bool menu_active = true;
  while (menu_active) {
    if (init_run) {
      m.updateTerminal();
      t.draw();
      init_run=false;
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
      user_decision = m.getSelection();
      menu_active = false;
      break;
    }
  }

  int boundary = setBoundary(screen_size);
  int startX = rand() % (boundary - 2) + 1;
  int startY = rand() % (boundary - 2) + 1;
  char startDirection = 'd';

  Snake snake(startX, startY, startDirection);

  switch (user_decision)
  {
  case 1:
    t.clearGrid();
    playGame(snake, t, screen_size);
    break;
  case 2:
    settingsMenu(t);
    break;
  case 3:
    exit(0);
    break;
  }
  }
}

// Changes grid based on size of screen, always square.
void createGrid(ipair screen_size, Terminal &t)
{
  int boundary = setBoundary(screen_size);

  for (int i = 0; i <= boundary; i++)
  {
    t.setChar(i, 0, '#');
    t.setChar(i, boundary * 2, '#');
  }
  for (int j = 0; j <= boundary * 2; j++)
  {
    t.setChar(0, j, '#');
    t.setChar(boundary, j, '#');
  }

  for (int i = 1; i < boundary; i++)
  {
    for (int j = 1; j < boundary * 2; j++)
    {
      t.setChar(i, j, ' ');
    }
  }

  t.draw();
}

/*
    drawSnake Function:
    This function is responsible for drawing the snake on a terminal screen. It takes the snake object and a terminal object as parameters.
    It iterates through the snake's body segments, drawing each segment using a specified character for the body.
    It also draws the head of the snake according to its direction, using different characters for each direction.
    The function erases the previous position of the snake's tail segment and then draws the updated grid with the snake.
*/
void drawSnake(const Snake &snake, Terminal &t)
{
  // Get the body of the snake
  const pvector &body = snake.getBody();
  // Define the characters to represent the snake's body
  char headChar;
  switch (snake.getDirection())
  {
  case 'a': // Left
    headChar = SNAKE_HEAD_LEFT;
    break;
  case 'd': // Right
    headChar = SNAKE_HEAD_RIGHT;
    break;
  case 'w': // Up
    headChar = SNAKE_HEAD_UP;
    break;
  case 's': // Down
    headChar = SNAKE_HEAD_DOWN;
    break;
  default: //Raise error if invalid head direction is found
    cerr << "Current head direction: " << snake.getDirection();
    throw logic_error("Invalid head direction while drawing snake");
  }

  //Print body segments (if there are any)
  for (const ipair &segment:body) {
    t.setChar(segment.first, segment.second, SNAKE_BODY_CHAR, false, false, false, false, SNAKE_BODY_COLOR, SNAKE_BODY_BG);
  }
  
  // Draw the snake's head
  const ipair& headPos = body.front();
  t.setChar(headPos.first, headPos.second, headChar, false, false, false, false, SNAKE_HEAD_COLOR, SNAKE_HEAD_BG);

  // Erase the previous position of the tail segment
  ipair prevTailPos = snake.getPrevTailPos();
  t.setChar(prevTailPos.first, prevTailPos.second, ' ');

  // Draw the updated grid with the snake
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

bool checkSelfCollision(const Snake &snake)
{
  // Get the body of the snake
  const pvector &body = snake.getBody();

  // Get the coordinates of the head
  ipair head = body.front();

  // Iterate over the body segments starting from the third segment
  for (auto i = body.begin() + 1; i != body.end(); i++)
  {
    // If the coordinates of the current segment match the head's coordinates, return true
    if (*i == head)
    {
      //Only report self collision if SELF_COLLISION flag is true
      if (SELF_COLLISION){
        return true;
      } else return false;
    }
  }

  // No collision detected
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
  // Get the coordinates of the head
  ipair head = snake.getBody().front();
  int boundary = setBoundary(screen_size);
  // Check if the head's coordinates exceed the boundaries
  if (head.first == 0 || head.first == boundary || head.second == 0 || head.second == boundary * 2)
  {
    return true; // Collision detected
  }
  // No collision detected
  return false;
}

void playGame(Snake &snake, Terminal &t, ipair screen_size)
{
  // Initialize random seed
  srand(time(NULL));
  Food food;
  int boundary = setBoundary(screen_size);
  int prevRow = -1, prevCol = -1; // Initialize previous position
  createGrid(screen_size, t);
  while (true)
  {
    // Process user input
    char input = getInput();
    if (input == 'w' || input == 'a' || input == 's' || input == 'd')
    {
      // Change snake direction based on user input
      snake.changeDirection(input);
    }

    // Move the snake
    snake.move();

    // Check if the food needs to be respawned
    if (food.row == -1)
    {
      food.spawn(boundary);
    }

    // Clear the previous position of the snake's head
    if (prevRow != -1 && prevCol != -1)
    {
      t.setChar(prevRow, prevCol, ' ');
    }

    // Draw the food
    t.setChar(food.row, food.col, FOOD_CHAR, false, false, false, false, CURSOR_FG_COLOR, FOOD_COLOR);

    // Draw the snake
    drawSnake(snake, t);

    // Get the new position of the snake's head
    ipair headPos = snake.getBody().front();
    prevRow = headPos.first;
    prevCol = headPos.second;

    // Check if the snake has eaten the food
    if (headPos.first == food.row && headPos.second == food.col)
    {
      // Snake eats food, grow the snake
      // Add a new segment to the snake's body at the position of the food
      snake.grow();
      // Respawn food
      food.spawn(boundary);
    }

    // Check for collisions after the snake has moved
    if (checkSelfCollision(snake) || checkBoundaryCollision(snake, screen_size))
    {
      // Handle collision (e.g., end the game)
      break;
    }

    // Sleep to control the speed of the game
    usleep(GAME_SPEED);
  }
}


//menu functions
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

void settingsMenu(Terminal &t)
{
  bool force_menu = true;
  bool active = true;
  vector<string> menu_text = {"NAVIGATE MENU WITH W&S", "PRESS ENTER TO SELECT, C TO RETURN TO MAIN MENU"};
  vector<string> menu_options = {"MENU_OPTION_BG_COLOR", "MENU_TEXT_BG_COLOR", "CURSOR_CHAR", "CURSOR_BG_COLOR",
  "CURSOR_FG_COLOR", "SNAKE_HEAD_BG", "SNAKE_BODY_BG", "FOOD_COLOR", "FOOD_CHAR", "SNAKE_HEAD_UP", "SNAKE_HEAD_DOWN", 
  "SNAKE_HEAD_LEFT", "SNAKE_HEAD_RIGHT", "DISABLE_SELF_COLLISION", "GAME_SPEED"};
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
            intInputMenu("ENTER ANSI COLOR CODE", t, MENU_OPTION_BG_COLOR);
            break;
          case 2:
            intInputMenu("ENTER ANSI COLOR CODE", t, MENU_TEXT_BG_COLOR);
            break;
          case 3:
            charInputMenu("ENTER A CHARACTER", t, CURSOR_CHAR);
            break;
          case 4:
            intInputMenu("ENTER ANSI COLOR CODE", t, CURSOR_BG_COLOR);
            break;
          case 5:
            intInputMenu("ENTER ANSI COLOR CODE", t, CURSOR_FG_COLOR);
            break;
          case 6:
            intInputMenu("ENTER ANSI COLOR CODE", t, SNAKE_HEAD_BG);
            break;
          case 7:
            intInputMenu("ENTER ANSI COLOR CODE", t, SNAKE_BODY_BG);
            break;
          case 8:
            intInputMenu("ENTER ANSI COLOR CODE", t, FOOD_COLOR);
            break;
          case 9:
            charInputMenu("ENTER FOOD CHARACTER", t, FOOD_CHAR);
            break;
          case 10:
            charInputMenu("ENTER SNAKE HEAD UP", t, SNAKE_HEAD_UP);
            break;
          case 11:
            charInputMenu("ENTER SNAKE HEAD DOWN", t, SNAKE_HEAD_DOWN);
            break;
          case 12:
            charInputMenu("ENTER SNAKE HEAD LEFT", t, SNAKE_HEAD_LEFT);
            break;
          case 13:
            charInputMenu("ENTER SNAKE HEAD RIGHT", t, SNAKE_HEAD_RIGHT);
            break;
          case 14:
            boolInputMenu("ENABLE/DISABLE SELF-COLLISION", t, SELF_COLLISION);
            break;
          case 15:
            intInputMenu("FRAME DELAY IN NANOSECONDS", t, GAME_SPEED);
        }
    }
  }
}