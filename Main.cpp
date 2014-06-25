#include <iostream>
using namespace std;

#include "ZEngine/ZEngine.h"

#include <stdlib.h>
#include <sstream>

#define ASSETLIST_PATH ".:Assets:list.json"

void loadAssets () {
	ZjObject *jsonAssetList;
	char *file;

    // Load file
	if (Engine->filesystemManager->fileExists(ZFilePath(ASSETLIST_PATH))) {
        file = Engine->filesystemManager->getFileData(ZFilePath(ASSETLIST_PATH));
        file [Engine->filesystemManager->getFileSize(ZFilePath(ASSETLIST_PATH))] = 0;
        string jsonStringAssetList = string (file);

        jsonAssetList = Engine->jsonManager->parse (jsonStringAssetList);

        for (int i=0;i<jsonAssetList->keys.size();i++) {
            cout << "[INF] (AL: " << ASSETLIST_PATH << ") " << jsonAssetList->keys[i] << " " << jsonAssetList->values[i]->str << endl;
            long int assetId = Engine->assetsManager->createAsset (jsonAssetList->values[i]->str);
            Engine->assetsManager->needAsset (assetId);
        }

        delete jsonAssetList;
	} else {
        cout << "[ERR] No AL file found!" << endl;
    }
}

int main (int,char**) {
	ZFilePath mapPath = ZFilePath (".:Assets:map.map");

	Engine = new ZEngine(false); // Parameter=debug
	Engine->init();

	// True is for debug mode, defalts to false
	Engine->videoManager->setWindowTitleAndIcon ("Bazinga Engine Demo","Bazinga Engine");

    // Load all assets
    loadAssets();

	Engine->loadMap (mapPath);

	Engine->box2dWorld->SetGravity (b2Vec2 (0,0));
	//Engine->box2dWorld->SetLinearDamping (0.5);

	Engine->assetsManager->getAsset <ZAudioAsset*>(ZFilePath(".:Assets:Audio:wind:wind.ogg"))->loop();
	Engine->textManager->initTextureMaps (Engine->assetsManager->getAsset <ZFontAsset*> (ZFilePath(".:Assets:Fonts:LinuxLibertine:LinLibertine_Mah.ttf")));

	ZPlayerObject* object = (ZPlayerObject*)Engine->createObject <ZPlayerObject> ();

	object->position->x = 0;
	object->position->y = 0;
	object->position->z = 0;
	object->graphic->position->x = -10;
	object->graphic->position->y = -22;
	object->graphic->position->z = 0;

	object->size->x = 8;
	object->size->y = 4;
	object->size->z = 0;
	object->graphic->size->x = 26;
	object->graphic->size->y = 26;
	object->graphic->size->z = 0;

	object->phys = true;

	Engine->addObject (object);
	Engine->camera->playerOid = object->oid;

	auto menu = new ZUI::Panel (Vec2(10,10),Vec2(400,300), 24);
	auto objects = new ZUI::Panel (Vec2(ww-410,10),Vec2(400,300), 24);
	auto button0 = new ZUI::wButton("Quit");
	auto button1 = new ZUI::wButton("Objects");
	auto button2 = new ZUI::wButton("Save map");
	auto button3 = new ZUI::wButton("Edit Mode");
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
                ZFilePath mapPath = ZFilePath (".:Assets:map.map");
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

	Engine->run();

	delete Engine;

	return 0;
}
