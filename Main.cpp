#include <iostream>
using namespace std;

#include "ZEngine/ZEngine.h"
#include "ZObject/Objects/ZPlayerObject.h"
#include <stdlib.h>
#include <sstream>

#define ASSETLIST_PATH ".:Assets:list.json"

void createGUI () {
	auto menu = new ZUI::Panel (vec2(10,10),vec2(400,300), 24);
	auto objects = new ZUI::Panel (vec2(ww-410,10),vec2(400,300), 24);
	auto button0 = new ZUI::wButton("Quit");
	auto button1 = new ZUI::wButton("Objects");
	auto button2 = new ZUI::wButton("Save map");
	auto button3 = new ZUI::wButton("Edit Mode");
	auto button4 = new ZUI::wButton("Browse files");
	auto entry = new ZUI::wEntry("ÁÉÍÓÚáéíóú");

	button0->addAction (
		new ZUI::Filter (button0, Engine->guiManager->onclick),
		new ZUI::Action ([] (ZUI::Widget* _w, ZUI::Event *_m) {
			Engine->quit();
		})
	);

	button1->addAction (
		new ZUI::Filter (button1, Engine->guiManager->onclick),
		new ZUI::Action ([=] (ZUI::Widget* _w, ZUI::Event *_m) {
			objects->setVisibility(!objects->isVisible());
		})
	);

	button2->addAction (
		new ZUI::Filter (button2, Engine->guiManager->onclick),
		new ZUI::Action ([=] (ZUI::Widget* _w, ZUI::Event *_m) {
			// Para não bloquear a UI
			SDL_CreateThread([] (void *_obj) {
			ZFilePath mapPath = ZFilePath (".:Assets:map00.map");
			    Engine->saveMap(mapPath);
			    return 0;
			},NULL);
		})
	);

	button3->addAction (
		new ZUI::Filter (button3, Engine->guiManager->onclick),
		new ZUI::Action ([] (ZUI::Widget* _w, ZUI::Event *_m) {
			Engine->editMode = !Engine->editMode;
		})
	);

	button4->addAction (
		new ZUI::Filter (button4, Engine->guiManager->onclick),
		new ZUI::Action ([] (ZUI::Widget* _w, ZUI::Event *_m) {
			Engine->listDirectory (ZFilePath(".:Assets:"));
		})
	);

	entry->addAction (
		new ZUI::Filter (entry, Engine->guiManager->onclick),
		new ZUI::Action ([=] (ZUI::Widget* _w, ZUI::Event *_m) {
			Engine->guiManager->requestFocus(menu, _w);
		})
	);

	entry->addAction (
		new ZUI::Filter (entry, Engine->guiManager->keydown),
		new ZUI::Action ([] (ZUI::Widget* _w, ZUI::Event *_k) {
			((ZUI::wEntry*)_w)->processKeyEvent(_k);
		})
	);

	menu->addWidget(button0);
	menu->addWidget(button1);
	menu->addWidget(button2);
	menu->addWidget(button3);
	menu->addWidget(button4);
	//menu->addWidget(entry);
	menu->setVisibility(true);

	unsigned int tAid=0;
	unsigned int tnAid=0;
	while (tnAid >= tAid) {
		tAid = tnAid;
		tnAid = Engine->editor->findNextTextureAsset (tAid);

		auto icon = new ZUI::wIcon(tnAid);
		icon->addAction (
			new ZUI::Filter (icon, Engine->guiManager->onclick),
			new ZUI::Action ([] (ZUI::Widget* _w, ZUI::Event *_m) {
				Engine->editor->createObject(((ZUI::wIcon*)_w)->getAid());
			})
		);
		objects->addWidget(icon);
	}

	Engine->guiManager->addPanel(objects);
	Engine->guiManager->addPanel(menu);

}

bool GUI;

bool loadAssets () {
	ZjObject *jsonAssetList;
	char *file;

	// Load file
	if (Engine->filesystemManager->fileExists(ZFilePath(ASSETLIST_PATH))) {
		file = Engine->filesystemManager->getFileData(ZFilePath(ASSETLIST_PATH));
		file [Engine->filesystemManager->getFileSize(ZFilePath(ASSETLIST_PATH))] = 0;
		string jsonStringAssetList = string (file);

		jsonAssetList = Engine->jsonManager->parse (jsonStringAssetList);

		for (int i=0;i<jsonAssetList->keys.size();i++) {
			if (jsonAssetList->keys[i] == "editmode" &&
			jsonAssetList->values[i]->number != 0) {
				GUI = true;
				SDL_ShowCursor (1);
			} else {
				cout << "[INF] (AL: " << ASSETLIST_PATH
				     << ") " <<
				jsonAssetList->keys[i] << " " <<
				jsonAssetList->values[i]->str << endl;
				long int assetId = Engine->assetsManager->createAsset (jsonAssetList->values[i]->str,jsonAssetList->keys[i]);
				Engine->assetsManager->needAsset (assetId);
			}
		}

		delete jsonAssetList;
		return true;
	} else {
        	cout << "[ERR] No AL file found!" << endl;
		return false;
	}
}


int main (int,char**) {
	Engine = new ZEngine(false); // Parameter=debug
	Engine->init();

	// True is for debug mode, defalts to false
	Engine->videoManager->setWindowTitleAndIcon ("Bazinga Engine Demo","Bazinga Engine");

	// Load all assets
	if (!loadAssets()) {
		delete Engine;
		return 0;
	}

	if (GUI)
        	createGUI();

	Engine->box2dWorld->SetGravity (b2vec2 (0,0));
	//Engine->box2dWorld->SetLinearDamping (0.5);

	Engine->textManager->initTextureMaps (Engine->assetsManager->getAsset <ZFontAsset*> ("font.default"));

	ZPlayerObject* object = (ZPlayerObject*)Engine->createObject <ZPlayerObject> ();

	object->position->x = 0;
	object->position->y = 0;
	object->position->z = 0;
	object->graphic->position->x = -8;
	object->graphic->position->y = -42;
	object->graphic->position->z = 0;

	object->size->x = 8;
	object->size->y = 4;
	object->size->z = 0;
	object->graphic->size->x = 24;
	object->graphic->size->y = 52;
	object->graphic->size->z = 0;

	object->phys = true;

	Engine->addObject (object);
	Engine->camera->playerOid = object->oid;

	// Pre-render every character (It is faster to render this way than using drawString)
	// Draw string re-renders for every characters, here we render the entire chunk
	string everything = "abcdefghijklmnopqrstyuvwxyzáéíóúãõÁÉÍÓÚâêôÂÊÔABCDEFGHIJKLMNOPQRSTUVWXYZ0123456789.,!?@#$&*()-+/:;\"'<>[]{}%/\\ çÇ";
	Engine->textManager->registerCharacters(everything);

	auto introScene = new scnIntro ("IntroFast.scene");

	Engine->sceneManager->addScene (introScene);

	try {
		Engine->run();
	} catch (exception e) {
		cout << "[ERR] [EXCEPT] " << e.what() << endl;
	}

	delete Engine;

	return 0;
}
