#include <iostream>
#include <vector>
#include <unistd.h>
#include <stdlib.h>
#include "TControl.hpp"
#include "GameCode.hpp"

using namespace std;
using ipair = pair<int, int>; //Type alias for integer pairs
using pvector = vector<pair<int, int>>; //Type alias for vectors containing integer pairs

int main()
{
  //Enable raw mode & clear
  enableRawMode();
  clear();

  //Input loop for confirming what display size the user wants
  //~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~//
  ipair screen_size;
  while(true) {
    clear();
    static char input;

    //Simple printout instructing user
    cout << "Adjust window to desired size and press any key to continue (Minimum 16x75)" << endl;
    //A "press any key to continue" break
    cin.ignore();

    //Reassign screen_size to current screen size
    screen_size = getTermSize();
    while(screen_size.first < 16 || screen_size.second < 75){
      clear();
      cout << "Please readjust your screen size to be at least 16x75 (currently is: " << screen_size.first << "x" << screen_size.second << ")" << endl;
      cout << "Adjust window to desired size and press any key to continue (Minimum 16x75)" << endl;
      //A "press any key to continue" break
      cin.ignore();
      screen_size = getTermSize();
    }
    /*
    while(screen_size.first < 24 || screen_size.second < 80){
      cout << "Please make sure your screen dimensions are at least 24x80. (Current screen size: " << screen_size.first << "x" << screen_size.second << ")" << endl;
      cout << "Adjust window to desired size and press any key to continue (Minumum 24x80)" << endl;
      cin.ignore();
      screen_size = getTermSize();
    }
    */

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
  //End of window size confirmation loop
  //~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~//

  Terminal t(screen_size.first, screen_size.second); //Initalize a terminal instance
  t.setCursorVisibility(false); //Disable cursor visibility

  vector<string> menu_text = {"", "NAVIGATE UP & DOWN WITH 'w' & 's'", "PRESS ENTER TO SELECT AN OPTION"}; //Main menu header
  vector<string> menu_options = {"PLAY", "SETTINGS", "EXIT"}; //Main menu options

  while(true){
    if (HIGHEST_SCORE>0) menu_text[0]= ("HIGHEST SCORE: "+to_string(HIGHEST_SCORE)); //Show highscore banner if there is a highscore
    Menu m(menu_text, menu_options, t); //Initialize main menu

    bool force_run = true; //forces the menu to render once
    int user_decision; //Holds the chosen user decision from the menu
    bool menu_active = true; //Exit trigger for main menu loop

    //Main menu loop
    while (menu_active) {
      //Force run loop for initial render
      if (force_run) {
        m.updateTerminal();
        t.draw();
        force_run=false;
      }
  
    //Cursor navigation
    switch (getInput()) {
      case 'w': //cursor up
        m.moveCursor(-1);
        m.updateTerminal();
        t.draw();
        break;
      case 's': //cursor down
        m.moveCursor(1);
        m.updateTerminal();
        t.draw();
        break;
      case '\n': //chosen option
        user_decision = m.getSelection(); //Assigned with users chosen option here
        menu_active = false; //Exits the main menu loop
        break;
      }
    }

    //Snake initial setup 
    ipair boundary = setBoundary(screen_size);
    int startX = rand() % (boundary.first - 8) + 5;
    int startY = rand() % (boundary.second - 8) + 5;
    char startDirection = 'd';
    Snake snake(t, boundary, startX, startY, startDirection);
    ScoreBoard sb(t, HIGHEST_SCORE);

    //Menu user_decision outcome switch
    switch (user_decision)
    {
    case 1: //Start game loop
      t.clearGrid();
      playGame(snake, t, screen_size, sb);
      break;
    case 2: //Open settings menu
      settingsEditorMenu(t);
      break;
    case 3: //Exit the game
      exit(0);
      break;
    }
  }
}