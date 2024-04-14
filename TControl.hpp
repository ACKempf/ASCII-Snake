/*
* File: TControl.hpp
* Date: 03/28/2024
*
* Description:
* Header file contains any terminal related functionality that would be OS
* specific. The file contains conditional compilation blocks for both
* windows and apple-based systems. This allows for later adaptation to windows
* without having to rewrite any existing code in the main program.
*/

//Redundancy safety check
#ifndef TCONTROL_H
#define TCONTROL_H

//Below is a link to pre-defined OS specific macros for conditional compilation
//https://sourceforge.net/p/predef/wiki/OperatingSystems/

//_WIN32 is defined for both 32 and 64 bit
#ifdef _WIN32

  //WINDOWS CODE HERE

//Defined by G++ and clang when compiling on an apple system
#elif __APPLE__
  #include <iostream>
  #include <vector>
  #include <string>
  #include <stdexcept>
  //Used for changing console input/output mode (enableRawMode function)
  #include <termios.h>
  //Used for retrieving the size of the console window
  #include <sys/ioctl.h>
  //Used for non-blocking input
  #include <poll.h>

  //A shorthand constant for the ANSI escape code on terminal
  const std::string ESC = "\033[";

  /*
  Returns a pair type that contains the width and height of the console window currently (respectively)

  Params: None

  Returns: A pair (width, height)
  */
  std::pair<int, int> getTermSize()
  {
    struct winsize sizes;
    //ioctl takes STDOUT_FILENO, TIOCGWINSZ as an argument tells it to collect window sizes, window sizes are saved to the winsize struct
    ioctl(STDOUT_FILENO, TIOCGWINSZ, &sizes);

    return std::pair<int, int>(sizes.ws_row, sizes.ws_col);
  }

  /*
  Returns the last (unread) character pressed, and the null character if no character has been pressed

  Params: None

  Returns: A character, the last (unread) character pressed
  */
  char getInput()
  {
    //Structure that contains the file descriptors to monitor for available input
    struct pollfd fds[1];

    //Tell it which file descriptor, and what to monitor, POLLIN means to look for available input
    fds[0].fd = STDIN_FILENO;
    fds[0].events = POLLIN;

    //Initialize the last pressed character with the null character
    unsigned char last_chr = 0;

    //As long as there is available input, exhaustively collect input until there is no new input to be read
    while (poll(fds, 1, 0)) last_chr = std::cin.get();

    //Return the last character read before none was available
    return last_chr;
  }

  /*
  A duplicate of the clear method for the Terminal class, Allows clearing the screen outside of a class instance

  Params: None

  Returns: Void
  */
  void clear() 
      {
        //Print the ANSI codes to clear screen and home the cursor
        //NOTE:This doesnt erase any characters, it only newlines the exact amount needed to hide the previous state of terminal
        std::cout << (ESC + "2J" + ESC + "H");
        std::cout.flush();
        return;
      }

  /*
  Enables raw mode in terminal, disabling echo and line-by-line reading mode (canonical)
  also enables non_blocking input

  Params: None

  Returns: Void
  */
  void enableRawMode()
  {
    struct termios raw_mode;
    //Get attribute sturct from current terminal and store in raw_mode struct
    tcgetattr(STDIN_FILENO, &raw_mode);

    //Disable the canonical mode and echo mode flags
    //Using bitwise operators, bitwise & operator is true if flag&specified bit is true.
    //By setting ~ to ECHO, it flips the bit if ECHO is on, thus disabling it
    //Same applied to canonical mode
    raw_mode.c_lflag &= ~(ECHO | ICANON);

    //Apply changes made to struct immediately
    tcsetattr(STDIN_FILENO, TCSANOW, &raw_mode);

    return;
  }

  /*
  The terminal class represents a 2D grid of characters, Each character in the grid is able to be formatted (color, bold, underlined, etc...)
  and can be positioned with standard 2D coordinates. This allows a program to carefully control a rectangular space in console of a given size. 
  The size of this area is constant after initialization.
  */
  class Terminal
  {
    /*
    Constructor for the class, as of now only initializes constants and display grid with given row and column size.

    Params: 2 integer
    int, r: The amount of rows of the display area
    int, c: The amount of columns in the display area
    */
    public: Terminal(int r, int c) : rows(r), columns(c), char_grid(r, std::vector<std::string>(c, " ")){};

    private:

      /*
      Sets an element at a certain row and column of the display table to a given string parameter.

      Params: 2 integer, 1 string
      int, row: The row to place the character (0-indexed)
      int, column: The column to place the character (0-indexed)
      string, to_set: The value the specified element will be set to

      Returns: void
      */
      void setElement(int row, int column, std::string to_set)
      {
         char_grid[row][column] = to_set;
         return;
      }

      /*
      Clears the screen of all contents by moving the terminal display down x rows with newline characters

      Params: None

      Returns: Void
      */
      void clear() 
      {
        //Print the ANSI codes to clear screen and home the cursor
        //NOTE:This doesnt erase any characters, it only newlines the exact amount needed to hide the previous state of terminal
        std::cout << (ESC + "H");
        std::cout.flush();
        return;
      }

      /*
      Takes a given character and applied a format to it using ANSI escape sequences

      Params: 1 char, 4 bool, 2 int
      char, chr: The character to be formatted
      bool, bold: Emboldens the character
      bool, italic: Italicizes the character
      bool, underline: Underlines the character
      bool, blink: Makes the character blink (cursor blink behavior)
      int, fg_color: Sets the color of the character itself (0-255 color)
      int, bg_color: Sets the color of the background of the cell containing the character (0-255 color)

      Returns: A string containing all relevant ANSI codes and the character to be printed
      */
      std::string generateFormatChar(char chr, bool bold=false, bool italic=false, bool underline=false, bool blink=false, int fg_color=231, int bg_color=232)
      {
        //string used to store all ANSI codes and character to return
        std::string formatted = "";

        //If statements check if format options are true then appends correct ANSI code
        if (bold) formatted+=(ESC+"1m");
        if (italic) formatted+=(ESC+"3m");
        if (underline) formatted+=(ESC+"4m");
        if (blink) formatted+=(ESC+"5m");

        //Color data has proper default values so is appended regardless of its values
        //ESC38;5;{ID}m following this ANSI code
        formatted+=(ESC+"38;5;" + std::to_string(fg_color) + "m");
        //ESC48;5;{ID}m
        formatted+=(ESC+"48;5;" + std::to_string(bg_color) + "m");

        //Append the character that is being formatted
        formatted+=chr;

        //"0m" resets all attributes, color included
        //This prevents characters in the array from inheriting formats from the previous
        formatted+=(ESC+"0m");

        return formatted;
      }

      //Constants for the number of rows and columns
      const int rows;
      const int columns;

      //Boolean storing visibility status of cursor
      bool cursor_visibility = true;

      

      //Display vector
      //Indexed as [row][column] OR [X][Y] (0-indexed)
      std::vector<std::vector<std::string>> char_grid;

    public:

      /*
      Get respective private values (in function name)

      Params: None

      Returns: Integer of respective value
      */
      int getRows() const {return rows;}
      int getColumns() const {return columns;}

      /*
      Prints all characters stored in the display table to console

      Params: None

      Returns: Void
      */
      void draw() 
      {
        //To help prevent flickering the contents of the table are appended to a single continuous string
        std::string pre_print = "";

        //range-based for-loops iterate through horizontal rows first and vertical columns second.
        //ch is the object found at the current position being iterated through on the table
        for (std::vector<std::string> column:char_grid) {
          for (std::string ch:column) {
            //Append the character to the pre_print string
            pre_print += ch;
          }
          //Append newline to the pre_print string to seperate rows
          pre_print += "\n";
        }

        //Clear the terminal and print the pre_print string.
        clear();
        std::cout << pre_print;
        std::cout.flush();
        return;
      };

      /*
      Changes the visibility status of the cursor (blinking vs not appearing at all)

      Params: 1 bool
      bool, to_set: True to show the cursor, False to display the cursor

      Returns: Void
      */
      void setCursorVisibility(bool to_set) 
      {
        //Checks if cursor_visibility is different from what the user requests
        if (to_set != cursor_visibility){
          //Checks if cursor visibility is true, changes cursor visiblity and sets cursor_visibility to false
          if (cursor_visibility) {
            //ANSI escape to hide the cursor
            std::cout << ESC+"?25l";
            cursor_visibility = false;
            return;
          } else {
            //ANSI escape to show the cursor
            std::cout << ESC+"?25h";
            cursor_visibility = true;
            return;
          }
        }
        return;
      }

      /*
      Sets a specific character at a certain row and column of the display table to the given character, with format options.

      Params:1 char, 4 int, 4 bool
      int, row: The row to place the character
      int, column: The column to place the character
      char, chr: The character to be printed to console with supplied format options
      bool, bold: Emboldens the character
      bool, italic: Italicizes the character
      bool, underline: Underlines the character
      bool, blink: Makes the character blink (cursor blink behavior)
      int, fg_color: Sets the color of the character itself (0-255 color)
      int, bg_color: Sets the color of the background of the cell containing the character (0-255 color)

      Returns: Void
      */
      void setChar(int row, int column, char chr, bool bold=false, bool italic=false, bool underline=false, bool blink=false, int fg_color=231, int bg_color=232)
      {
        //Some error handling code to make it easier to diagnose later
        if (0 <= row && row < rows && 0 <= column && column < columns){
          //Fairly simple functionality, Just uses the correct private method to perform the task
          setElement(row, column, generateFormatChar(chr, bold, italic, underline, blink, fg_color, bg_color));
        } else {
          std::cerr << "Attempted to set character @ (" << row << ", " << column << ")" << std::endl;
          throw std::out_of_range("Attempted to set value outside of display grid.");
        }
        return;
      }

      /*
      Returns the center of the display as a pair
      Will be innacurate if the number of rows and columns is odd and there is no true center

      Params: None

      Returns: A pair containing the row and columb of the center
      */
      std::pair<int, int> findCenter(){
        std::pair<int, int> to_return;
        to_return.first = rows/2;
        to_return.second = columns/2;

        return to_return;
      }


      /*
      Clears the display grid to be blank

      Params: None

      Returns: Void
      */
      void clearGrid()
      {
        //Iterate through all character slots and set them to whitespace
        for (std::vector<std::string>& r:char_grid) {
          for (std::string &ch:r) {
            ch = " ";
          }
        }

        return;
      }
  };

#endif

#endif