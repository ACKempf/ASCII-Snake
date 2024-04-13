#include <iostream>
#include <vector>
#include <unistd.h>
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


}