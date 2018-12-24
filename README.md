# Clunk
![Clunk Screenshot](https://i.imgur.com/1sPrqgk.png)

## About
Clunk is a terminal-based note-taking program that focuses on taking notes and getting out of the way. Some features:
- Notes are stored locally in ~/.clunk/
- All prompts will be in the lower left
- Notes are NOT automatically saved
- Lines beginning with ^ are highlighted
- Works on any system with ncurses installed
- Shift+Left/Right or Page Up/Down pages the screen
- Home/End go to the beginning and end of the line

## Installation
The only dependency to build is ncurses. To install, simply clone the source and run `make`. A `build` folder will be created with the executable inside.

## Testing
The tests are built with [Criterion.](https://github.com/Snaipe/Criterion) To execute them, run `make tests` and then run the `test\clunk_tests` executable.

