#ifndef TEXT_MANAGER_H
#define TEXT_MANAGER_H

#include <map>
#include <SDL/SDL_opengl.h>
#include "../ZAssetsManager/ZAssetsManager.h"
#include "../ZMath/Vec3.h"
#include "../ZMath/Vec2.h"

class ZUTF8Atlas {
    class GlyphPosition {
    public:
        int p;
        int s;

        GlyphPosition();
        GlyphPosition(int,int);
    };

    vector <uint16_t> characters;
    map <uint16_t, GlyphPosition> positions;
    ZFontAsset font;
public:
    ZTextureAsset* texture;

    ZUTF8Atlas(ZFontAsset&);
    ~ZUTF8Atlas();

    int drawCharacter (uint16_t);
    int characterSize (uint16_t);
private:
    void update ();
    void render (string&);

};

class ZTextManager {
	ZFontAsset *defaultFont;
    ZUTF8Atlas *utf8atlas;
	Vec3 color;
public:
	ZTextManager();
	~ZTextManager();

	void setColor (Vec3);

    float measureString (string&, float);
	void drawString (Vec3,string&, float);
	void drawStringCentered (Vec3, string&, float);

	void initTextureMaps (ZFontAsset*);
private:
	void createColorShader();
};

#endif /* TEXT_MANAGER_H */
