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
    //if (texture) {
    //    delete texture;
    //}
}

void ZUTF8Atlas::registerCharacters (vector <uint16_t> _characters) {
    for (auto c :_characters) {
        characters.push_back(c);
    }
    update();
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

        glAlphaFunc(GL_GREATER,0.5f);
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
        positions.insert (pair<uint16_t,GlyphPosition>(character,GlyphPosition(oldTextW,textW*0.5-oldTextW)));
        oldTextW = textW*0.5;
    }

    // Renderiza o texto
    render(text);
}

Uint32 getpixel(SDL_Surface *surface, int x, int y)
{
    int bpp = surface->format->BytesPerPixel;
    /* Here p is the address to the pixel we want to retrieve */
    Uint8 *p = (Uint8 *)surface->pixels + y * surface->pitch + x * bpp;

    switch(bpp) {
    case 1:
        return *p;
        break;

    case 2:
        return *(Uint16 *)p;
        break;

    case 3:
        if(SDL_BYTEORDER == SDL_BIG_ENDIAN)
            return p[0] << 16 | p[1] << 8 | p[2];
        else
            return p[0] | p[1] << 8 | p[2] << 16;
        break;

    case 4:
        return *(Uint32 *)p;
        break;

    default:
        return 0;       /* shouldn't happen, but avoids warnings */
    }
}

void putpixel(SDL_Surface *surface, int x, int y, Uint32 pixel)
{
    int bpp = surface->format->BytesPerPixel;
    /* Here p is the address to the pixel we want to set */
    Uint8 *p = (Uint8 *)surface->pixels + y * surface->pitch + x * bpp;

    switch(bpp) {
    case 1:
        *p = pixel;
        break;

    case 2:
        *(Uint16 *)p = pixel;
        break;

    case 3:
        if(SDL_BYTEORDER == SDL_BIG_ENDIAN) {
            p[0] = (pixel >> 16) & 0xff;
            p[1] = (pixel >> 8) & 0xff;
            p[2] = pixel & 0xff;
        } else {
            p[0] = pixel & 0xff;
            p[1] = (pixel >> 8) & 0xff;
            p[2] = (pixel >> 16) & 0xff;
        }
        break;

    case 4:
        *(Uint32 *)p = pixel;
        break;
    }
}

enum Color {BLACK,WHITE};

int hline (SDL_Surface* in, int px, int py, int dx, int dy, int l, Color color) {
    if (px >= in->w ||
        py >= in->h)
            return 255*255;

    int dst = px+l;
    if (dst >= in->w)
        dst = in->w-1;

    if (px < 0)
        px = 0;
    if (py < 0)
        py = 0;

    int d=255*255;
    int by = (py-dy);

    while (px <= dst) {
        if (getpixel(in, px, py)>>24&0xFF) {
            if (color == WHITE) {
                int nd = (px-dx)*(px-dx)+by*by;
                if (nd < d)
                    d = nd;
            }
        } else {
            if (color == BLACK) {
                int nd = (px-dx)*(px-dx)+by*by;
                if (nd < d)
                    d = nd;
            }
        }
        px++;
    }

    return d;
}

int vline (SDL_Surface* in, int px, int py, int dx, int dy, int l, Color color) {
    if (px >= in->w ||
        py >= in->h)
            return 255*255;

    int dst = py+l;
    if (dst >= in->h)
        dst = in->h-1;

    if (px < 0)
        px = 0;
    if (py < 0)
        py = 0;

    int d=255*255;
    int bx = (px-dx);

    while (py <= dst) {
        if (getpixel(in, px, py)>>24&0xFF) {
            if (color == WHITE) {
                int nd = (py-dy)*(py-dy)+bx*bx;
                if (nd < d)
                    d = nd;
            }
        } else {
            if (color == BLACK) {
                int nd = (py-dy)*(py-dy)+bx*bx;
                if (nd < d)
                    d = nd;
            }
        }
        py++;
    }

    return d;
}

int distanceSquare (SDL_Surface *in, int px, int py, Color color, int l) {
    int nd;
    int d =255*255;

    nd = hline (in,px-l,py-l,px,py,l*2,color);
    if (nd < d)
       d = nd;
    nd = hline (in,px-l,py+l,px,py,l*2,color);
    if (nd < d)
       d = nd;
    nd = vline (in,px-l,py-l+1,px,py,l*2-2,color);
    if (nd < d)
       d = nd;
    nd = vline (in,px+l,py-l+1,px,py,l*2-2,color);
    if (nd < d)
       d = nd;

    return d;
}

int distanceTo (SDL_Surface *in, int px, int py, Color color, int *startl) {
    int d = 255*255;
    int l=1;

    for (;l<255;l++) {
        int nd = distanceSquare(in, px, py, color, l);
        if (nd < d) {
            return sqrt(nd);
        }
    }

    //*startl = l;

    return 0;
}

SDL_Surface* genSDF (SDL_Surface *in) {
    float sdfScale = 0.5;

    int w = in->w*sdfScale;
    int h = in->h*sdfScale;

    int potW = pow(2,ceil(log2(w)));
    int potH = pow(2,ceil(log2(h)));

    SDL_Surface *out = SDL_CreateRGBSurface (in->flags, potW, potH,
                                        in->format->BitsPerPixel,
                                        in->format->Rmask,
                                        in->format->Gmask,
                                        in->format->Bmask,
                                        in->format->Amask);

    if (!out)
        return NULL;

    //cout << "Gen SDF algorithm" << endl;

    int b=-255*255,s=255*255;
    int startl = 1;

    // Para cada pixel na imagem
    for (int y=0;y<h;y++) {
        if (y%2)
            for (int x=0;x<w;x++) {
                if (getpixel(in,x/sdfScale,y/sdfScale)>>24&0xFF) { // Pixel branco
                    int d = distanceTo(in, x/sdfScale, y/sdfScale, BLACK, &startl)*sdfScale;
                    if (d > b)
                        b = d;
                    putpixel(out, x, y, d);
                } else { // Pixel preto
                    int d = distanceTo(in, x/sdfScale, y/sdfScale, WHITE, &startl)*sdfScale;
                    if (-d < s)
                        s = -d;
                    putpixel(out, x, y, d<<16);
                }
            }
        else
            for (int x=w-1;x>=0;x--) {
                if (getpixel(in,x/sdfScale,y/sdfScale)>>24&0xFF) { // Pixel branco
                    int d = distanceTo(in, x/sdfScale, y/sdfScale, BLACK, &startl)*sdfScale;
                    if (d > b)
                        b = d;
                    putpixel(out, x, y, d);
                } else { // Pixel preto
                    int d = distanceTo(in, x/sdfScale, y/sdfScale, WHITE, &startl)*sdfScale;
                    if (-d < s)
                        s = -d;
                    putpixel(out, x, y, d<<16);
                }
            }
    }

    // Ajusta as cores para 8 bits
    float scale = 255.0/((float)b-(float)s);

    // Para cada pixel na imagem
    for (int y=0;y<h;y++) {
        for (int x=0;x<w;x++) {
            int v = ((int)getpixel(out, x, y));
            if (v > 0xFFFF)
                v = -(v>>16);
            v *= scale;
            v += 129;
            if (v > 255)
                v = 255;
            if (v < 0)
                v = 0;
            putpixel (out, x,y, 0x00FFFFFF | ((int)v<<24));
        }
    }

   // cout << "Gen SDF finished" << endl;

    return out;
}

void ZUTF8Atlas::render (string& _text) {
    if (texture)
        delete texture;
    texture = new ZTextureAsset(0, string(""), string ("texture.text.utf8atlas"));

	SDL_Surface *renderedText;

	SDL_Color textColor = {255,255,255}; // Branco

	renderedText = TTF_RenderUTF8_Blended (font.font, _text.c_str(), textColor);

	if (!renderedText) {
		cout << "[INF]" << " [FAILED TO RENDER TEXT]" << endl;
        return;
	}

    SDL_Surface* potSurface = genSDF(renderedText);

    // Calcula os tamanhos para uma textura POT (Power Of Two) equivalente
    if (!potSurface) {
        cout << "[ERR] Creating [^2] renderedText" << endl;
    }

    //SDL_SetAlpha(renderedText, 0, renderedText->format->alpha);
    //SDL_SetAlpha(potSurface, 0, potSurface->format->alpha);

    //if (SDL_BlitSurface (renderedText, NULL, potSurface, NULL) == 0) {
    //    cout << "[INF] [CONV] [^2] {OK}" << endl;
    //} else {
    //    cout << "[INF] [CONV] [^2] {FAIL}" << endl;
    //}

    //SDL_SaveBMP (potSurface, "debug.bmp");

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
	texture->width = renderedText->w*0.5;
	texture->height = renderedText->h*0.5;
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
	opacity = 1;
	createColorShader();
}

ZTextManager::~ZTextManager () {
	//if (utf8atlas != NULL)
	//	delete utf8atlas;
}

void ZTextManager::initTextureMaps (ZFontAsset *_font) {
	defaultFont = _font;
    utf8atlas = new ZUTF8Atlas(*defaultFont);
 }

void ZTextManager::drawString (Vec3 _pos, string& _text, float _h) {
   float scale = (float)_h/32.0;

    glPushMatrix();
        if (opacity < 1)
            glEnable (GL_BLEND);
        else
            glDisable (GL_BLEND);
        glColor4f (color.x,color.y,color.z,opacity);
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

void ZTextManager::registerCharacters (string& _text) {
    vector <uint16_t> allchars;

    for (int i=0;i<_text.size();i++) {
        uint16_t utf8;
        if (_text[i] > 192)
            utf8 = (uint16_t)_text[i] | (((uint16_t)_text[i+=1])<<8);
        else
            utf8 = _text[i];
    }

    utf8atlas->registerCharacters(allchars);
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
        if (opacity < 1)
            glEnable (GL_BLEND);
        else
            glDisable (GL_BLEND);
        glColor4f (color.x,color.y,color.z,opacity);
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

void ZTextManager::setOpacity (float _opacity) {
    opacity = _opacity;
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
