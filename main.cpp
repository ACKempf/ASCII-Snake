#include <unistd.h>
#include "TControl.hpp"
#include "SnakeUtils.hpp"

using namespace std;

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

  ipair boundary = setBoundary(screen_size);
  int startX = rand() % (boundary.first - 8) + 5;
  int startY = rand() % (boundary.second - 8) + 5;
  char startDirection = 'd';

  Snake snake(startX, startY, startDirection);

  switch (user_decision)
  {
  case 1:
    t.clearGrid();
    playGame(snake, t, screen_size);
    break;
  case 2:
    settingsEditorMenu(t);
    break;
  case 3:
    exit(0);
    break;
  }
  }
}
