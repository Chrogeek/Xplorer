# Xplorer
A 2D jumping game. Also the assignment of Fundamentals of Programming course @ Tsinghua University.

## Project structure

* `main.cpp`: Initialization and termination.
* `geometry.h/cpp`: Implementation of geometric methods and functions. Features computational geometry.
* `handler.h/cpp`: Handles events and messages.
* `global.cpp`: Defines global variables and objects.
* `defs.h`: Declaration of constants and enumerations.
* `game.h/cpp`: Implements game main process.
* `utility.h/cpp`: Functions defined for help.
* `gameLevel.h/cpp`: Structure of the game (chapters/levels).
* `resource.h/rc`: Resource file.

## Data files

* `data/`: game main configuration directory
	* `config.json/`: game settings
* `chapters/`: game chapters folder
	* `config.json`: configuration for the whole game
	* `#/`: folder for a single chapter
		* `config.json`: configuration for this chapter
		* `#/`: folder for a single level
			* `data.json`: map data for the level
			* `tiles.json`: tile set for the level
			* `save.json`: save config for the level

## To-Do list

* [] Add statistics (for both single chapter and the whole game process)
* [] Add options
* [] Add About frame

* [] Add 'thanks for playing'
* [] Add music & sounds & more background pictures

* [] Add death splash (important)

* [] Improve double jumping

## Story

The game is set on faraway planets. Theme: the universe.

* Chapter 0: Welcome to Xplorer
* Chapter 1: Departure
* Chapter 2: Cathey
* Chapter 3: Dhosey
* Chapter 4: Khaiocey
* Chapter 5: Back to Earth
* Chapter 6: Where the Dream Ends
* Chapter 7: Thanks for Playing!

**The trial version contains only Chapters 0, 1, 4 and 7.**
