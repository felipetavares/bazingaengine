#include "ZEngine.h"
#include "ZObject/Objects/ZPlayerObject.h"
#include "ZObject/Objects/ZDoorObject.h"
#include "ZObject/Objects/ZCabinetObject.h"

ZTime::ZTime () {
	currentTime = 0;
	deltaTime   = 0;
	scale       = 1;
}

void ZTime::setDelta (float _deltaTime) {
	deltaTime = _deltaTime*scale;
}

void ZTime::runTime () {
	currentTime += deltaTime;
}

ZFPS::ZFPS () {
	_fps = 0;
	fps = 0;
}

void ZFPS::run () {
    static auto _time = chrono::high_resolution_clock::now();
    auto now = chrono::high_resolution_clock::now();

    float d = chrono::duration_cast<chrono::seconds>(now - _time).count();
    if (d < second) {
        _fps ++;
    } else {
        _time = chrono::high_resolution_clock::now();
        fps = _fps;
        _fps = 0;
    }
}

ZEngine::ZEngine (bool _debug) {
	// Initialize the id pointer
	maxOid = 0;

	// Initialize the object's vector
	objects = new vector <ZObject*>();

	// Initialize debug (defaults to false)
	debug  = _debug;

	fps = new ZFPS();

	realTime = new ZTime();
	gameTime = new ZTime();

	if (debug) {
		cout << "[INF] [INITIALIZING SDL]" << endl;
	}

	if (SDL_Init (SDL_INIT_EVERYTHING) < 0) {
		cout << "[ERR] [CANNOT INIT SDL]" << endl;
		exit (-1);
	}

	SDL_EnableUNICODE(SDL_ENABLE);

	if (TTF_Init() < 0) {
		cout << "[ERR] [CANNOT INIT TTF FONT SUPPORT] [" << TTF_GetError() << "]" << endl;
		exit (-1);
	} else {
		cout << "[INF] [TTF FONT SUPPORTED]" << endl;
	}

	filesystemManager = new ZFilesystemManager();
	assetsManager     = new ZAssetsManager();
	inputManager      = new ZInputManager();
	videoManager      = new ZVideoManager();
	textManager		  = new ZTextManager();
	audioManager      = new ZAudioManager();
	jsonManager		  = new ZJSON();
	util			  = new ZUtil();
	box2dWorld		  = new b2World(b2vec2(0,0), false); // No gravity
	camera			  = new ZCamera();
	editor			  = new ZEditor();
	guiManager		  = new ZGUIManager();
	sceneManager		= new ZSceneManager();

	colorMultiplier = 1;

	guiManager->onclick = [] (ZUI::Widget* _w, ZMouse *_m) {
		if (_m->pos.x >= _w->p.x && _m->pos.x <= _w->p.x+_w->s.x &&
			_m->pos.y >= _w->p.y && _m->pos.y <= _w->p.y+_w->s.y)
			return _m->leftIsShot();
		return false;
	};

	guiManager->keydown = [] (ZUI::Widget* _w, ZKeyboard *_k) {
		if (_k->getUnicode() > 0)
			return true;
		return false;
	};
}

ZEngine::~ZEngine () {
	for (auto o : (*objects) ) {
		if (debug)
			cout << "[DEL] [OBJ] [id=" << o->oid << "]"<< endl;
		if (o)
			delete o;
	}

	fflush(stdout);

	delete objects;

	delete editor;
	delete box2dWorld;
	delete camera;

	delete sceneManager;
	delete guiManager;
	delete filesystemManager;
	delete assetsManager;
	delete textManager;
	delete videoManager;
	delete inputManager;
	delete audioManager;
	delete jsonManager;
	delete util;

	TTF_Quit();
	SDL_Quit();

	delete realTime;
	delete gameTime;

	delete fps;
}

void ZEngine::addObject (ZObject* object) {
	if (debug)
		cout << "[ADD] [OBJ] [id=" << object->oid << "]"<< endl;
	object->init();
	objects->push_back (object);
}

bool ZEngine::removeObject (long int oid) {
	auto i = objects->begin();

	for (auto o : (*objects) )  {
		if (o->oid == oid) {
			objects->erase (i);

			return true;
			break;
		}

		i++;
	}

	return false;
}

ZObject* ZEngine::getObject (long int oid) {
	for (auto o : (*objects) ) {
		if (o->oid == oid)
			return o;
	}

	return NULL;
}

ZObject* ZEngine::getObjectByPos (long int oid) {
	if (oid < objects->size()) {
		return (*objects)[oid];
	}
	return NULL;
}

long int ZEngine::getObjectsNumber () {
	return objects->size();
}

void ZEngine::run () {
	while (!exitFlag) {
		fps->run();
		float deltaTime = getDeltaTime();
		realTime->setDelta (deltaTime);
		gameTime->setDelta (realTime->deltaTime);
		realTime->runTime();
		gameTime->runTime();

		if (!events()) {
			// Wait for all subsystems to shutdown
			//SDL_Delay (300);
			return;
		}

		if (debug || editMode) {
			camera->debugMovement();
			controller();
		} else
			camera->run();

		step();

		videoManager->render();

		assetsManager->sync();

		stringstream ss;
		ss << fps->fps << "fps " << (int)realTime->currentTime << "sec.";
		videoManager->setWindowTitleAndIcon (ss.str());

		SDL_Delay (10);
	}
}

void ZEngine::quit () {
    exitFlag = true;
}

void ZEngine::step () {
	for (auto& o :(*objects)) {
		o->step();
	}
	box2dWorld->Step (Engine->gameTime->deltaTime,6,2);
    box2dWorld->ClearForces();
}

float ZEngine::getDeltaTime () {
	if (fps->fps != 0)
		return 1/fps->fps;
	else
		return 0;
}

bool ZEngine::events () {
    if (exitFlag)
        return !exitFlag;

	SDL_Event event;

    while ( SDL_PollEvent(&event) ) {
        switch (event.type) {
            case SDL_JOYBUTTONDOWN:
            case SDL_JOYBUTTONUP:
            	inputManager->joystickButtonEvent (event);
            break;
            case SDL_JOYAXISMOTION:
            	inputManager->joystickAxisEvent (event);
            break;
            case SDL_KEYDOWN:
            case SDL_KEYUP:
            	if (event.key.keysym.sym != SDLK_ESCAPE)
            		inputManager->keyboardKeyEvent (event);
            	else
            		return false;
            break;
            case SDL_MOUSEMOTION:
            	inputManager->mouseMoveEvent (event);
            break;
            case SDL_MOUSEBUTTONDOWN:
            case SDL_MOUSEBUTTONUP:
            	inputManager->mouseButtonEvent (event);
            break;
            case SDL_QUIT:
                return false;
        }

        if (inputManager->getKeyboards()[0]->keys[SDLK_BACKSPACE])
        	editMode = !editMode;
    }

    return true;
}

void ZEngine::controller () {
	editor->run();
}

void ZEngine::loadMap (ZFilePath _path) {
	if (!Engine->filesystemManager->fileExists(_path))
		return;

	char *file;
	file = Engine->filesystemManager->getFileData(_path);
	file [Engine->filesystemManager->getFileSize(_path)] = 0;

	string *jsonData = new string (file);

	ZjObject *map = Engine->jsonManager->parse (*jsonData);

	delete jsonData;

	for (int i=0;i<map->keys.size();i++) {
		auto def = map->values[i];
		auto type = map->keys[i];

		if (type == "o" && def->type == ZjValue::jObject) {
			auto oDef = def->object;
			ZObject* object = NULL;

			// Add types overhere
			if (oDef->values[0]->str == "box")
			   object = Engine->createObject <ZBoxObject> ();
			else if (oDef->values[0]->str == "door")
			   object = Engine->createObject <ZDoorObject> ();
			else if (oDef->values[0]->str == "cabinet")
			   object = Engine->createObject <ZCabinetObject> ();
			else
				continue;

			object->position->x = oDef->get("x")->number;
			object->position->y = oDef->get("y")->number;
			object->position->z = oDef->get("z")->number;
			object->graphic->position->x = oDef->get("gx")->number;
			object->graphic->position->y = oDef->get("gy")->number;
			object->graphic->position->z = oDef->get("gz")->number;

			object->size->x = oDef->get("sx")->number;
			object->size->y = oDef->get("sy")->number;
			object->size->z = oDef->get("sz")->number;
			object->graphic->size->x = oDef->get("sgx")->number;
			object->graphic->size->y = oDef->get("sgy")->number;
			object->graphic->size->z = oDef->get("sgz")->number;

			object->rotation->z = oDef->get("rz")->number;

			object->phys = (bool)(int)oDef->get("col")->number;
			object->background = (bool)(int)oDef->get("back")->number;

			auto asset = Engine->assetsManager->getAsset <ZAsset*> (oDef->get("gtexture")->str);

			if (asset->type == ZAsset::Type::Texture)
				object->graphic->texture = (ZTextureAsset*)asset;
			if (asset->type == ZAsset::Type::Animation)
				object->graphic->animation = new ZAnimation((ZAnimationAsset*)asset);

			Engine->addObject (object);
		}
	}

	delete map;
}

void ZEngine::saveMap (ZFilePath _path) {
	auto menu = new ZUI::Panel (vec2(ww/2-150,wh/2-50),vec2(300,100), 24);
	auto bStatus = new ZUI::wProgressBar("Saving ...");
	auto bOk = new ZUI::wButton("Ok");

	bOk->addAction (
		new ZUI::Filter (bOk, Engine->guiManager->onclick),
		new ZUI::Action ([=] (ZUI::Widget* _w, ZUI::Event *_m) {
			Engine->guiManager->removePanel(menu);
		})
	);

	menu->addWidget(bStatus);
	menu->addWidget(bOk);

	menu->setVisibility(true);
	Engine->guiManager->addPanel(menu);

	ZjObject *map = new ZjObject();

	float completed = 0;

	for (auto o :(*objects)) {
		if (!o->mapped)
			continue;
		ZjObject *jsonObject = new ZjObject();

		string objType = o->getType();
		jsonObject->keys.push_back ("type"); jsonObject->values.push_back (new ZjValue(objType));
		jsonObject->keys.push_back ("x"); jsonObject->values.push_back (new ZjValue(o->position->x));
		jsonObject->keys.push_back ("y"); jsonObject->values.push_back (new ZjValue(o->position->y));
		jsonObject->keys.push_back ("z"); jsonObject->values.push_back (new ZjValue(o->position->z));
		jsonObject->keys.push_back ("sx"); jsonObject->values.push_back (new ZjValue(o->size->x));
		jsonObject->keys.push_back ("sy"); jsonObject->values.push_back (new ZjValue(o->size->y));
		jsonObject->keys.push_back ("sz"); jsonObject->values.push_back (new ZjValue(o->size->z));

		jsonObject->keys.push_back ("gx"); jsonObject->values.push_back (new ZjValue(o->graphic->position->x));
		jsonObject->keys.push_back ("gy"); jsonObject->values.push_back (new ZjValue(o->graphic->position->y));
		jsonObject->keys.push_back ("gz"); jsonObject->values.push_back (new ZjValue(o->graphic->position->z));
		jsonObject->keys.push_back ("sgx"); jsonObject->values.push_back (new ZjValue(o->graphic->size->x));
		jsonObject->keys.push_back ("sgy"); jsonObject->values.push_back (new ZjValue(o->graphic->size->y));
		jsonObject->keys.push_back ("sgz"); jsonObject->values.push_back (new ZjValue(o->graphic->size->z));

		jsonObject->keys.push_back ("rz"); jsonObject->values.push_back (new ZjValue(o->rotation->z));

		jsonObject->keys.push_back ("col"); jsonObject->values.push_back (new ZjValue((float)(int)o->phys));
		jsonObject->keys.push_back ("back"); jsonObject->values.push_back (new ZjValue((float)(int)o->background));

		string path = "";

		if (o->graphic->texture != NULL)
			path = o->graphic->texture->getName();
		if (o->graphic->animation != NULL)
			path = o->graphic->animation->asset->getName();

		jsonObject->keys.push_back ("gtexture"); jsonObject->values.push_back (new ZjValue(path));

		ZjValue *object = new ZjValue(jsonObject);
		map->keys.push_back ("o"); map->values.push_back (object);

		completed ++;

		stringstream ss;
		ss << int(completed/objects->size()*100) << " %";
		bStatus->setText(ss.str());
		bStatus->setProgress(completed/objects->size());

		//SDL_Delay(100);
	}

	string mapString = jsonManager->dumpJSON(map);

	bool ok = filesystemManager->setFileData (_path, mapString.c_str(), mapString.length());

	if (ok) {
		cout << "[INF] [MAP SAVED TO] " << _path.getPath() << endl;
		stringstream ss;
		ss << "Successfully saved to " << _path.getPath();
		bStatus->setText(ss.str());
		bStatus->setProgress(1);
	} else {
		cout << "[ERR] [SAVING MAP]" << endl;
		stringstream ss;
		ss << "Error saving to " << _path.getPath();
		bStatus->setText(ss.str());
		bStatus->setProgress(0);
	}

}

void ZEngine::init () {
}

bool b2qGetFirstObject::ReportFixture (b2Fixture *_fixture) {
    auto keyboard = Engine->inputManager->getKeyboards()[0];

    if ((object == NULL || object->position->z < ((ZObject*)_fixture->GetBody()->GetUserData())->position->z) &&
        (!((ZObject*)_fixture->GetBody()->GetUserData())->background || keyboard->keys[SDLK_b]))
		object = (ZObject*)_fixture->GetBody()->GetUserData();
	return true;
}

bool b2ContactFilter::ShouldCollide(b2Fixture* fixtureA, b2Fixture* fixtureB) {
	const b2Filter& filterA = fixtureA->GetFilterData();
	const b2Filter& filterB = fixtureB->GetFilterData();

	if (filterA.groupIndex == 2 && filterB.groupIndex == 3) {
		ZPlayerObject *player = (ZPlayerObject*)fixtureB->GetBody()->GetUserData();
		player->position->x = rand()%300-150;
		player->position->y = rand()%300-150;

		player->reinit = true;

		((ZBulletObject*)fixtureB->GetBody()->GetUserData())->suicide = true;
	}
	else
	if (filterA.groupIndex == 3 && filterB.groupIndex == 2) {
		ZPlayerObject *player = (ZPlayerObject*)fixtureA->GetBody()->GetUserData();
		player->position->x = rand()%300-150;
		player->position->y = rand()%300-150;

		player->reinit = true;

		((ZBulletObject*)fixtureB->GetBody()->GetUserData())->suicide = true;
	}

	if (filterA.groupIndex == 0 || filterB.groupIndex == 0 ||
		filterA.groupIndex == 2 || filterB.groupIndex == 2)
		return false;
	return true;
}

ZEngine* Engine;
