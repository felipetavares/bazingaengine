#include "ZAnimation.h"
#include "../ZEngine/ZEngine.h"

ZAnimation::ZAnimation (ZAnimationAsset *_asset):
    asset(_asset) {
	nextSwitchFrameTime = 0;
	prevSwitchFrameTime = 0;
	position = 0;

    offsetX = 0;
    offsetY = 0;

	flipV = false;
	flipH = false;

    isPlaying = true;

    callback = NULL;
}

void ZAnimation::setCallback(function <void (ZAnimation*)> _callback) {
    callback = _callback;
}

void ZAnimation::run () {
	if (isPlaying) {
		if (Engine->gameTime->currentTime >= nextSwitchFrameTime ||
			Engine->gameTime->currentTime < prevSwitchFrameTime) {

			prevSwitchFrameTime = nextSwitchFrameTime;
			nextSwitchFrameTime = Engine->gameTime->currentTime+asset->frameDuration;

			if (asset->direction == ZAnimationAsset::PlayDirection::Forward)
				nextFrame();
			else if (asset->direction == ZAnimationAsset::PlayDirection::Backward)
				prevFrame();

			if (asset->sheetType == ZAnimationAsset::SheetType::Horizontal)
				offsetX = position*asset->width;
			else if (asset->sheetType == ZAnimationAsset::SheetType::Vertical)
				offsetY = position*asset->height;

            if (callback)
                callback(this);
		}
	}
}

void ZAnimation::rewind () {
	if (asset->direction == ZAnimationAsset::PlayDirection::Forward)
		position = 0;
	else if (asset->direction == ZAnimationAsset::PlayDirection::Backward)
		position = asset->frames-1;
}

void ZAnimation::nextFrame () {
	position ++;

	if (position > asset->frames-1)
		if (asset->loop)
			position = 0;
		else {
			position = asset->frames-1; // Last frame
			isPlaying = false;   // Stop playing
		}

	if (position == asset->callback.frame)
		asset->callback.call();
}

void ZAnimation::prevFrame () {
	position --;

	if (position < 0)
		if (asset->loop)
			position = asset->frames-1;
		else {
			position = 0; 		 // Last frame
			isPlaying = false;   // Stop playing
		}

	if (position == asset->callback.frame)
		asset->callback.call();
}

void ZAnimation::draw () {
	if (!asset->loaded)
		return;

	float oX = float(offsetX)/(float)asset->rwidth;
	float oY = float(offsetY)/(float)asset->rheight;
	float sX = float(asset->width)/(float)asset->rwidth;
	float sY = float(asset->height)/(float)asset->rheight;

	float x=0,y=0;

    glPushMatrix();
        if (flipH) {
            glTranslatef (asset->width,0,0);
            glScalef (-1,1,1);
        }
        if (flipV) {
            glScalef (1,-1,1);
        }

        glEnable (GL_TEXTURE_2D);
        glBindTexture (GL_TEXTURE_2D, asset->id);

        glBegin(GL_QUADS);
            glTexCoord2f (oX,oY); 		glVertex3f(x, y, 0);
            glTexCoord2f (oX+sX,oY); 	glVertex3f(asset->width, y, 0);
            glTexCoord2f (oX+sX,oY+sY); glVertex3f(asset->width, asset->height, 0);
            glTexCoord2f (oX,oY+sY); 	glVertex3f(x, asset->height, 0);
        glEnd();
    glPopMatrix();
}

void ZAnimation::drawShadow (float _coef) {
	if (!asset->loaded)
		return;

	float oX = float(offsetX)/(float)asset->rwidth;
	float oY = float(offsetY)/(float)asset->rheight;
	float sX = float(asset->width)/(float)asset->rwidth;
	float sY = float(asset->height)/(float)asset->rheight;

	float x=0,y=0;

    glPushMatrix();
        if (flipH) {
            glTranslatef (asset->width,0,0);
            glScalef (-1,1,1);
        }
        if (flipV) {
            glScalef (1,-1,1);
        }

        glEnable (GL_TEXTURE_2D);
        glBindTexture (GL_TEXTURE_2D, asset->id);

        glBegin(GL_QUADS);
            glTexCoord2f (oX,oY+sY); 		glVertex3f(x, y, 0);
            glTexCoord2f (oX+sX,oY+sY); 	glVertex3f(asset->width, y, 0);
            glTexCoord2f (oX+sX,oY); glVertex3f(asset->width, asset->height, 0);
            glTexCoord2f (oX,oY); 	glVertex3f(x, asset->height, 0);
        glEnd();
    glPopMatrix();
}
