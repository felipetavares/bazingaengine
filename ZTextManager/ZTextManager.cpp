#include "ZTextManager.h"
#include "../ZEngine/ZEngine.h"
#include <SDL/SDL.h>

ZUTF8Atlas::GlyphPosition::GlyphPosition () {
    p = 0;
    s = 0;
}

ZUTF8Atlas::GlyphPosition::GlyphPosition (int _p, int _s) {
    p = _p;
    s = _s;
}

ZUTF8Atlas::ZUTF8Atlas(ZFontAsset& _font):
    font(_font) {
    texture = NULL;
}

ZUTF8Atlas::~ZUTF8Atlas () {
    if (texture) {
        delete texture;
    }
}

int ZUTF8Atlas::drawCharacter (uint16_t _character) {
    GlyphPosition pos;

    try {
        pos = positions.at(_character);
    } catch (out_of_range out) {
        characters.push_back(_character);
        update();
        pos = positions.at(_character);
    }

    glPushMatrix ();
		glBindTexture (GL_TEXTURE_2D, texture->id);

		float pX = (float)pos.p/(float)texture->rwidth;
		float pY = (float)0/(float)texture->rwidth;
		float sX = (float)pos.s/(float)texture->rwidth;
		float sY = (float)texture->height/(float)texture->rheight;

        glEnable (GL_ALPHA_TEST);

		glBegin(GL_QUADS);
			glTexCoord2f (pX,pY); 		glVertex3f(0, 0, 0);
			glTexCoord2f (pX+sX,pY); 	glVertex3f(pos.s, 0, 0);
			glTexCoord2f (pX+sX,pY+sY); glVertex3f(pos.s, texture->height, 0);
			glTexCoord2f (pX,pY+sY); 	glVertex3f(0, texture->height, 0);
		glEnd();
	glPopMatrix ();

    return pos.s;
}

int ZUTF8Atlas::characterSize(uint16_t _character) {
    GlyphPosition pos;

    try {
        pos = positions.at(_character);
    } catch (out_of_range out) {
        characters.push_back(_character);
        update();
        pos = positions.at(_character);
    }

    return pos.s;
}

void ZUTF8Atlas::update () {
    if (characters.size() == 0)
        return;

    string text;
    int textW,textH,oldTextW=0;

    // Mede o texto (suporta utf8)
    positions.clear();
    for (uint16_t character :characters) {
        // Adiciona um novo caractere à string que vai ser renderizada
        text += (character&0xFF);

        if ((character&0xFF) > 192)
            text += (character>>8);
        // Mede o tamanho da string
        TTF_SizeUTF8 (font.font, text.c_str(), &textW, &textH);
        // Adiciona o tamanho ao mapa
        positions.insert (pair<uint16_t,GlyphPosition>(character,GlyphPosition(oldTextW,textW-oldTextW)));
        oldTextW = textW;
    }

    // Renderiza o texto
    render(text);
}

void ZUTF8Atlas::render (string& _text) {
    if (texture)
        delete texture;
    texture = new ZTextureAsset(0, string(""));

	SDL_Surface *renderedText;

	SDL_Color textColor = {255,255,255}; // Branco

	renderedText = TTF_RenderUTF8_Blended (font.font, _text.c_str(), textColor);

	if (!renderedText) {
		cout << "[INF]" << " [FAILED TO RENDER TEXT]" << endl;
        return;
	}

    // Calcula os tamanhos para uma textura POT (Power Of Two) equivalente
    int potW = pow(2,ceil(log2(renderedText->w)));
    int potH = pow(2,ceil(log2(renderedText->h)));

    SDL_Surface *potSurface = SDL_CreateRGBSurface (renderedText->flags, potW, potH,
                                        renderedText->format->BitsPerPixel,
                                        renderedText->format->Rmask,
                                        renderedText->format->Gmask,
                                        renderedText->format->Bmask,
                                        renderedText->format->Amask);

    if (!potSurface) {
        cout << "[ERR] Creating [^2] renderedText" << endl;
    }

    SDL_SetAlpha(renderedText, 0, renderedText->format->alpha);
    //SDL_SetAlpha(potSurface, 0, potSurface->format->alpha);

    if (SDL_BlitSurface (renderedText, NULL, potSurface, NULL) == 0) {
        cout << "[INF] [CONV] [^2] {OK}" << endl;
    } else {
        cout << "[INF] [CONV] [^2] {FAIL}" << endl;
    }

    SDL_SaveBMP (potSurface, "debug.bmp");

	GLuint id;

	glGenTextures (1, &id);
	glBindTexture (GL_TEXTURE_2D, id);

	int imgMode = GL_BGR;
	if (renderedText->format->BytesPerPixel == 4)
		imgMode = GL_BGRA;

	glTexImage2D(GL_TEXTURE_2D, 0, potSurface->format->BytesPerPixel, potSurface->w, potSurface->h,
								0, imgMode, GL_UNSIGNED_BYTE,
								potSurface->pixels);

	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

	texture->id = id;
	texture->width = renderedText->w;
	texture->height = renderedText->h;
	texture->rwidth = potSurface->w;
	texture->rheight = potSurface->h;
	texture->loaded = true;

	SDL_FreeSurface (renderedText);
	SDL_FreeSurface (potSurface);
}

//! Text manager
ZTextManager::ZTextManager () {
	defaultFont = NULL;
	color.x = color.y = color.z = 0;
	createColorShader();
}

ZTextManager::~ZTextManager () {
	if (utf8atlas != NULL)
		delete utf8atlas;
}

void ZTextManager::initTextureMaps (ZFontAsset *_font) {
	defaultFont = _font;
    utf8atlas = new ZUTF8Atlas(*defaultFont);
 }

void ZTextManager::drawString (Vec3 _pos, string& _text, float _h) {
   float scale = (float)_h/32.0;

    glPushMatrix();
        glColor3f (color.x,color.y,color.z);
        glTranslatef (_pos.x, _pos.y, _pos.z);
        glScalef(scale,scale,scale);
        for (int i=0;i<_text.size();i++) {
            uint16_t utf8;
            if (_text[i] > 192)
                utf8 = (uint16_t)_text[i] | (((uint16_t)_text[i+=1])<<8);
            else
                utf8 = _text[i];
            int advance = utf8atlas->drawCharacter(utf8);
            glTranslatef(advance,0,0);
        }
    glPopMatrix();
}

float ZTextManager::measureString (string& _text, float _h) {
	float len=0;
    float scale = (float)_h/32.0;

    for (int i=0;i<_text.size();i++) {
        uint16_t utf8;
        if ((uint8_t)_text[i] > 192)
            utf8 = (uint16_t)_text[i] | (((uint16_t)_text[i+=1])<<8);
        else
            utf8 = _text[i];
        len += utf8atlas->characterSize(utf8);
    }

    return len;
}

void ZTextManager::drawStringCentered (Vec3 _pos, string& _text, float _h) {
    uint8_t *text = (uint8_t*) _text.c_str();

    float scale = (float)_h/32.0;

    Vec2 size = Vec2(measureString(_text,_h),32.0);

    glPushMatrix();
        glColor3f (color.x,color.y,color.z);
        glTranslatef (_pos.x, _pos.y, _pos.z);
        glScalef(scale,scale,scale);
        glTranslatef (-size.x/2,-size.y/2,0);
        for (int i=0;i<strlen((char*)text);i++) {
            uint16_t utf8;
            if (text[i] > 192)
                utf8 = (uint16_t)text[i] | (((uint16_t)text[i+=1])<<8);
            else
                utf8 = text[i];
            int advance = utf8atlas->drawCharacter(utf8);
            glTranslatef(advance,0,0);
        }
    glPopMatrix();
}

void ZTextManager::setColor (Vec3 _color) {
	color = _color;
}

void ZTextManager::createColorShader () {
    /*
	string vShaderCode =
	"void main() { \
	gl_Position = gl_ModelViewProjectionMatrix * glVertex; \
	}";

	string fShaderCode =
	"void main() { \
	gl_FragColor = vec4(0.0,1.0,0.0,1.0); \
	}";

	GLuint vShader, fShader;

	vShader = glCreateShader (GL_VERTEX_SHADER);
	fShader = glCreateShader (GL_FRAGMENT_SHADER);

	glShaderSourceARB(fShader, 1, fShaderCode.c_str(), fShaderCode.size());
	glShaderSourceARB(vShader, 1, fShaderCode.c_str(), vShaderCode.size());

	glCompileShaderARB(vShader);
	glCompileShaderARB(fShader);


	GLint compiled;

	glGetObjectParameteriv(fShader, GL_COMPILE_STATUS, &compiled);
	if (compiled) {
		cout << "[INF] [TXT] [FRAGSHADER] {OK}"
	} else {
		cout << "[INF] [TXT] [FRAGSHADER] {FAIL}"
	}

	glGetObjectParameteriv(vShader, GL_COMPILE_STATUS, &compiled);
	if (compiled) {
		cout << "[INF] [TXT] [VERTSHADER] {OK}"
	} else {
		cout << "[INF] [TXT] [VERTSHADER] {FAIL}"
	}

    */
}
