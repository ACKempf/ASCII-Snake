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

#ifndef TCONTROL_H
#define TCONTROL_H

//#ifdef will only compile what is within it if the object is defined
//Below is a link to pre-defined OS specific macros
//https://sourceforge.net/p/predef/wiki/OperatingSystems/
//With this we can deduce what operatin system is compiling the file

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

  //TODO: This class is more or less a shell, I will only begin working on it when i finish dchar struct
  class Terminal
  {
    //Constructor
    //Accepts 2 integers as size, representing the x-size (columns) and y-size (rows) of the console
    //Initializes the constant values for rows and columns
    //Initializes a matrix of strings (string rather than chars because ANSI data will be baked into them)
    public: Terminal(int r, int c) : rows(r), columns(c), char_grid(r, std::vector<std::string>(c, " ")){};

    private:
      //Variables for the x and y size of the terminal screen
      const int rows;
      const int columns;

      //The display array
      //NOTE: Matrix is a vector of horizontal lines of formatted characters
      std::vector<std::vector<std::string>> char_grid;

      //Useful resource for ANSI escape sequences
      //https://gist.github.com/fnky/458719343aabd01cfb17a3a4f7296797

      //String that contains the ANSI escape sequences to both clear the screen -
      //and return the cursor to the top-left "home" position
      const std::string ANSI_CLEAR = ESC + "2J" + ESC + "H";

    public:

      //Member Functions for getting the size variables of the console
      int getRows() const {return rows;}
      int getColumns() const {return columns;}

      //Draws the contents of the display array to the screen
      void draw() 
      {
        //Nested range-based for loops, outer to iterate rows, inner to iterate individual characters in rows
        //NOTE: ch is not a character type, but the way "characters" are added to the array, other parts of the string are ANSI escape sequences
        //So the element should always print to console exactly 1 character that is formatted
        for (std::vector<std::string> column:char_grid) {
          for (std::string ch:column) {
            std::cout << ch;
          }
          //seperate columns
          std::cout << "\n";
        }

        return;
      };

  };

#endif

#endif