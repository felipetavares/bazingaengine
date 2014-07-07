#ifndef ZINPUTMANAGER_H
#define ZINPUTMANAGER_H

#include <functional>
#include <iostream>
#include <string>
#include <sstream>
#include <vector>
#include <SDL/SDL.h>
#include "ZMath/math/vec3/vec3.h"

using namespace std;

class ZKeyboard;
class ZMouse;
class ZJoystick;

#include "../ZGUIManager/ZGUIManager.h"

class ZJoystickButton {
	string name;
public:
	bool shot;
	bool state;
	ZJoystickButton (string);
	~ZJoystickButton ();

	bool isShot();
	string getName ();
};

class ZJoystickAxis {
public:
	float value;
};

class ZJoystick {
	long int jid;
public:
	SDL_Joystick *sdlJoystick;
	string deviceName;
	vector <ZJoystickButton> buttons;
	vector <ZJoystickAxis> axis;

	ZJoystick (long int);
	~ZJoystick ();

	void press (int);
	void unpress (int);
	void move (int,int);

	long int getJid();
private:
	void sendEvent();
};

class ZKeyboard {
	int unicode;
	long int kid;
public:
	bool *keys;
	bool *skeys;

	ZKeyboard (long int);
	~ZKeyboard();

	void press (int);
	void unpress (int);
	void setUnicode (int);
	int getUnicode ();

	bool isShot(int);
	long int getKid();
private:
	void sendEvent();
};

class ZMouse {
	long int mid;
public:
	vec3 pos; // x,y, scroll
	bool lshot,rshot,cshot;
	bool left,right,center;

	bool scrollDown,scrollUp;
	bool sDownShot,sUpShot;

	ZMouse (long int);
	~ZMouse ();

	void move (float,float);
	void scroll (float);
	void press (int);
	void unpress (int);

	bool leftIsShot();
	bool rightIsShot();
	bool centerIsShot();

	void clear();
private:
	void sendEvent();
};

class ZInputManager {
	long int mJid;
	long int mMid;
	long int mKid;

	vector <ZJoystick*> *joysticks;
	vector <ZMouse*>    *mice;
	vector <ZKeyboard*> *keyboards;
public:
	ZInputManager ();
	~ZInputManager ();

	void joystickAxisEvent (SDL_Event&);
	void joystickButtonEvent (SDL_Event&);
	void keyboardKeyEvent (SDL_Event&);
	void mouseButtonEvent (SDL_Event&);
	void mouseMoveEvent (SDL_Event&);
	void mouseScrollEvent (SDL_Event&);

	vector<ZJoystick*> getJoysticks ();
	vector<ZKeyboard*> getKeyboards ();
	vector<ZMouse*>    getMice();

	void clear();
};

#endif /* ZINPUTMANAGER_H */
