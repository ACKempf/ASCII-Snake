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

### 2. Develop Game Mechanics
- [ ] Implement Tetris grid.
- [ ] Create and display Tetromino shapes within the grid.
- [ ] Implement boundary detection to prevent Tetriminos from moving beyond the grid boundaries.
- [ ] Implement collision detection to detect when Tetriminos collide with other Tetriminos or the boundaries.
- [ ] Allow rotation of Tetriminos while ensuring they do not collide with other Tetriminos or the boundaries.
- [ ] Implement Tetrimino movement speed control, adjusting how quickly Tetriminos fall down the grid.

### 3. Develop Game Logic
- [ ] Implement a mechanism to control the speed of Tetrimino descent.
- [ ] game states (ex. playing, paused, game over).
- [ ] Implement game over detection to end the game when Tetriminos reach the top of the grid.
- [ ] Implement scoring mechanism based on cleared rows(?).
- [ ] Implement level progression(?)
- [ ] Implement statistics screen to show the amount of times each tetrimino has been placed on the grid during the game(?).

### 4. Miscellaneous
- [ ] Add settings menu with options (expand on this later).
- [ ] Add color (Background and tetromino)
- [ ] Add music(?)
- [ ] Implement file to log and save high scores(?)
- [ ] Add support for Windows(?)
