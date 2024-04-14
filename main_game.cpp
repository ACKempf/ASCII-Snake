#include <iostream>
#include <vector>
#include <unistd.h>
#include <stdlib.h>
#include <ctime>
#include <exception>
#include "TControl.hpp"

using namespace std;
//Some type aliases for readability
using ipair = pair<int, int>;
using pvector = vector<pair<int, int>>;

bool MENU_BOLD = false;
bool MENU_ITALIC = false;
bool MENU_UNDERLINE = false;
bool MENU_BLINK = false;
int MENU_FG_COLOR = 231;
int MENU_BG_COLOR = 232;
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
class Snake {
private:
    pvector body; // Vector to store the coordinates of the snake's body segments
    char direction; // Current direction of the snake ('a' for left, 'd' for right, 'w' for up, 's' for down)
    ipair prevTailPos; // Previous position of the tail segment

public:
    // Constructor to initialize the snake with a starting position and direction
    Snake(int initX, int initY, char initDirection) {
        // Initialize the snake with a single segment at the starting position
        body.push_back({initX, initY});
        direction = initDirection;
        prevTailPos = {initX, initY};
    }

    // Function to move the snake in the current direction
    void move() {
    // Calculate the offset for the new head position based on the current direction
    int xOffset = 0, yOffset = 0;
    switch (direction) {
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
    for (size_t i = body.size() - 1; i > 0; --i) {
        body[i] = body[i - 1];
    }

    // Calculate the new head position
    ipair& head = body.front();
    head.first += xOffset;
    head.second += yOffset;
}


    // Function to change the direction of the snake
    void changeDirection(char newDirection) {
        // Update the direction
        direction = newDirection;
    }

    // Getter function to retrieve the snake's body
    const pvector& getBody() const {
        return body;
    }

    // Getter function to retrieve the previous position of the tail segment
    ipair getPrevTailPos() const {
        return prevTailPos;
    }

    // Getter function to retrieve the current direction of the snake
    char getDirection() const {
        return direction;
    }

    // Function to make the snake grow by adding a new segment
    void grow() {
        // Get the last segment of the snake's body
        const ipair& tailSegment = body.back();

        // Calculate the position for the new segment
        int newX = tailSegment.first;
        int newY = tailSegment.second;

        // Add the new segment in a position adjacent to the tail segment, depending on the direction
        switch (direction) {
            case 'a': // Left
                newY += 1; // Move the new segment one column to the left
                break;
            case 'd': // Right
                newY -= 1; // Move the new segment one column to the right
                break;
            case 'w': // Up
                newX += 1; // Move the new segment one row up
                break;
            case 's': // Down
                newX -= 1; // Move the new segment one row down
                break;
        }

        // Add the new segment to the end of the body vector, extending the tail
        body.push_back({newX, newY});
    }
};



int setBoundary(ipair screen_size){
    int height = screen_size.first;
    int width = screen_size.second;
    int boundary;
    if(height > width) boundary = width - 1;
    else boundary = height - 1;
return boundary;
}

void playGame(Snake &snake, Terminal &t, ipair screensize);
void drawSnake();
void createGrid(ipair screensize);
bool checkBoundaryCollision(const Snake& snake, ipair screensize);
bool checkSelfCollision(const Snake& snake);

struct Food {
    int row=-1;
    int col=-1;
    // Method to spawn food at a random location on the grid
    void spawn(int boundary) {
        // Generate random row and column within the grid boundaries
        row = rand() % (boundary - 1) + 1;
        col = rand() % ((boundary * 2) - 1) + 1;
    }

    // Method to check the food collision with the snake
    bool checkCollision(const Snake& snake) const {
        // Iterate over the snake's body segments and check if any segment matches the food's position
        for (const ipair& segment : snake.getBody()) {
            if (row == segment.first && col == segment.second) {
                return true; //Collision detected
            }
        }
        return false; //No collision
    }
};

int main()
{
  //Enable raw mode
  enableRawMode();
  clear();

  //Input loop for confirming what display size the user wants
  //Also for taking dev commands
  ipair screen_size;
  while(true) {
  static char input;

  cout << "Adjust window to desired size and press any key to continue." << endl;
  cin.ignore();

  //Store screen size in a integer pair
  //Trim down number of rows to avoid top row being invisible but usable
  screen_size = getTermSize();
  screen_size.first--;

  //Get closest odd screen sizes
  if (screen_size.first%2 == 0) screen_size.first--;
  if (screen_size.second%2 == 0) screen_size.second--;

  cout << "Enter y/Y to confirm display size: " << screen_size.first << "x" << screen_size.second << endl;
  cin >> input;
  if (input=='y'||input=='Y') break;
    clear();
  }

  //Create Terminal object of the correct size
  Terminal t(screen_size.first, screen_size.second);
  t.setCursorVisibility(false);

  vector<string> menu_text = {"EXIT", "OPTIONS", "PLAY"};

  int biggest_str_len = 0;
  for (string s:menu_text) {
    if (s.size() > biggest_str_len) biggest_str_len = s.size();
  }

  //Where menu item begins printing and where the cursor should be placed
  int menu_left_justification_column = t.findCenter().second - biggest_str_len;
  int cursor_column = menu_left_justification_column-2;
  int number_home_position;

  //Menu loop, after running it returns the selection as the number presented on the menu
  char input = 49;
  int selected;
  while (true) {

    if (input==selected+48) break;

    //Initialize and print menu (selected should only be -1 on initialization)
    if (input) {
      selected = input - 48;
      //Column offset for spacing menu items to be centered vertically
      int row_offset = menu_text.size()/2;
      ipair center = t.findCenter();
      int menu_option_number = 1;

      for (string s:menu_text) {
        int column_offset = 0;
        for (char c:s){
            t.setChar(center.first+row_offset, menu_left_justification_column+column_offset, c, MENU_BOLD, MENU_ITALIC, MENU_UNDERLINE, MENU_BLINK, MENU_FG_COLOR, MENU_BG_COLOR);
            if (menu_option_number == selected) t.setChar(center.first+row_offset, cursor_column, menu_option_number+48, CURSOR_BOLD, CURSOR_ITALIC, CURSOR_UNDERLINE, CURSOR_BLINK, CURSOR_FG_COLOR, CURSOR_BG_COLOR);
            else t.setChar(center.first+row_offset, cursor_column, menu_option_number+48, MENU_BOLD, MENU_ITALIC, MENU_UNDERLINE, MENU_BLINK, MENU_FG_COLOR, MENU_BG_COLOR);
            
            column_offset++;
        }
        menu_option_number++;
        row_offset--;
      }

      t.draw();
    }
    input = getInput();

  }

  int boundary = setBoundary(screen_size);
  int startX = rand() % (boundary - 2) + 1;
  int startY = rand() % (boundary - 2) + 1;
  char startDirection = 'd';

  Snake snake(startX, startY, startDirection);
  switch(selected) {
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
    
    for (int i = 0; i <= boundary; i++) {
        t.setChar(i, 0, '#');
        t.setChar(i, boundary * 2, '#');     
    }
    for (int j = 0; j <= boundary * 2; j++) {
        t.setChar(0, j, '#');                
        t.setChar(boundary, j, '#');         
    }
    
    for (int i = 1; i < boundary; i++) {
        for (int j = 1; j < boundary * 2; j++) {
            t.setChar(i, j, ' '); 
        }
    }
    
    t.draw();
}
void drawSnake(const Snake& snake, Terminal& t) {
    // Get the body of the snake
    const pvector& body = snake.getBody();
    // Define the characters to represent the snake's body
    char bodyChar = '*'; // Character for snake's body
    char headChar;
    switch (snake.getDirection()) {
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
    const ipair& headPos = body.front();
    t.setChar(headPos.first, headPos.second, headChar, false, false, false, false, SNAKE_HEAD_COLOR, SNAKE_HEAD_BG);

    // Draw the snake's body segments (if any)
    if (body.size() > 1) {
        for (size_t i = 1; i < body.size(); ++i) {
            const ipair& segment = body[i];
            t.setChar(segment.first, segment.second, bodyChar, false, false, false, false, SNAKE_BODY_COLOR, SNAKE_BODY_BG);
        }
    }

    // Erase the previous position of the tail segment
    ipair prevTailPos = snake.getPrevTailPos();
    t.setChar(prevTailPos.first, prevTailPos.second, ' ');

    // Draw the updated grid with the snake
    t.draw();
}

bool checkSelfCollision(const Snake& snake) {
    // Get the body of the snake
    const pvector& body = snake.getBody();

    // Get the coordinates of the head
    ipair head = body.front();

    // Iterate over the body segments starting from the second segment
    for (auto i = body.begin() + 1; i != body.end(); i++) {
        // If the coordinates of the current segment match the head's coordinates, return true
        if (*i == head) {
            return true;
        }
    }

    // No collision detected
    return false;
}

bool checkBoundaryCollision(const Snake& snake, ipair screen_size) {
  // Get the coordinates of the head
  ipair head = snake.getBody().front();
  int boundary = setBoundary(screen_size);
    // Check if the head's coordinates exceed the boundaries
    if (head.first == 0 || head.first == boundary || head.second == 0 || head.second == boundary*2) {
        return true; // Collision detected
    }
    // No collision detected
    return false;
}
void playGame(Snake &snake, Terminal &t, ipair screen_size) {
    // Initialize random seed
    srand(time(NULL));
    Food food;
    int boundary = setBoundary(screen_size);
    int prevRow = -1, prevCol = -1; // Initialize previous position
    createGrid(screen_size, t);
    while (true) {
        // Process user input
        char input = getInput();
        if (input == 'w' || input == 'a' || input == 's' || input == 'd') {
            // Change snake direction based on user input
            snake.changeDirection(input);
        }

        // Move the snake
        snake.move();

        // Check if the food needs to be respawned
        if (food.row == -1) {
            food.spawn(boundary);
        }

        // Clear the previous position of the snake's head
        if (prevRow != -1 && prevCol != -1) {
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
        if (headPos.first == food.row && headPos.second == food.col) {
            // Snake eats food, grow the snake
            // Add a new segment to the snake's body at the position of the food
            snake.grow();
            // Respawn food
            food.spawn(boundary);
        }

        // Check for collisions after the snake has moved
        if (checkSelfCollision(snake) || checkBoundaryCollision(snake, screen_size)) {
            // Handle collision (e.g., end the game)
            break;
        }

        // Sleep to control the speed of the game
        usleep(GAME_SPEED);
    }
}