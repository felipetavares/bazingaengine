#include "ZEngine/ZEngine.h"

ZUtil::ZUtil () {

}

ZUtil::~ZUtil () {

}

Selector::Selector ():
	y(vec2(tnow, tnow), vec2(-Engine->videoManager->windowHeight/2,-Engine->videoManager->windowHeight/2)) {

}

Selector::Selector (vector <PI::Item*> _icons):
	y(vec2(tnow, tnow), vec2(-Engine->videoManager->windowHeight/2,-Engine->videoManager->windowHeight/2)) {
	icons = _icons;
}

void Selector::addItem (PI::Item* item) {
	if (item != NULL)
		icons.push_back(item);
}

void Selector::setIcons (vector <PI::Item*> _icons) {
	icons = _icons;
}

void Selector::draw () {
	float windowWidth = Engine->videoManager->windowWidth;
	float windowHeight = Engine->videoManager->windowHeight;

	float startY = 0;

	auto back = Engine->assetsManager->getAsset<ZTextureAsset*> ("image.menu.back");
	auto top = Engine->assetsManager->getAsset<ZTextureAsset*> ("image.menu.top");

	glDisable (GL_DEPTH_TEST);
	glPushMatrix();
		glLoadIdentity();
		glBindTexture(GL_TEXTURE_2D, back->id);
		glColor4f(0.25,0.25,0.25,1);
		float w = windowWidth*2/(float)back->rwidth/4;
		float h = windowHeight*2/(float)back->rheight/4;
		glBegin(GL_QUADS);
			glTexCoord2f (0,0);
			glVertex3f(-windowWidth, y.v(), 0);
			glTexCoord2f (w,0);
			glVertex3f(windowWidth, y.v(), 0);
			glTexCoord2f (w,h);
			glVertex3f(windowWidth, y.v()-windowHeight, 0);
			glTexCoord2f (0,h);
			glVertex3f(-windowWidth, y.v()-windowHeight, 0);
		glEnd();
		w = windowWidth*2/(float)top->rwidth/4;
		glBindTexture(GL_TEXTURE_2D, top->id);
		glBegin(GL_QUADS);
			glTexCoord2f (0,0);
			glVertex3f(-windowWidth, y.v()+8, 0);
			glTexCoord2f (w,0);
			glVertex3f(windowWidth, y.v()+8, 0);
			glTexCoord2f (w,1);
			glVertex3f(windowWidth, y.v()-top->rheight+8, 0);
			glTexCoord2f (0,1)	;
			glVertex3f(-windowWidth, y.v()-top->rheight+8, 0);
		glEnd();
	glPopMatrix();

	glPushMatrix();
		glLoadIdentity();

		glTranslatef(0, y.v()-windowHeight/4, 0);

		float n = 0;
		for (auto i :icons) {
			glPushMatrix();
				glTranslatef (n, 0, 0);
				if (i)
					i->draw(vec3());
			glPopMatrix();
			n+=i->getSize().x;
		}

	glPopMatrix();
	glEnable (GL_DEPTH_TEST);	
}

void Selector::open () {
	y = li(vec2(tnow, tnow+0.3), vec2(y.v(), 0));
}

void Selector::close () {
	float windowHeight = Engine->videoManager->windowHeight;
	y = li(vec2(tnow, tnow+0.3), vec2(y.v(), -windowHeight/2));
}

bool Selector::isHidden () {
	float windowHeight = Engine->videoManager->windowHeight;
	return y.v() <= -windowHeight/2;
}