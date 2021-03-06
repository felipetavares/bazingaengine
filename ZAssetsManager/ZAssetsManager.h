#ifndef ZASSETSMANAGER_H
#define ZASSETSMANAGER_H

#include "../ZFilesystemManager/ZFilesystemManager.h"
#include "../ZJSON/ZJSON.h"
#include "ZMath/math/vec3/vec3.h"
#include <vector>
#include <iostream>
#include <GL/gl.h>
#include <SDL/SDL.h>
#include <SDL/SDL_thread.h>
#include <SDL/SDL_image.h>
#include <SDL/SDL_ttf.h>
#include <unordered_map>
using namespace std;

class ZAsset {
protected:
	long int aid;
	ZFilePath path;
	string name;
public:
	bool error;
	bool loaded;
	char *data;

	enum Type {Texture=0,Font,Audio,Animation,JSON};

	Type type;

	ZAsset (long int,ZFilePath, string);
	virtual ~ZAsset ();

	void dump ();
	virtual void load ();
	virtual void sync ();
	virtual void run () {};

	long int getAid();
	void setAid (long int);

	ZFilePath getPath();
	string getName();
};

class ZTextureAsset: public ZAsset {
	SDL_Surface *surface;
	vec3 color;
	float opacity;
public:
	GLuint id;
	int width,height;
	int rwidth,rheight;

	ZTextureAsset (long int,ZFilePath, string);
	~ZTextureAsset();

	void load ();
	void sync ();

	void setOpacity(float);
	void setColor(vec3);
	void draw(vec3);
	void drawCentered(vec3);
};

class ZAnimationAsset: public ZAsset {
	SDL_Surface *surface;
	SDL_Surface *potSurface;
	ZFilePath imagePath;
public:
	enum PlayDirection {Forward,Backward};
	PlayDirection direction;
	enum SheetType {Horizontal,Vertical};
	SheetType     sheetType;

	int frames;

	GLuint id;
	int width;   // frame width
	int height;  // frame height

	int swidth;  // Sheet width
	int sheight; // Sheet height

	int rwidth;  // Sheet in-memory width
	int rheight; // Sheet in-memory height

	float frameDuration; // In seconds (based upon game time)

	bool loop;

	struct callback {
		int frame;
		std::function <void()> call;
	}callback;

	ZAnimationAsset(long int, ZFilePath, string);

	void load ();
	void sync ();
};

class ZFontAsset: public ZAsset {
public:
	TTF_Font *font;

	ZFontAsset (long int, ZFilePath,string);
	~ZFontAsset();

	void load();
	void sync();
};

class ZJSONAsset: public ZAsset {
	ZjObject *json;
public:
	ZJSONAsset (long int, ZFilePath,string);
	~ZJSONAsset();

	void load();

	ZjObject* getJSON ();
};

class ZAssetsManager {
	unordered_map <string, long int> assetPathToAid;
	unordered_map <string, long int> assetNameToAid;

	unordered_map <long int, ZAsset*> unloadedAssets;
	unordered_map <long int, ZAsset*> loadingAssets;
	unordered_map <long int, ZAsset*> loadedAssets;
	long int maxAid;

	SDL_Thread* loadFilesThread = NULL;
	SDL_mutex*  loadingAssetLock;

	vector <ZAsset*> syncLoading;

	bool run;
public:
	ZAssetsManager ();
	~ZAssetsManager ();

	void loadFiles();

	long int createAsset (ZFilePath, string);
	long int createAsset (ZAsset*);
	long int getAssetsNum ();

	void dumpAsset (long int);
	void needAsset (long int);
	template <class AssetType>
	AssetType getAsset (long int _aid) {
		while (true) {// blocks until asset isn't loaded
            fflush (stdout);
			//SDL_LockMutex(loadingAssetLock);
				ZAsset *asset;
				try {
					asset = loadedAssets.at(_aid);
				} catch (exception e) {
					asset = NULL;
				}
				if (asset != NULL)
					return (AssetType)asset;
			//SDL_UnlockMutex(loadingAssetLock);
			SDL_Delay(50);
		}
	}
	template <class AssetType>
	AssetType getAsset (ZFilePath _path) {
		fflush (stdout);

		long int aid;

		try {
			aid = assetPathToAid.at(_path.getPath());
		} catch (exception e) {
			aid = -1;
		}

		if (aid < 0) {
			return NULL;
		}

		while (true) {// blocks until asset isn't loaded
			//SDL_LockMutex(loadingAssetLock);
				ZAsset *asset;
				try {
					asset = loadedAssets.at(aid);
				} catch (exception e) {
					asset = NULL;
				}
				if (asset != NULL) {
					return (AssetType)asset;
				}
			//SDL_UnlockMutex(loadingAssetLock);
			SDL_Delay(50);
		}
	}
	template <class AssetType>
	AssetType getAsset (string _name) {
		fflush (stdout);

		long int aid;

		try {
			aid = assetNameToAid.at(_name);
		} catch (exception e) {
			aid = -1;
		}

		if (aid < 0) {
			return NULL;
		}

		while (true) {// blocks until asset isn't loaded
			//SDL_LockMutex(loadingAssetLock);
				ZAsset *asset;
				try {
					asset = loadedAssets.at(aid);
				} catch (exception e) {
					asset = NULL;
				}
				if (asset != NULL) {
					return (AssetType)asset;
				}
			//SDL_UnlockMutex(loadingAssetLock);
			SDL_Delay(50);
		}
	}
	size_t getAssetSize (long int);

	void needSyncLoading (ZAsset *asset);

	void sync ();
};

#endif /* ZASSETSMANAGER_H */
