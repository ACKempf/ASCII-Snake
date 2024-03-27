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

  //MACOS CODE HERE

#endif

#endif