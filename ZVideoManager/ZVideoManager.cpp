#include "ZVideoManager.h"
#include "../ZEngine/ZEngine.h"
#include <cmath>

ZVideoManager::ZVideoManager () {
	SDL_GL_SetAttribute(SDL_GL_RED_SIZE, 8);
	SDL_GL_SetAttribute(SDL_GL_GREEN_SIZE, 8);
	SDL_GL_SetAttribute(SDL_GL_BLUE_SIZE, 8);
	SDL_GL_SetAttribute(SDL_GL_ALPHA_SIZE, 8);
	SDL_GL_SetAttribute(SDL_GL_BUFFER_SIZE, 32);
	SDL_GL_SetAttribute(SDL_GL_DEPTH_SIZE, 16);
	SDL_GL_SetAttribute(SDL_GL_DOUBLEBUFFER, 1);

	anyResolution = false;
	// Initialize video modes array
	videoModes = new vector <SDL_Rect>();
	getVideoModes();

	findBestVideoMode();

	icon = ZFilePath (".:Assets:ZEngine.bmp");
	cout << "[INF] Loading icon from " << icon.getPath() << endl;
	SDL_WM_SetIcon(SDL_LoadBMP(icon.getPath().c_str()), NULL);
	screen = SDL_SetVideoMode (windowWidth,windowHeight,windowBpp,videoFlags);
	SDL_ShowCursor (1);

	cout << "[CRT] [WIN] [w:" << windowWidth << ",h:" << windowHeight << "]" << endl;

	glEnable (GL_DEPTH_TEST);

	glEnable( GL_TEXTURE_2D );
	glEnable( GL_BLEND );
	glEnable( GL_ALPHA );

	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

	glAlphaFunc(GL_GREATER,0.5f);
	glEnable(GL_ALPHA_TEST);

	glClearColor( 0.0f, 0.0f, 0.0f, 0.0f );

	glViewport( -windowWidth/2, windowWidth/2, windowHeight/2, -windowHeight/2);

	glClear( GL_COLOR_BUFFER_BIT );

	glMatrixMode( GL_PROJECTION );
	glLoadIdentity();

	glOrtho(-windowWidth/2, windowWidth/2, windowHeight/2, -windowHeight/2, -100.0f, 100.0f);

	glMatrixMode( GL_MODELVIEW );
	glLoadIdentity();

	cout << "[INF]" << " Using OpenGL version: " << glGetString(GL_VERSION) << endl;
}

ZVideoManager::~ZVideoManager () {
	delete videoModes;
}

void ZVideoManager::getVideoModes () {
	SDL_Rect **modeList;

	cout << "[INF] [QUERYING VIDEO MODES]" << endl;

	modeList = SDL_ListModes (NULL, videoFlags);

	cout << "[INF] [GOT VIDEO MODES FROM SDL]" << endl;

	if (modeList != NULL) {
		if (modeList == (SDL_Rect**)-1) {
			cout << "[INF] [ALL RESOLUTIONS AVAILABLE]" << endl;
			anyResolution = true;
		} else {
			for (int i=0;modeList[i];i++) {
				videoModes->push_back (*modeList[i]);
			}
		}
	} else {
		cout << "[ERR] [NO VIDEO MODE AVAIABLE]" << endl;
		exit (-1);
	}

	SDL_WM_GrabInput(SDL_GRAB_OFF); // Allow the WM to execute it's default keys
}

void ZVideoManager::findBestVideoMode () {
	// Bits per pixel
	windowBpp = SDL_GetVideoInfo()->vfmt->BitsPerPixel;

	// Resolution

	windowWidth = windowHeight = 0;

	if (anyResolution) { // Any resolution available
		windowWidth = 800;
		windowHeight = 600;
	} else {
		for (auto rect : (*videoModes) ) {
			if (rect.w * rect.h > windowHeight * windowWidth) {
				windowWidth = rect.w;
				windowHeight = rect.h;
			}
		}
	}
}

void ZVideoManager::setWindowTitleAndIcon (	string _title,
										string _icon) {
	SDL_WM_SetCaption (_title.c_str(),_icon.c_str());
}

void ZVideoManager::render () {
   	long int i;

   	glClearColor(1,1,1,1);
   	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    glLoadIdentity();

    float gloom = Engine->gameTime->currentTime/10;

    if (Engine->editMode) {
    	Engine->editor->drawHUD();
	}

    Engine->guiManager->render();

	ZCamera *camera = Engine->camera;
    camera->run();

	Vec3 mp;

	mp.x = camera->position->x-Engine->videoManager->windowWidth/2/camera->scale->x+Engine->inputManager->getMice()[0]->pos.x/camera->scale->x;
	mp.y = camera->position->y-Engine->videoManager->windowHeight/2/camera->scale->y+Engine->inputManager->getMice()[0]->pos.y/camera->scale->y;

	glScalef (camera->scale->x,camera->scale->y,camera->scale->z);

	if (Engine->editor->grid && Engine->editMode) {
		float gX,gY;
		gX = Engine->editor->gridSize->x;
		gY = Engine->editor->gridSize->y;

		float startX 	= ((int)-camera->position->x)%(int)(gX) - gX;
		float endX 		= startX + windowWidth/camera->scale->x;
		float startY	= ((int)-camera->position->y)%(int)(gY) - gY;
		float endY		= startY + windowHeight/camera->scale->y;

		startX  -= (int)(windowWidth/2/camera->scale->x)/(int)gX  *(int)gX;
		startY  -= (int)(windowHeight/2/camera->scale->y)/(int)gY *(int)gY;

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

	/*
	glPushMatrix ();
	    glTranslatef (mp.x,mp.y,0);
	    glBegin(GL_QUADS);
	        glTexCoord2f (0,0); glVertex3f(0, 0, 0);
	        glTexCoord2f (1,0); glVertex3f(5/camera->scale->x, 0, 0);
	        glTexCoord2f (1,1); glVertex3f(5/camera->scale->x, 5/camera->scale->y, 0);
	        glTexCoord2f (0,1); glVertex3f(0, 5/camera->scale->y, 0);
	    glEnd();
    glPopMatrix();
	*/

    long int len=Engine->getObjectsNumber();
	for (i=0;i<len;i++) {
		ZObject* obj = Engine->getObjectByPos(i);
		if (obj == NULL)
			continue;

		float objz;
		if (!obj->background)
            objz = ((obj->position->y+obj->size->y/2))/1000.0+10.0;
        else
            objz = 0;

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
                    glColor3f(0.5*gloom,0.5*gloom,1.4*gloom);
                else
                    glColor3f(0.55*gloom,0.45*gloom,0.45*gloom);
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
				else
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
				obj->draw();
			glPopMatrix();
		glPopMatrix();
	}

	/*
	ZTextureAsset *texture = Engine->assetsManager->getAsset <ZTextureAsset*> (4);

	if (Engine->realTime->currentTime < 7.00) {
		if (Engine->realTime->currentTime > 1.5)
			alpha += Engine->realTime->deltaTime;
		if (Engine->realTime->currentTime > 1.5)
			if (angle < 360)
				angle += 1000*Engine->realTime->deltaTime;
			else
				angle = 360;
	}

	if (texture->loaded) {
		glBindTexture (GL_TEXTURE_2D, texture->id);

		int w = texture->width;
		int h = texture->height;

		glTranslatef (-w/2,-h/2,0);

		glPushMatrix();
			glRotatef (angle,0,0,1);
			glScalef (s,s,s);

		    glBegin(GL_QUADS);
		    	//glColor4f(1, 1, 1, alpha);
		        glTexCoord2f (0,0); glVertex3f(x, y, 0);
		        glTexCoord2f (1,0); glVertex3f(x+w, y, 0);
		        glTexCoord2f (1,1); glVertex3f(x+w, y+h, 0);
		        glTexCoord2f (0,1); glVertex3f(x, y+h, 0);
		    glEnd();
		glPopMatrix();
 	}
	*/

 	//cout << Engine->fps->fps << "\r";

 	Engine->inputManager->clear();

    SDL_GL_SwapBuffers();
}
