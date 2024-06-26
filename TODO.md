## TO-DO List

### 1. Create a library with macOS-Specific Functions
- [x] Set up a separate library project for macOS-specific functions.
- [x] Implement private method for changing data in display "character" array.
- [x] Implement private method for generating a formatted "character" with ANSI sequences.
- [x] Implement public method for collecting format preferences and placing appropriate string in array.
- [x] Implement color formatting into generateFormatChar() method.
- [x] Implement private method of clearing screen.
- [x] Change the draw() method to clear the screen prior to drawing the display table.
- [x] Implement cursor visibility options.
- [x] Implement method for changing whether echo is enabled.
- [x] Implement function for retrieving the size of the console window.
- [x] Implement a way to have non-blocking input
- [x] Test the library functions on macOS to ensure compatibility and functionality.

*NOTE: I place "character" in quotes because the type of the elements in the array are string, but the class should be structured in such a way that those strings only actually print 1 character to console, the string is used to contain the ANSI escape sequences and the single character to be printed*

### 2. Develop Core Functionality
- [x] Implement snake grid system
- [x] Implement snake object
- [x] Implement snake movement
- [x] Implement snake growth
- [x] Implement food placement mechanics
- [x] Implement food collision "eating" mechanics
- [x] Implement wall collision "game-over" mechanics
- [x] Implement self collision "game-over" mechanics

### 3. Implement QOL features
- [x] Implement directional graphic for snake head
- [ ] Implement possible "slithering" graphic effect for snake body using italic effect
- [x] Implement game margin and border
- [x] Implement score counter
- [x] Implement gameover restart screen
- [x] Implement cursor based main menu
- [ ] Implement file to log and save high scores(?)
- [ ] Implement method to browse high scores from menu
- [x] Implement settings menu
- [x] Implement pause functionality

### 4. If there is time
- [x] Allow changing of core & graphics settings from settings menu
- [x] Allow changing of timing and control settings from the settings menu
- [ ] Implement additional gameplay modes
- [ ] Implement dynamically resizing the screen while playing
