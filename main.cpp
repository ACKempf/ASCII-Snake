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

/*The menu class allows the programmer to define any number of header lines, and any number of option lines, 
and allows the user to manipulate a cursor to retrieve a selection from the option lines.*/
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