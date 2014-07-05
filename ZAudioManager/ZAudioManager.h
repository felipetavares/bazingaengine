#ifndef ZAUDIOMANAGER_H
#define ZAUDIOMANAGER_H

#include <cmath>
#include <iostream>
#include <SDL/SDL.h>
#include <SDL/SDL_mixer.h>
#include "../ZAssetsManager/ZAssetsManager.h"
#include "../ZFilesystemManager/ZFilesystemManager.h"

using namespace std;

// This class manages an OGG vorbis file
class ZAudioAsset: public ZAsset {
	Mix_Music *sound;
public:
	ZAudioAsset (long int, ZFilePath,string);
	~ZAudioAsset ();

	void load ();
	void sync ();
	void play ();
	void loop ();
};

class ZAudioManager {
	const int freq = 44100;
	const int bufferSize  = 8192;
public:
	bool support;

	ZAudioManager();
	~ZAudioManager();
};

#endif /* ZAUDIOMANAGER_H */
