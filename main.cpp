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
int GAME_SPEED = 70000;

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
  Snake(int initX, int initY, char initDirection)
  {
    // Initialize the snake with a single segment at the starting position
    body.push_back({initX, initY});
    direction = initDirection;
    prevTailPos = {initX, initY};
  }

  // Function to move the snake in the current direction
  void move()
  {
    // Calculate the offset for the new head position based on the current direction
    int xOffset = 0, yOffset = 0;
    switch (direction)
    {
    case 'a': // Left
      xOffset = 0;
      yOffset = -1;
      break;
    case 'd': // Right
      xOffset = 0;
      yOffset = 1;
      break;
    case 'w': // Up
      xOffset = -1;
      yOffset = 0;
      break;
    case 's': // Down
      xOffset = 1;
      yOffset = 0;
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
    head.first += xOffset;
    head.second += yOffset;
  }

  // Function to change the direction of the snake
  void changeDirection(char newDirection)
  {
    // Update the direction
    direction = newDirection;
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
    int newX = tailSegment.first;
    int newY = tailSegment.second;

    // Add the new segment in a position adjacent to the tail segment, depending on the direction
    switch (direction)
    {
    case 'a':    // Left
      newY += 1; // Move the new segment one column to the left
      break;
    case 'd':    // Right
      newY -= 1; // Move the new segment one column to the right
      break;
    case 'w':    // Up
      newX += 1; // Move the new segment one row up
      break;
    case 's':    // Down
      newX -= 1; // Move the new segment one row down
      break;
    }

    // Add the new segment to the end of the body vector, extending the tail
    body.push_back({newX, newY});
  }
};

int setBoundary(ipair screen_size)
{
  int height = screen_size.first;
  int width = screen_size.second;
  int boundary;
  if (height > width)
    boundary = width - 1;
  else
    boundary = height - 1;
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
  vector<string> menu_text = {"USE (UP KEY PLACE HOLDER) W & (DOWN KEY PLACE HOLDER) S TO NAVIGATE MENU", "USE ENTER TO CONFIRM SELECTION"};
  Menu m(menu_text, menu_options, t);

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
    case 's':
      m.moveCursor(-1);
      m.updateTerminal();
      t.draw();
      break;
    case 'a':
      m.moveCursor(1);
      m.updateTerminal();
      t.draw();
      break;
    case 'e':
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
  case 3:
    t.clearGrid();
    playGame(snake, t, screen_size);
    break;
  case 2:
    cout << "Options";
    break;
  case 1:
    exit(0);
    break;
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
  char bodyChar = '*'; // Character for snake's body
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
  default:
    headChar = SNAKE_HEAD_RIGHT;
    break;
  }

  // Draw the snake's head
  const ipair &headPos = body.front();
  t.setChar(headPos.first, headPos.second, headChar, false, false, false, false, SNAKE_HEAD_COLOR, SNAKE_HEAD_BG);

  // Draw the snake's body segments (if any)
  if (body.size() > 1)
  {
    for (size_t i = 1; i < body.size(); ++i)
    {
      const ipair &segment = body[i];
      t.setChar(segment.first, segment.second, bodyChar, false, false, false, false, SNAKE_BODY_COLOR, SNAKE_BODY_BG);
    }
  }

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

  // Iterate over the body segments starting from the second segment
  for (auto i = body.begin() + 1; i != body.end(); i++)
  {
    // If the coordinates of the current segment match the head's coordinates, return true
    if (*i == head)
    {
      return true;
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