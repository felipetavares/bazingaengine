#ifndef ZSCENE_H
#define ZSCENE_H

#include "../ZFilesystemManager/ZFilesystemManager.h"
#include "../ZAssetsManager/ZAssetsManager.h"

#define SCENE_BASE ".:Assets:Scenes:"

class ZScene {
protected:
	ZJSONAsset *data;
public:
	ZScene (string);

	virtual void load () = 0;
	virtual void render () = 0;

	void loadData(string);
};

class scnGame: public ZScene {
public:
	scnGame(string);
	void load ();
	void render ();
};

class scnIntro: public ZScene {
    vector <ZjValue*> list;
    vector <ZjValue*> time;
    int currentText;
    float changeTime;
public:
    scnIntro(string);
    void load ();
    void render ();
};

class ZSceneManager {
	vector <ZScene*> scenes;
	ZScene* currentScene;
public:
	ZSceneManager();
    ~ZSceneManager();

	void addScene (ZScene*);
	void setCurrentScene (ZScene*);
	void render();
};

#endif /* ZSCENE_H */
