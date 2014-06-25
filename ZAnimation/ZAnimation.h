#ifndef ZANIMATION_H
#define ZANIMATION_H

#include <functional>
#include "../ZAssetsManager/ZAssetsManager.h"

class ZAnimation {
    function <void (ZAnimation*)> callback;
public:
    ZAnimationAsset *asset;

	int position;
	float nextSwitchFrameTime; // The time in which we need to switch frames
	float prevSwitchFrameTime; // For detecting overflows

	int offsetX; // current frame x offset
	int offsetY; // current frame y offset

	bool isPlaying;

	bool flipH,flipV;

    ZAnimation (ZAnimationAsset*);

	void run (); // run the animation

	void draw ();
	void drawShadow (float);

	void rewind ();

	void setCallback(function <void (ZAnimation*)>);
private:
	void nextFrame();
	void prevFrame();
};

#endif // ZANIMATION_H
