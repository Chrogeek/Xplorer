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
