#ifndef ZVIDEOMANAGER_H
#define ZVIDEOMANAGER_H

#include <iostream>
#include <string>
#include "../ZFilesystemManager/ZFilesystemManager.h"
#include <SDL/SDL.h>
#include <SDL/SDL_opengl.h>
#include <vector>
using namespace std;

class ZVideoManager {
	vector <SDL_Rect> *videoModes;
	bool anyResolution;

	SDL_Surface *screen;

	const Uint32 videoFlags = SDL_FULLSCREEN | SDL_OPENGL | SDL_HWSURFACE | SDL_DOUBLEBUF;
	ZFilePath icon;
public:
	int windowBpp;
	int windowWidth;
	int windowHeight;

	ZVideoManager();
	~ZVideoManager();

	void setWindowTitleAndIcon (string,string="");
	void render ();
private:
	void getVideoModes ();
	void findBestVideoMode ();
};

#endif /* ZVIDEOMANAGER_H */
