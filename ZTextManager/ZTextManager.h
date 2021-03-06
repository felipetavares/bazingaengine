#ifndef TEXT_MANAGER_H
#define TEXT_MANAGER_H

#include <map>
#include <stdexcept>
#include <SDL/SDL_opengl.h>
#include "../ZAssetsManager/ZAssetsManager.h"
#include "ZMath/math/vec3/vec3.h"
#include "ZMath/math/vec2/vec2.h"

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
    void registerCharacters (vector <uint16_t>);
private:
    void update ();
    void render (string&);
};

class ZTextManager {
	ZFontAsset *defaultFont;
    ZUTF8Atlas *utf8atlas;
	vec3 color;
	float opacity;
public:
	ZTextManager();
	~ZTextManager();

	void setColor (vec3);

    float measureString (string&, float);
	void drawString (vec3,string&, float);
	void drawStringCentered (vec3, string&, float);

	void initTextureMaps (ZFontAsset*);
    void setOpacity(float);
    void registerCharacters (string&);
private:
	void createColorShader();
};

#endif /* TEXT_MANAGER_H */
