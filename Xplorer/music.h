#ifndef XPLORER_MUSIC_H
#define XPLORER_MUSIC_H

#include <string>
#include <vector>
#include "bass.h"

struct musicFile {
	std::string fileName;
	bool playLoop; // whether the music loops automatically
	int count; // number of handles
	std::vector<HSTREAM> hMusic; // handle list
	musicFile(std::string, bool, int);
};

void addMusicList();
void addToFileList(std::string, bool = false, int = 1);

QWORD getPlayingPosition(int, int = 0);
QWORD getPlayingPosition(std::string, int = 0);

double getPlayingSecond(int, int = 0);
double getPlayingSecond(std::string, int = 0);

QWORD getMusicLength(int, int = 0);
QWORD getMusicLength(std::string, int = 0);

double getMusicLengthSecond(int, int = 0);
double getMusicLengthSecond(std::string, int = 0);

void initializeMusic();
void terminateMusic();

void playMusic(int, bool = false, int = 0);
void playMusic(std::string, bool = false, int = 0);

void stopMusic(int, bool = true, int = -1);
void stopMusic(std::string, bool = true, int = -1);

int findListIndex(std::string);

void setVolume(float);

#endif
