#include <io.h>
#include <windows.h>
#include "utility.h"
#include "music.h"

extern HWND gameWindow;

const std::string musicDirectory = "./music/";
std::vector<musicFile> fileList;

musicFile::musicFile(std::string fileName, bool playLoop, int count) {
	this->fileName = fileName;
	this->playLoop = playLoop;
	this->count = count;
}

void addMusicList() {
	fileList.clear();
	// Start of music list
	// Must be corresponding to the constants defined in defs.h
	addToFileList("main.mp3", true);
	addToFileList("game.mp3", true);
	addToFileList("about.mp3", true);

	addToFileList("penalty.wav", false);
	addToFileList("save.wav", false);
	addToFileList("death.wav", false);
	addToFileList("levelUp.wav", false);
	addToFileList("collision.wav", false);
	addToFileList("click.wav", false);
	// End of music list
}

void addToFileList(std::string fileName, bool playLoop, int count) {
	fileList.push_back(musicFile(fileName, playLoop, count));
}

QWORD getPlayingPosition(int id, int index) {
	if (id < 0 || id >= (int)fileList.size()) return -1;
	if (fileList[id].hMusic[index] != NULL) {
		return BASS_ChannelGetPosition(fileList[id].hMusic[index], BASS_POS_BYTE);
	} else return -1;
}

QWORD getPlayingPosition(std::string id, int index) {
	return getPlayingPosition(findListIndex(id), index);
}

double getPlayingSecond(int id, int index) {
	if (id < 0 || id >= (int)fileList.size()) return -1.0;
	if (fileList[id].hMusic[index] != NULL) {
		QWORD ans = BASS_ChannelGetPosition(fileList[id].hMusic[index], BASS_POS_BYTE);
		return BASS_ChannelBytes2Seconds(fileList[id].hMusic[index], ans);
	} else return -1.0;
}

double getPlayingSecond(std::string id, int index) {
	return getPlayingSecond(findListIndex(id), index);
}

QWORD getMusicLength(int id, int index) {
	if (id < 0 || id >= (int)fileList.size()) return -1;
	if (fileList[id].hMusic[index] != NULL) {
		return BASS_ChannelGetLength(fileList[id].hMusic[index], BASS_POS_BYTE);
	} else return -1;
}

QWORD getMusicLength(std::string id, int index) {
	return getMusicLength(findListIndex(id), index);
}

double getMusicLengthSecond(int id, int index) {
	if (id < 0 || id >= (int)fileList.size()) return -1.0;
	if (fileList[id].hMusic[index] != NULL) {
		QWORD ans = BASS_ChannelGetLength(fileList[id].hMusic[index], BASS_POS_BYTE);
		return BASS_ChannelBytes2Seconds(fileList[id].hMusic[index], ans);
	} else return -1.0;
}

double getMusicLengthSecond(std::string id, int index) {
	return getMusicLengthSecond(findListIndex(id), index);
}

void initializeMusic() {
	addMusicList();
	if ((_access("bass.dll", 0)) != -1) {
		if (HIWORD(BASS_GetVersion()) != BASSVERSION) {
			MessageBox(nullptr, "BASS.dll version incompatible.", "BASS Initialization Error", MB_ICONERROR);
			PostQuitMessage(0);
		}
		if (!BASS_Init(-1, 44100, 0, gameWindow, 0)) {
			MessageBox(gameWindow, "Failed to initialize digital output.", "BASS Initialization Error", MB_ICONERROR);
			PostQuitMessage(0);
		}
	}
	for (auto &o : fileList) {
		o.hMusic.resize(o.count);
		for (int i = 0; i < (int)o.count; ++i) {
			o.hMusic[i] = BASS_StreamCreateFile(false, (musicDirectory + o.fileName).c_str(), 0, 0, o.playLoop ? BASS_SAMPLE_LOOP : 0);
		}
	}
}

void terminateMusic() {
	if ((_access("bass.dll", 0)) == -1) return;
	for (auto &o : fileList) {
		for (int i = 0; i < (int)o.count; ++i) {
			if (o.hMusic[i] != NULL) {
				BASS_StreamFree(o.hMusic[i]);
			}
		}
	}
	BASS_Free();
}

void playMusic(int id, bool restart, int index) {
	if (id < 0 || id >= (int)fileList.size()) return;
	if (fileList[id].hMusic[index] != NULL) {
		BASS_ChannelPlay(fileList[id].hMusic[index], getPlayingSecond(id, index) < 0.08 ? false : restart);
	}
}

void playMusic(std::string id, bool restart, int index) {
	playMusic(findListIndex(id), restart, index);
}

void stopMusic(int id, bool restart, int index) {
	if (id < 0 || id >= (int)fileList.size()) return;
	if (index == -1) {
		for (int j = 0; j < (int)fileList[id].count; ++j) {
			if (fileList[id].hMusic[j] != NULL) {
				BASS_ChannelStop(fileList[id].hMusic[j]);
				if (restart) {
					BASS_ChannelSetPosition(fileList[id].hMusic[j], 0, BASS_POS_BYTE);
				}
			}
		}
	} else {
		if (fileList[id].hMusic[index] != NULL) {
			BASS_ChannelStop(fileList[id].hMusic[index]);
			if (restart) {
				BASS_ChannelSetPosition(fileList[id].hMusic[index], 0, BASS_POS_BYTE);
			}
		}
	}
}

void stopMusic(std::string id, bool restart, int index) {
	stopMusic(findListIndex(id), restart, index);
}

int findListIndex(std::string str) {
	str = toLower(str);
	for (int i = 0; i < (int)fileList.size(); ++i) {
		if (toLower(fileList[i].fileName) == str) return i;
	}
	return -1;
}

void setVolume(float volume) {
	for (auto &o : fileList) {
		for (int i = 0; i < (int)o.count; ++i) {
			BASS_ChannelSetAttribute(o.hMusic[i], BASS_ATTRIB_VOL, volume);
		}
	}
}

HSTREAM getMusicHandle(int id, int index) {
	if (id < 0 || id >= (int)fileList.size()) return NULL;
	return fileList[id].hMusic[index];
}

HSTREAM getMusicHandle(std::string id, int index) {
	return getMusicHandle(findListIndex(id), index);
}
