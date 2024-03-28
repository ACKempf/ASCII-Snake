/*
* File: TControl.hpp
* Date: 03/26/2024
*
* Description:
* Header file contains any terminal related functionality that would be OS
* specific. The file contains conditional compilation blocks for both
* windows and apple-based systems. This allows for later adaptation to windows
* without having to rewrite any existing code in the main program.
*/

//Redundancy safety check, only compiles if TCONTROL_H isnt defined, TCONTROL_H only defined when if check passed
#ifndef TCONTROL_H
#define TCONTROL_H

//#ifdef will only compile what is within it if the object is defined
//Below is a link to pre-defined OS specific macros
//https://sourceforge.net/p/predef/wiki/OperatingSystems/
//With this we can deduce what operating system is compiling the file

//_WIN32 is defined for both 32 and 64 bit
#ifdef _WIN32

  //WINDOWS CODE HERE

//Defined by G++ and clang when compiling on an apple system
#elif __APPLE__
  #include <iostream>
  #include <tuple>
  #include <vector>
  #include <string>

  //A shorthand constant for the ANSII escape code
  const std::string ESC = "\033[";

  class Terminal
  {
    //Class Constructor
    //Inputs the size of the terminal as r (rows) & c (columns), initializes the rows and columns constants
    //Initializes the vector and populated it with ' ' characters
    public: Terminal(int r, int c) : rows(r), columns(c), char_grid(r, std::vector<std::string>(c, " ")){};

    private:

      //TODO: Test functionality through setChar() method
      //Sets the element at a given row to the given string
      //Inputs 2 integers and 1 string
      //Integers representing the row and column to be set
      //String representing the data to be placed at that place
      void setElement(int row, int column, std::string to_set)
      {
         char_grid[row][column] = to_set;
         return;
      }

      //TODO: Test functionality through setChar() method
      //Clears the console screen of any printed characters
      //Outputs the ANSI clear code to console and the code to return the cursor to the home position
      void clear() {std::cout << (ESC + "2J" + ESC + "H");}

      //TODO: Test functionality, private function can only be accessed through setChar() method
      //Generated a string that when printed to console, presents one character with desired formatting options
      //Inputs 1 char, 4 boolean values, 2 integer 
      //Char is the char to be printed.
      //Bools are various formatting options (bold, italic, underline, blinking)
      //Integers, foreground color and background color respectively. Refer to 8-bit color table, populated with default black and white colors
      //TODO: Because of how i plan on implementing the reset between characters, it would be best to implement color directly in here rather than a seperate function
      std::string generateFormatChar(char chr, bool bold=false, bool italic=false, bool underline=false, bool blink=false, int fg_color=231, int bg_color=232)
      {
        std::string formatted = "";

        //If statement used to append format data to characters
        if (bold) formatted+=(ESC+"1m");
        if (italic) formatted+=(ESC+"3m");
        if (underline) formatted+=(ESC+"4m");
        if (blink) formatted+=(ESC+"5m");

        //Color data has proper default values so is implemented regardless of its values
        //ESC38;5;{ID}m following this ANSI code
        formatted+=(ESC+"38;5;" + std::to_string(fg_color) + "m");
        //ESC48;5;{ID}m
        formatted+=(ESC+"48:5:" + std::to_string(bg_color) + "m");

        //"0m" resets all attributes, color included
        formatted+=(ESC+"0m");

        return formatted;
      }

      //Constants for the number of rows and columns
      const int rows;
      const int columns;

      //Boolean storing visibility status of cursor
      bool cursor_visibility = true;

      //Display vector
      //Indexed as [row][column] OR [X][Y]
      std::vector<std::vector<std::string>> char_grid;

    public:

      //Getters for row and column constants
      int getRows() const {return rows;}
      int getColumns() const {return columns;}

      //TODO: Test screen clear functionality in draw method
      //Draws the contents of the display array to the screen
      void draw() 
      {
        //Clear the console beforehand
        clear();
        //Nested range-based for loops, outer to iterate rows, inner to iterate individual characters in rows
        //NOTE: ch is not a character type, but the way "characters" are added to the array, other parts of the string are ANSI escape sequences
        //So the element should always print to console exactly 1 character that is formatted
        for (std::vector<std::string> column:char_grid) {
          for (std::string ch:column) {
            std::cout << ch;
          }
          //newline for row seperation
          std::cout << "\n";
        }

        return;
      };

      //TODO: Test functionality
      //Changed the visibility of the cursor
      //Inputs 1 boolean value, true will display cursor, false will hide cursor
      void setCursorVisibility(bool to_set) {
        //Do not do anything if the cursor visibility is the same as what is requested 
        if (to_set != cursor_visibility){
          //We know that what is requested is the opposite of the current value
          //So if cursor visibiltiy it currently true, mae it invisible and change value
          if (cursor_visibility) {
            std::cout << ESC+"?25l";
            cursor_visibility = false;
            return;
          } else {
            std::cout << ESC+"?25h";
            cursor_visibility = true;
            return;
          }
        }
      }

      //TODO: Test function, Implement any safety features
      //inputs 4 integers, 1 char, and 4 boolean parameters
      //4 integers are row to place, column to place, foreground color, and background color respectively
      //char is the single character to be formatted and placed in display array
      //4 boolean values are format values, bold, italic, underline, and blinking respectively
      void setChar(int row, int column, char chr, bool bold=false, bool italic=false, bool underline=false, bool blink=false, int fg_color=231, int bg_color=232)
      {
        //Fairly simple functionality, just a public wrapper of 2 private methods
        setElement(row, column, generateFormatChar(chr, bold, italic, underline, blink, fg_color, bg_color));
        return;
      }
  };

#endif

#endif