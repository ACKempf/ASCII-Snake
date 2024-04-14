# ASCII-Snake

## Introduction
ASCII snake is an adaptation of the famous snake game built to run on a command-line interface. The game works to demonstrate a TControl library that uses ANSI escape sequences and low-level terminal interface libraries to produce a grid of characters that can have any number of colors & 
format effects applied to them.

## Usage
Upon startup, ASCII snake requests the user to resize the terminal window to the desires size and press any key to continue and prompting the user to confirm this size. All other relevant instructions are given to the user within the game.

## Troubleshooting
If the menu fails to print, typically this means the window is too small to accomodate the size of the games menu, try restarting the steps detailed in usage with a bigger console window. 

## Features
ASCII snake aims to recreate the snake game with additional modes, and a high-level of customization. Using the in-game settings menu the 
player is able to tweak the graphics, controls, and timing in any way they please. 

## Requirements
ASCII snake requires nothing other than a MACOS system with access to the terminal application

## Installation
ASCII snake (when compiled) is an entirely portable executable file, depending on no external or non-standard libraries or files, and can be ran from anywhere on a MACOS system.

## Acknowledgements
###The following library documentation was used in the creation of this project
https://cplusplus.com/reference/stdexcept/
https://cplusplus.com/reference/exception/
https://pubs.opengroup.org/onlinepubs/7908799/xsh/termios.h.html
https://pubs.opengroup.org/onlinepubs/009604599/functions/ioctl.html
https://pubs.opengroup.org/onlinepubs/7908799/xsh/poll.h.html
###The following github was referenced for ANSI escape codes
https://gist.github.com/fnky/458719343aabd01cfb17a3a4f7296797
