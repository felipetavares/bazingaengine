#include "ZScene.h"
#include "../ZEngine/ZEngine.h"

ZScene::ZScene (string _name) {
	loadData(_name);
}

void ZScene::loadData (string _name) {
	long int jsonAsset = Engine->assetsManager->createAsset (ZFilePath(string(SCENE_BASE)+_name), "scene."+_name);
	Engine->assetsManager->needAsset (jsonAsset);
	data = Engine->assetsManager->getAsset<ZJSONAsset*> (jsonAsset);
}

ZSceneManager::ZSceneManager () {
	currentScene = NULL;
	nextScene = NULL;
	fadeOut = false;
}

ZSceneManager::~ZSceneManager () {
	for (auto scn :scenes) {
		delete scn;
	}
}

void ZSceneManager::addScene (ZScene *_scene) {
	scenes.push_back (_scene);
	setCurrentScene (_scene);
}

void ZSceneManager::addSceneFadeOut (ZScene *_scene, float _fadeTime) {
	scenes.push_back (_scene);
	nextScene = _scene;
	changeTime = Engine->gameTime->currentTime+_fadeTime;
	startTime = Engine->gameTime->currentTime;
}

void ZSceneManager::setCurrentScene (ZScene *_scene) {
	currentScene = _scene;
	currentScene->load();
}

void ZSceneManager::render () {
	if (nextScene) {
		float color = (Engine->gameTime->currentTime-startTime)/(changeTime-startTime);
		Engine->colorMultiplier = 1-color;
	}

	if (fadeOut) {
		float color = (Engine->gameTime->currentTime-startTime)/(changeTime-startTime);
		color = color;
		Engine->colorMultiplier = color;
	}

	float bc = Engine->colorMultiplier;
	glClearColor(bc,bc,bc,1);
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	glLoadIdentity();

	if (nextScene && Engine->gameTime->currentTime >= changeTime) {
		currentScene = nextScene;
		currentScene->load();
		nextScene = NULL;
		fadeOut = true;
		changeTime = Engine->gameTime->currentTime+8;
		startTime = Engine->gameTime->currentTime;
	}

	if (fadeOut && Engine->gameTime->currentTime >= changeTime) {
		fadeOut = false;
	}

	if (currentScene) {
		currentScene->render();
	} else {
		// Draws a default scene
	}
}

scnIntro::scnIntro (string _name):
ZScene(_name) {
}

void scnIntro::load () {
	currentText = 0;
	currentImage = 0;
	list = data->getJSON()->get("text")->array->array;
	time = data->getJSON()->get("time")->array->array;
	startTime = Engine->gameTime->currentTime;
	changeTime = Engine->gameTime->currentTime+time[0]->number;
}

void scnIntro::render () {
    // For debugging purposes ONLY
    //auto gameScene = new scnGame ("Game.scene");
    //Engine->sceneManager->addScene(gameScene);
    // END OF DEBUG CODE

	static bool end = false;

    //int w = Engine->videoManager->windowWidth;
    //int h = Engine->videoManager->windowHeight;

	if (Engine->gameTime->currentTime >= changeTime) {
		if (currentText < list.size()-1) {
			string text = list[currentText]->str;

			if (text.size() > 0 &&
				text[0] == '#') {
				currentImage++;
		}
		currentText++;
	} else {
		auto gameScene = new scnGame ("Game.scene");
		Engine->sceneManager->addSceneFadeOut(gameScene,5);
		end = true;
	}

	if (currentText == 2) {
		Engine->assetsManager->getAsset <ZAudioAsset*>("audio.music.intro2")->loop();
	}

	if (currentText == 6) {
		Engine->assetsManager->getAsset <ZAudioAsset*>("audio.effect.wind")->loop();
	}

	startTime = Engine->gameTime->currentTime;
	changeTime = Engine->gameTime->currentTime+time[currentText]->number;
}

if (!end) {
	float color = (Engine->gameTime->currentTime-startTime)/(changeTime-startTime);
	color = (1-(-cos(color*M_PI*2)+1)/2);
	color = color * color * color;

	float py = 0;

	string text = list[currentText]->str;

	if (text.size() > 0 &&
		text[0] == '#') {
		string texPath = data->getJSON()->get("image")->array->array[currentImage]->str;
	auto tex = Engine->assetsManager->getAsset <ZTextureAsset*>(texPath);
	py += tex->height+10;
	tex->setColor (vec3(0.25,0.25,0.25));
	tex->setOpacity (1.0-color);

	glPushMatrix();
	glScalef (2,2,2);
	tex->drawCentered(vec3(0,0,0));
	glPopMatrix();

	text = text.substr(1);
}

	//stringstream pc;
	//pc << color;
	//string percent = pc.str();

Engine->textManager->setOpacity (1);
Engine->textManager->setColor(vec3(color,color,color));
Engine->textManager->drawStringCentered(vec3(0,py,0), text, 32);
        //Engine->textManager->setColor(vec3(0,0,0));
        //Engine->textManager->drawStringCentered(vec3(0,py+32,0), percent, 16);
}
}

scnGame::scnGame (string _name):
ZScene(_name) {
}

void scnGame::load () {
	ZFilePath mapPath = ZFilePath (data->getJSON()->get("map")->str);
	Engine->loadMap (mapPath);
}

void scnGame::render () {
	long int i;

	float gloom = 1;

	//glClearColor(Engine->colorMultiplier,Engine->colorMultiplier,Engine->colorMultiplier,1);
	glClearColor (0,0,0,1);
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	glLoadIdentity();

	if (Engine->editMode) {
		Engine->editor->drawHUD();
	}

	Engine->guiManager->render();

	ZCamera *camera = Engine->camera;
	camera->run();

	vec3 mp;

	mp.x = camera->position->x-Engine->videoManager->windowWidth/2/camera->scale->x+Engine->inputManager->getMice()[0]->pos.x/camera->scale->x;
	mp.y = camera->position->y-Engine->videoManager->windowHeight/2/camera->scale->y+Engine->inputManager->getMice()[0]->pos.y/camera->scale->y;

	glScalef (camera->scale->x,camera->scale->y,camera->scale->z);

	if (Engine->editor->grid && Engine->editMode) {
		float gX,gY;
		gX = Engine->editor->gridSize->x;
		gY = Engine->editor->gridSize->y;

		float startX 	= ((int)-camera->position->x)%(int)(gX) - gX;
		float endX 		= startX + Engine->videoManager->windowWidth/camera->scale->x;
		float startY	= ((int)-camera->position->y)%(int)(gY) - gY;
		float endY		= startY + Engine->videoManager->windowHeight/camera->scale->y;

		startX  -= (int)(Engine->videoManager->windowWidth/2/camera->scale->x)/(int)gX  *(int)gX;
		startY  -= (int)(Engine->videoManager->windowHeight/2/camera->scale->y)/(int)gY *(int)gY;

		for (i=startX;i<=endX;i+=gX) {
			glDisable (GL_TEXTURE_2D);
			glColor3f (Engine->editor->gridColor->x, Engine->editor->gridColor->y, Engine->editor->gridColor->z);

			glBegin(GL_LINE_LOOP);
			glVertex3f(i,-Engine->videoManager->windowHeight/2/camera->scale->y,0.9);
			glVertex3f(i,Engine->videoManager->windowHeight/2/camera->scale->y,0.9);
			glEnd();

			glEnable (GL_TEXTURE_2D);
		}

		for (i=startY;i<=endY;i+=gY) {
			glDisable (GL_TEXTURE_2D);
			glColor3f (Engine->editor->gridColor->x, Engine->editor->gridColor->y, Engine->editor->gridColor->z);

			glBegin(GL_LINE_LOOP);
			glVertex3f(-Engine->videoManager->windowWidth/2/camera->scale->x,i,0.9);
			glVertex3f(Engine->videoManager->windowWidth/2/camera->scale->x,i,0.9);
			glEnd();

			glEnable (GL_TEXTURE_2D);
		}
	}

	glTranslatef (-camera->position->x,-camera->position->y,-camera->position->z);

	long int len=Engine->getObjectsNumber();
	for (long int i=0;i<len;i++) {

		ZObject* obj = Engine->getObjectByPos(i);
		if (obj == NULL)
			continue;


		float objz;
		if (!obj->background)
			objz = ((obj->position->y+obj->size->y/2))/1000.0+10.0;
		else
			objz = obj->graphic->position->z;

		obj->position->z = objz;

		glPushMatrix();
		glTranslatef(obj->position->x,obj->position->y,objz);
		glRotatef	(obj->rotation->x, 1, 0 ,0);
		glRotatef	(obj->rotation->y, 0, 1 ,0);
		glRotatef	(obj->rotation->z, 0, 0 ,1);
		glTranslatef (-obj->size->x/2,-obj->size->y/2,0);

		if (Engine->editMode) {
			glDisable (GL_TEXTURE_2D);
			glColor3f (0,1,0);
			glBegin(GL_LINE_LOOP);
			glVertex3f(0, 0, 50);
			glVertex3f(obj->size->x, 0, 50);
			glVertex3f(obj->size->x, obj->size->y, 50);
			glVertex3f(0, obj->size->y, 50);
			glEnd();
			glEnable (GL_TEXTURE_2D);
		}

		if (gloom > 1)
			gloom = 1;
		if (gloom < -1)
			gloom = -1;

		if (Engine->editMode)
			glColor3f(0.25,0.25,0.25);
		else {
			if (gloom < 0.5)
				glColor3f(0.5*gloom*Engine->colorMultiplier,0.5*gloom*Engine->colorMultiplier,1.4*gloom*Engine->colorMultiplier);
			else
				glColor3f(0.35*gloom*Engine->colorMultiplier,0.30*gloom*Engine->colorMultiplier,0.30*gloom*Engine->colorMultiplier);
		}

		glPushMatrix();
		glRotatef	(obj->graphic->rotation->x, 1, 0 ,0);
		glRotatef	(obj->graphic->rotation->y, 0, 1 ,0);
		glRotatef	(obj->graphic->rotation->z, 0, 0 ,1);
		glTranslatef(obj->graphic->position->x,obj->graphic->position->y,0);

		if (obj->graphic->texture != NULL && obj->graphic->texture->loaded) {
			glEnable(GL_ALPHA_TEST);
			glBindTexture (GL_TEXTURE_2D, obj->graphic->texture->id);
			glBegin(GL_QUADS);
			glTexCoord2f (0,0); glVertex3f(0, 0, 0);
			glTexCoord2f (obj->graphic->size->x/obj->graphic->texture->rwidth,0); glVertex3f(obj->graphic->size->x, 0, 0);
			glTexCoord2f (obj->graphic->size->x/obj->graphic->texture->rwidth,obj->graphic->size->y/obj->graphic->texture->rheight); glVertex3f(obj->graphic->size->x, obj->graphic->size->y, 0);
			glTexCoord2f (0,obj->graphic->size->y/obj->graphic->texture->rheight); glVertex3f(0, obj->graphic->size->y, 0);
			glEnd();

			if (!obj->background) {
				glPushMatrix();
				glColor3f(0,0,0);
				glTranslatef (0, obj->graphic->size->y-obj->size->y, -objz+1);
				glScalef (1, (obj->size->y-obj->graphic->position->y)/obj->graphic->size->y*cos(gloom), 1);
				glBegin(GL_QUADS);
				glTexCoord2f (0,0);
				glVertex3f(0, obj->graphic->size->y, 0);

				glTexCoord2f (obj->graphic->size->x/obj->graphic->texture->rwidth,0);
				glVertex3f(obj->graphic->size->x, obj->graphic->size->y, 0);

				glTexCoord2f (obj->graphic->size->x/obj->graphic->texture->rwidth,obj->graphic->size->y/obj->graphic->texture->rheight);
				glVertex3f(obj->graphic->size->x, 0, 0);

				glTexCoord2f (0,obj->graphic->size->y/obj->graphic->texture->rheight);
				glVertex3f(0, 0, 0);
				glEnd();
				glPopMatrix();
			}
		}
		else {
			if (obj->graphic->animation != NULL && obj->graphic->animation->asset->loaded) {
				obj->graphic->animation->run();
				obj->graphic->animation->draw();
				glPushMatrix();
				glColor3f(0,0,0);
				glTranslatef (0, obj->graphic->size->y-obj->size->y/2, -objz+1);
				glScalef (1, (obj->size->y-obj->graphic->position->y)/obj->graphic->size->y*cos(gloom), 1);
				obj->graphic->animation->drawShadow(gloom);
				glPopMatrix();
			}
		}
		glPopMatrix();
		glPopMatrix();
	}

	for (long int i=0;i<len;i++) {
		ZObject* obj = Engine->getObjectByPos(i);
		if (obj == NULL)
			continue;

		obj->draw();
	}
}
