#include "ZAssetsManager.h"
#include "../ZEngine/ZEngine.h"
#include <cmath>

int callLoadFiles (void *c) {
	static_cast <ZAssetsManager*>(c)->loadFiles();
}

ZAsset::ZAsset (long int _aid,ZFilePath _path, string _name) {
	path = _path;
	name = _name;
	data = NULL;
	loaded = false;
	error = false;
	aid = _aid;
}

ZAsset::~ZAsset () {
	if (data != NULL)
		delete data;
}

void ZAsset::dump () {
	if (data != NULL)
		delete data;
	loaded = false;
}

void ZAsset::load () {
	loaded = true;
}

void ZAsset::sync () {

}

long int ZAsset::getAid () {
	return aid;
}

void ZAsset::setAid (long int _aid) {
	aid = _aid;
}

ZFilePath ZAsset::getPath () {
	return path;
}

string ZAsset::getName () {
	return name;
}

ZTextureAsset::ZTextureAsset (long int _id, ZFilePath _path, string _name):
	ZAsset::ZAsset (_id,_path,_name) {
		type = ZAsset::Texture;
	opacity = 1;
	color = Vec3 (0.25,0.25,0.25);
}

ZTextureAsset::~ZTextureAsset () {
	glDeleteTextures(1, &id);
}

void ZTextureAsset::load () {
    if (!Engine->filesystemManager->fileExists(path)) {
        cout << "[ERR] [FILE NOT FOUND] " << ZAsset::path.getPath() << endl;
        error = true;
	return;
    }

    surface = IMG_Load (ZAsset::path.getPath().c_str());
	width = surface->w;
	height = surface->h ;

	if (surface) {
		Engine->assetsManager->needSyncLoading(this);
	} else {
		cout << "[ERR] " << "[CANNOT LOAD BITMAP]" << ZAsset::path.getPath()  << " [aid=" << ZAsset::aid << "]" << endl;
	}

	loaded = false;
}

void ZTextureAsset::sync () {
    SDL_Surface *potSurface;

    int potW = pow(2,ceil(log2(surface->w)));
    int potH = pow(2,ceil(log2(surface->h)));

	rwidth = potW;
	rheight = potH;

    potSurface = SDL_CreateRGBSurface (surface->flags, potW, potH,
                                        surface->format->BitsPerPixel,
                                        surface->format->Rmask,
                                        surface->format->Gmask,
                                        surface->format->Bmask,
                                        surface->format->Amask);

    if (!potSurface) {
        cout << "[ERR] Creating [^2] surface" << endl;
    }

    SDL_SetAlpha(surface, 0, surface->format->alpha);
    //SDL_SetAlpha(potSurface, 0, potSurface->format->alpha);

    if (SDL_BlitSurface (surface, NULL, potSurface, NULL) == 0) {
        cout << "[INF] [CONV] [^2] {OK}" << endl;
    } else {
        cout << "[INF] [CONV] [^2] {FAIL}" << endl;
    }

    glGenTextures (1, &id);
	glBindTexture (GL_TEXTURE_2D, id);

	int imgMode = GL_BGR;

	if (path.getExtension() == "png")
		imgMode = GL_RGB;

	if (potSurface->format->BytesPerPixel == 4) {
		imgMode = GL_BGRA;
		if (path.getExtension() == "png")
			imgMode = GL_RGBA;
	}

	glTexImage2D(GL_TEXTURE_2D, 0, potSurface->format->BytesPerPixel, potSurface->w, potSurface->h,
								0, imgMode, GL_UNSIGNED_BYTE,
								potSurface->pixels);

	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
	glTexEnvi(GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_COMBINE);
	glTexEnvi(GL_TEXTURE_ENV, GL_COMBINE_RGB, GL_MODULATE);
	glTexEnvi(GL_TEXTURE_ENV, GL_SRC0_RGB, GL_TEXTURE);
	glTexEnvi(GL_TEXTURE_ENV, GL_SRC1_RGB, GL_PREVIOUS);
	glTexEnvi(GL_TEXTURE_ENV, GL_RGB_SCALE, 4.0);

	SDL_FreeSurface (surface);
	SDL_FreeSurface (potSurface);

	loaded = true;
}

void ZTextureAsset::setColor (Vec3 _color) {
	color = _color;
}

void ZTextureAsset::setOpacity (float _opacity) {
	opacity = _opacity;
}

void ZTextureAsset::draw (Vec3 _pos) {
	float sX = (float)width/(float)rwidth;
	float sY = (float)height/(float)rheight;
	glPushMatrix();
		glTranslatef (_pos.x,_pos.y,_pos.z);
		glEnable (GL_TEXTURE);
		glEnable (GL_ALPHA_TEST);
		glAlphaFunc (GL_GREATER, 0);
		glEnable (GL_BLEND);
		glBindTexture (GL_TEXTURE_2D, id);
		glColor4f(color.x,color.y,color.z, opacity);
		glBegin(GL_QUADS);
			glTexCoord2f (0,0); 	glVertex3f(0, 0, 0);
			glTexCoord2f (sX,0); 	glVertex3f(width, 0, 0);
			glTexCoord2f (sX,sY); 	glVertex3f(width, height, 0);
			glTexCoord2f (0,sY); 	glVertex3f(0, height, 0);
		glEnd();
	glPopMatrix();
}

void ZTextureAsset::drawCentered (Vec3 _pos) {
	float sX = (float)width/(float)rwidth;
	float sY = (float)height/(float)rheight;
	glPushMatrix();
		glTranslatef (_pos.x,_pos.y,_pos.z);
		glTranslatef (-width/2, -height/2, 0);
		glEnable (GL_TEXTURE);
		glEnable (GL_ALPHA_TEST);
		glAlphaFunc (GL_GREATER, 0);
		glEnable (GL_BLEND);
		glBindTexture (GL_TEXTURE_2D, id);
		glColor4f(color.x,color.y,color.z, opacity);
		glBegin(GL_QUADS);
			glTexCoord2f (0,0); 	glVertex3f(0, 0, 0);
			glTexCoord2f (sX,0); 	glVertex3f(width, 0, 0);
			glTexCoord2f (sX,sY); 	glVertex3f(width, height, 0);
			glTexCoord2f (0,sY); 	glVertex3f(0, height, 0);
		glEnd();
	glPopMatrix();
}

ZJSONAsset::ZJSONAsset (long int _id, ZFilePath _path, string _name):
	ZAsset::ZAsset (_id,_path,_name) {
	type = ZAsset::JSON;
	json = NULL;
}

ZJSONAsset::~ZJSONAsset () {
	if (json != NULL)
		delete json;
}

void ZJSONAsset::load () {
	if (!Engine->filesystemManager->fileExists(path)) {
		cout << "[ERR] [FILE NOT FOUND]" << path.getPath() << endl;
		error = true;
		loaded = false;
		return;
	}

	cout << "[INF] [LOADING JSON...]" << endl;

	// Já temos certeza de que o arquivo existe
	// Começamos a carrega-lo

	// Variável para armazenar os dados temporariamente
	char* fileData;
	// Lemos os dados do arquivo
	fileData = Engine->filesystemManager->getFileData(path);
	// Colocamos um caractere de finalização para tornar uma string válida
	fileData [Engine->filesystemManager->getFileSize(path)] = 0;

	cout << "[INF] [LOADED JSON FILE...]" << endl;

	string stringFileData = string (fileData);

	// Chamamos o serviço da própria engine para criar um objeto a
	// partir dos dados lidos
	json = Engine->jsonManager->parse (stringFileData);

	cout << "[INF] [STRING PARSED]" << endl;

	// Limpamos a memória utilizada
	free (fileData);

	// Avisamos que conseguimos carregar os dados
	loaded = true;
}

ZjObject* ZJSONAsset::getJSON () {
	return json;
}

// Initialize ZAsset and animation type
ZAnimationAsset::ZAnimationAsset (long int _id, ZFilePath _path, string _name):
	ZAsset::ZAsset (_id,_path,_name) {
	type = ZAsset::Animation;
}

// Load animation
void ZAnimationAsset::load () {
    if (!Engine->filesystemManager->fileExists(path)) {
        cout << "[ERR] [FILE NOT FOUND] " << ZAsset::path.getPath() << endl;
	error = true;
	return;
    }

    ZjObject *jsonAnimation;
	char *file;

	file = Engine->filesystemManager->getFileData(path);
	file [Engine->filesystemManager->getFileSize(path)] = 0;

	string jsonStringAnimation = string (file);
	jsonAnimation = Engine->jsonManager->parse (jsonStringAnimation);

	free(file);

	imagePath = ZFilePath(jsonAnimation->get ("file")->str);

	frameDuration = jsonAnimation->get("duration")->number;

	width = (int)jsonAnimation->get("width")->number;
	height = (int)jsonAnimation->get("height")->number;

	loop = (bool)(int)jsonAnimation->get("loop")->number;

	sheetType = SheetType::Horizontal;

	if (jsonAnimation->get("type")->str == "horizontal")
		sheetType = SheetType::Horizontal;
	else if (jsonAnimation->get("type")->str == "vertical")
		sheetType = SheetType::Vertical;

	direction = PlayDirection::Forward;

	if (jsonAnimation->get("direction")->str == "forward")
		direction = PlayDirection::Forward;
	else if (jsonAnimation->get("direction")->str == "backward")
		direction = PlayDirection::Backward;

	callback.frame = -1;

    if (!Engine->filesystemManager->fileExists(imagePath)) {
        cout << "[ERR] [FILE NOT FOUND] " << imagePath.getPath() << endl;
        return;
    }

	surface = IMG_Load (imagePath.getPath().c_str());
	swidth = surface->w;
	sheight = surface->h;

	if (sheetType == SheetType::Horizontal)
		frames = swidth/width;
	else if (sheetType == SheetType::Vertical)
		frames = sheight/height;

	if (surface) {
		Engine->assetsManager->needSyncLoading(this);
	} else {
		cout << "[ERR] " << "[CANNOT LOAD BITMAP]" << ZAsset::path.getPath()  << " [aid=" << ZAsset::aid << "]" << endl;
	}

	loaded = false;
}

void ZAnimationAsset::sync () {
    SDL_Surface *potSurface;

    int potW = pow(2,ceil(log2(swidth)));
    int potH = pow(2,ceil(log2(sheight)));

    rwidth = potW;
    rheight = potH;

    potSurface = SDL_CreateRGBSurface (surface->flags, potW, potH,
                                        surface->format->BitsPerPixel,
                                        surface->format->Rmask,
                                        surface->format->Gmask,
                                        surface->format->Bmask,
                                        surface->format->Amask);

    if (!potSurface) {
        cout << "[ERR] Creating [^2] surface" << endl;
    }

    SDL_SetAlpha(surface, 0, surface->format->alpha);
    //SDL_SetAlpha(potSurface, 0, potSurface->format->alpha);

    if (SDL_BlitSurface (surface, NULL, potSurface, NULL) == 0) {
        cout << "[INF] [CONV] [^2] {OK}" << endl;
    } else {
        cout << "[INF] [CONV] [^2] {FAIL}" << endl;
    }

    SDL_SaveBMP (potSurface, "log.bmp");

	glGenTextures (1, &id);
	glBindTexture (GL_TEXTURE_2D, id);

	int imgMode = GL_BGR;

	if (imagePath.getExtension() == "png")
		imgMode = GL_RGB;

	if (potSurface->format->BytesPerPixel == 4) {
		imgMode = GL_BGRA;
		if (imagePath.getExtension() == "png")
			imgMode = GL_RGBA;
	}

	glTexImage2D(GL_TEXTURE_2D, 0, potSurface->format->BytesPerPixel, potSurface->w, potSurface->h,
								0, imgMode, GL_UNSIGNED_BYTE,
								potSurface->pixels);

	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);

    SDL_FreeSurface (potSurface);
	SDL_FreeSurface (surface);

	loaded = true;
}

ZFontAsset::ZFontAsset (long int _id, ZFilePath _path, string _name):
	ZAsset::ZAsset (_id,_path, _name) {
	type = ZAsset::Font;
}

ZFontAsset::~ZFontAsset () {
	TTF_CloseFont (font);
}

void ZFontAsset::load () {
    if (!Engine->filesystemManager->fileExists(path)) {
        cout << "[ERR] [FILE NOT FOUND] " << ZAsset::path.getPath() << endl;
	error = true;
	return;
    }
	font = TTF_OpenFont(path.getPath().c_str(), 64);
	TTF_SetFontHinting (font, TTF_HINTING_LIGHT);
	TTF_SetFontKerning (font, 0);
	if (!font) {
		cout << "[ERR] " << "[CANNOT LOAD FONT]" << ZAsset::path.getPath()  << " [aid=" << ZAsset::aid << "]" << endl;
		loaded = false;
		error = true;
		return;
	}

	loaded = true;
}

void ZFontAsset::sync () {
}

ZAssetsManager::ZAssetsManager () {
	maxAid = 0;
	loadingAssetLock = SDL_CreateMutex();
	SDL_UnlockMutex (loadingAssetLock);
	run = true;
}

ZAssetsManager::~ZAssetsManager () {
	SDL_KillThread (loadFilesThread);

	long int aid;

	for (aid=0;aid<maxAid;aid++) {
		ZAsset *asset = NULL;

		try {
			asset = loadedAssets.at(aid);
		} catch (exception e) {}

		try {
			asset = loadingAssets.at(aid);
		} catch (exception e) {}

		try {
			asset = unloadedAssets.at(aid);
		} catch (exception e) {}

		if (asset != NULL) {
			cout << "[DEL] [ASSET] path=" << asset->getPath().getPath() << endl;
			delete asset;
		}
	}
}

long int ZAssetsManager::createAsset (ZFilePath _path, string _name) {
	/* When we create the first asset, the
	loadFiles thread is started */
	if (loadFilesThread == NULL) {
		cout << "[CRT] load files thread" << endl;
		loadFilesThread = SDL_CreateThread(callLoadFiles,this);
	}

	cout << "[INF] creating asset id=" << maxAid << ",path=" << _path.getPath()<< endl;
	ZAsset *asset;

	if (_path.getExtension() == "bmp" || _path.getExtension() == "png")
		asset = new ZTextureAsset (maxAid++,_path, _name);
	else if (_path.getExtension() == "ttf")
		asset = new ZFontAsset (maxAid++,_path, _name);
	else if (_path.getExtension() == "ogg")
		asset = new ZAudioAsset (maxAid++,_path, _name);
	else if (_path.getExtension() == "anim")
		asset = new ZAnimationAsset (maxAid++,_path, _name);
	else if (_path.getExtension() == "json" ||
		 _path.getExtension() == "scene")
		asset = new ZJSONAsset (maxAid++, _path, _name);
	else
		asset = new ZAsset (maxAid++,_path, _name);

	unloadedAssets.insert (unordered_map <long int, ZAsset*>::value_type (asset->getAid(),asset));

	assetPathToAid.insert (unordered_map <string, long int>::value_type (asset->getPath().getPath(),asset->getAid()));
	assetNameToAid.insert (unordered_map <string, long int>::value_type (asset->getName(), asset->getAid()));

	return asset->getAid();
}

long int ZAssetsManager::createAsset (ZAsset *_asset) {
	cout << "[INF] creating asset id=" << maxAid << ",path=" << "MEMORY" << endl;
	ZAsset *asset = _asset;

	asset->setAid (maxAid++);

	loadedAssets.insert (unordered_map <long int, ZAsset*>::value_type (asset->getAid(),asset));

	assetPathToAid.insert (unordered_map <string, long int>::value_type (asset->getPath().getPath(),asset->getAid()));
	assetNameToAid.insert (unordered_map <string, long int>::value_type (asset->getName(), asset->getAid()));

	return asset->getAid();
}

long int ZAssetsManager::getAssetsNum () {
	return maxAid;
}

void ZAssetsManager::needAsset (long int _aid) {
	SDL_LockMutex(loadingAssetLock);
		ZAsset *asset;
		try {
			asset = unloadedAssets.at(_aid);
		} catch (exception e) {
			asset = NULL;
		}
		if (asset != NULL) {
			unloadedAssets.erase (asset->getAid());
			loadingAssets.insert (unordered_map <long int, ZAsset*>::value_type (asset->getAid(),asset));
		}
	SDL_UnlockMutex(loadingAssetLock);
}

void ZAssetsManager::dumpAsset (long int _aid) {
	SDL_LockMutex(loadingAssetLock);
		ZAsset *asset;
		try {
			asset = loadedAssets.at(_aid);
		} catch (exception e) {
			asset = NULL;
		}
		if (asset != NULL) {
			loadedAssets.erase (asset->getAid());
			asset->dump();
			unloadedAssets.insert (unordered_map <long int, ZAsset*>::value_type (asset->getAid(),asset));
		}
	SDL_UnlockMutex(loadingAssetLock);
}

size_t ZAssetsManager::getAssetSize (long int _aid) {
	// Please don't use this
	// unimplemented function
}

void ZAssetsManager::loadFiles () {
	cout << "[INF] [ASYCHRONOUS FILE IO STARTED]" << endl;

	int milis = 0;

	while (run) {
		if (milis < 100)
			milis ++; // Each time we give a plus in the wait time
					  // This way we will not consume processor when
					  // there is nothing to load

		SDL_LockMutex (loadingAssetLock);
		if (loadingAssets.size() > 0) { // While something to load
			auto asset = loadingAssets.begin()->second;

			cout << "[INF] [LOADING FILE " << asset->getPath().getPath() << "] "
				 << endl;

			asset->load ();

			loadingAssets.erase (asset->getAid());

			if (!asset->error) {
				loadedAssets.insert (unordered_map <long int, ZAsset*>::value_type (asset->getAid(),asset));
				cout << "[INF] [FILE LOADED]" << endl;
				milis = 0; // Probabily we will have something to do very soon
			} else {
				// We had a problem! We couldn't load an asset!
				cout << "/!\\ [ERR] [FATAL] COULDN'T LOAD ASSET!" << asset->getPath().getPath() << endl;
				Engine->quit();
			}
		}
		SDL_UnlockMutex (loadingAssetLock);

		// Wait before checking again
		if (milis > 0)
			SDL_Delay (milis);
	}

	cout << "[INF] [FILE LOADING - DEINITIALIZED]" << endl;

	return;
}

void ZAssetsManager::needSyncLoading (ZAsset *asset) {
	cout << "[INF] [NEEDSYNC] [id=" << asset->getAid() << "]" << endl;
	syncLoading.push_back (asset);
}


// Call from the main thread (the one that uses OpenGL)
void ZAssetsManager::sync () {
	long int i;
	for (i=0;i<getAssetsNum();i++) {
		if (getAsset<ZAsset*>(i)->loaded)
			getAsset<ZAsset*>(i)->run();
	}

	while (syncLoading.size() > 0) {
		cout << "[MAIN THREAD] [SYNC] Asset id=" << syncLoading[0]->getAid() << endl;

		syncLoading[0]->sync();
		syncLoading.erase(syncLoading.begin(),syncLoading.begin()+1);
	}
}
