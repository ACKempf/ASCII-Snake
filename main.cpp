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

/*The menu class allows the user to define any number of header lines, and any number of option lines, and allows the user to manipulate a cursor to retrieve
a selection from the option lines.*/
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
    int, offset: 
    */
    void moveCursor(int offset)
    {
      int to_position = cursor_position + offset;
      if (to_position>0 && to_position<=max_cursor_position) cursor_position = to_position;
      return;
    };

    //Public method for updating terminal screen
    void updateTerminal() 
    {
      pushToTerminal();
      return;
    }

    //Getter for the current selection, this is how the users selection is actually handled
    int getSelection() {return cursor_position;}


  private:
    //Terminal reference for modifying terminal
    Terminal& t;
    //Constants of the Menu class
    const vector<string> menu_options;
    const vector<string> menu_text;
    const ipair center;
    const int cursor_column_offset = 2;
    //The amount of new lines between the menu text messages and the selectable options
    const int text_option_seperation = 1;
    //The starting position for printing the first character, and the row offset where the 
    const int left_justification_column;
    //Cursor members
    int cursor_position = 1;
    const int max_cursor_position;

    //Pushed the data to the center of the terminal (DOES NOT DRAW)
    void pushToTerminal(){
      t.clearGrid();
      //The row to which the first text line is printed
      //Vertically centered
      int current_row = center.first-((menu_options.size()+menu_text.size())/2)-text_option_seperation;
      for (string s:menu_text) {
        int current_column_offset=0;
        for (char c:s) {
          //Set current character with MENU_TEXT constant style
          t.setChar(current_row, left_justification_column+current_column_offset, c, MENU_TEXT_BOLD, MENU_TEXT_ITALIC, MENU_TEXT_UNDERLINE, MENU_TEXT_BLINK, MENU_TEXT_FG_COLOR, MENU_TEXT_BG_COLOR);
          //Advance the column to the left
          current_column_offset++;
        }
        //move down one row between given text lines
        current_row++;
      }
      //move down certain amount of rows
      current_row += text_option_seperation;

      //used to track cursor position
      int selection_cursor_index = 1;

      for (string s:menu_options) {
        int current_column_offset=0;
        for (char c:s) {
          //Set current character with MENU_OPTION constant style
          t.setChar(current_row, left_justification_column+current_column_offset, c, MENU_OPTION_BOLD, MENU_OPTION_ITALIC, MENU_OPTION_UNDERLINE, MENU_OPTION_BLINK, MENU_OPTION_FG_COLOR, MENU_OPTION_BG_COLOR);
          //Advance the column to the left
          current_column_offset++;
        }

        if (selection_cursor_index == cursor_position) t.setChar(current_row, left_justification_column-cursor_column_offset, CURSOR_CHAR, CURSOR_BOLD, CURSOR_ITALIC, CURSOR_UNDERLINE, CURSOR_BLINK, CURSOR_FG_COLOR, CURSOR_BG_COLOR);

        current_row++;
        selection_cursor_index++;
      }
      return;
    }

    //Just gets the longest string length between menu_text and menu_options
    int getLongestStringLength()
    {
      //string length
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

  //Menu loop, after running it returns the selection as the number presented on the menu

    vector<string> menu_options = {"PLAY", "SETTINGS", "EXIT"};
    vector<string> menu_text = {"USE (UP KEY PLACE HOLDER) W & (DOWN KEY PLACE HOLDER) S TO NAVIGATE MENU", "USE ENTER TO CONFIRM SELECTION"};
    Menu m(menu_text, menu_options, t);

    //Signals the menu to run once regardless of whether there is input available
    bool init_run = true;
    while (true) {
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
      }
    }


}