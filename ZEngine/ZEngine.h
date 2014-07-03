#ifndef ZENGINE_H
#define ZENGINE_H

#include <vector>
#include <ratio>
using namespace std;

#include "../ZObject/ZObject.h"
#include "../ZFilesystemManager/ZFilesystemManager.h"
#include "../ZAssetsManager/ZAssetsManager.h"
#include "../ZInputManager/ZInputManager.h"
#include "../ZVideoManager/ZVideoManager.h"
#include "../ZAudioManager/ZAudioManager.h"
#include "../ZUtil/ZUtil.h"
#include "../ZJSON/ZJSON.h"
#include "../ZTextManager/ZTextManager.h"
#include "../ZCamera/ZCamera.h"
#include "../ZEditor/ZEditor.h"
#include "../ZGUIManager/ZGUIManager.h"
#include "../ZScene/ZScene.h"

#include <SDL/SDL.h>
#include <SDL/SDL_opengl.h>
#include <Box2D/Box2D.h>
#include <chrono>

class ZTime {
public:
	float currentTime;
	float deltaTime;

	float scale;

	ZTime();

	void setDelta (float);
	void runTime (); // MUST be called in each frame for all timelines
};

class ZFPS {
	const float second = 1;
public:
	float _fps;
	float fps;

	ZFPS();

	void run ();
};

class b2qGetFirstObject: public b2QueryCallback {
public:
	ZObject* object;

	bool ReportFixture (b2Fixture *);
};

class ZEngine {
	long int maxOid; 	// Top id reached until now
	vector <ZObject*> *objects;  // List of objects
    bool exitFlag = false;
public:
	ZTime *realTime;
	ZTime *gameTime;

	ZFPS *fps;

	bool editMode = false;

	bool debug; // Debugging mode (set once, this can be changed)
	ZUtil			*util			= NULL; // [0] OK, COMPLETE
	ZFilesystemManager	*filesystemManager	= NULL; // [0.1] OK, COMPLETE
	ZJSON			*jsonManager		= NULL; // [0.2] OK, Implementing
	//ZSettingsManager    *settingsManager   = NULL; // [.5] Uninplemented
	//ZLogManager         *logManager        = NULL; // [1] Uninplemented
	ZTextManager		*textManager		= NULL;
	ZAssetsManager		*assetsManager		= NULL; // [1.5] OK, COMPLETE
	ZInputManager		*inputManager		= NULL; // [2] OK, COMPLETE
	ZVideoManager		*videoManager		= NULL; // [2] OK, COMPLETE
	ZAudioManager		*audioManager		= NULL; // [2] OK, COMPLETE
	ZEditor			*editor			= NULL;
	ZGUIManager		*guiManager		= NULL;
	ZSceneManager		*sceneManager		= NULL; // Scenes
public:
	ZCamera			*camera			= NULL;
	b2World			*box2dWorld		= NULL; // Physics

    float colorMultiplier;

	template <class Type>
	ZObject* createObject () {
		auto newObject = new Type (maxOid++);
		if (debug)
			cout << "[CRT] [OBJ] [id=" << newObject->oid << "]"<< endl;
		return (ZObject*)newObject;
	}

	ZEngine (bool=false);
	~ZEngine ();

	void addObject (ZObject*);
	bool removeObject (long int); // Use with extreme caution!
	ZObject* getObject (long int);
	ZObject* getObjectByPos (long int);
	long int getObjectsNumber ();

	// Game running related functions
	void init ();
	bool events ();
	void run ();
	void step();
	void controller();
	void quit();
	//void deinit();

	void loadMap (ZFilePath);
	void saveMap (ZFilePath);
private:
	float getDeltaTime();
};

extern ZEngine* Engine;

#endif /* ZENGINE_H */
