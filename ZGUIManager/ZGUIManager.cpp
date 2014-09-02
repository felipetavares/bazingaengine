#include "ZGUIManager.h"
#include "../ZEngine/ZEngine.h"

using namespace ZUI;

//! Eventos
Event::Event (Type _type, ZMouse *_mouse 	   = NULL,
						  ZKeyboard *_keyboard = NULL,
						  ZJoystick *_joystick = NULL) {
	type = _type;
	mouse = _mouse;
	keyboard = _keyboard;
	joystick = _joystick;
}

Event::Type Event::getType () {
	return type;
}

ZMouse* Event::getMouse () {
	return mouse;
}

ZKeyboard* Event::getKeyboard () {
	return keyboard;
}

ZJoystick* Event::getJoystick () {
	return joystick;
}

//!Ações
Action::Action (function <void (Widget*, Event*)> _action) {
	action = _action;
}

void Action::act (Widget *_widget, Event* _event) {
	action (_widget,_event);
}

//! Filtros
Filter::Filter (Widget* _widget,
				function <bool (Widget*, ZMouse*)> _mouse) {
	mouse = _mouse;
	widget = _widget;
	type = Event::Mouse;
}

Filter::Filter (Widget* _widget,
				function <bool (Widget*, ZKeyboard*)> _keyboard) {
	keyboard = _keyboard;
	widget = _widget;
	type = Event::Keyboard;
}

Filter::Filter (Widget* _widget,
				function <bool (Widget*, ZJoystick*)> _joystick) {
	joystick = _joystick;
	widget = _widget;
	type = Event::Joystick;
}

bool Filter::filter (Event* _event) {
	if (_event->getType() == type) {
		switch (type) {
			case Event::Mouse:
				return mouse(widget, _event->getMouse());
			break;
			case Event::Keyboard:
				return keyboard(widget, _event->getKeyboard());
			break;
			case Event::Joystick:
				return joystick(widget, _event->getJoystick());
			break;
		}
	}
}

//! Widgets
Widget::Widget () {
	actions = new map <Filter*, Action*> ();
}

Widget::~Widget () {
	//! Para todas as ações
	for (auto action :*actions) {
		//! Deleta o filtro
		delete action.first;
		// Deleta a ação
		delete action.second;
	}

	delete actions;
}

void Widget::draw (vec2 _p, vec2 _s, float _z) {
	z = _z;
	p = _p;
	s = _s;
}

void Widget::addAction (Filter *_filter, Action* _action) {
	actions->insert (pair<Filter*,Action*>(_filter, _action));
}

void Widget::routeEvent (Event* _event) {
	for (auto action :*actions) {
		if (action.first->filter(_event)) {
			action.second->act(this, _event);
		}
	}
}

//! Panels
Panel::Panel (vec2 _p, vec2 _s, int _bar) {
	p = _p;
	s = _s;
	z = 99.9;
	bar = _bar;
	drawStart = 0;
	visible = false;
	dragged = false;
	haveFocus = false;
	focus = NULL;
}

Panel::~Panel () {
	for (auto widget :widgets) {
		delete widget;
	}
}

bool Panel::isVisible () {
	return visible;
}

void Panel::setVisibility (bool _visible) {
	visible = _visible;
}

void Panel::addWidget (Widget* _widget) {
	widgets.push_back(_widget);
}

void Panel::draw() {
	if (!visible)
		return;

	glPushMatrix();
	glDisable (GL_TEXTURE_2D);
		if (haveFocus)
			glColor3f (0,0,1);
		else
			glColor3f (0.5,0.5,0.5);
		glBegin(GL_QUADS);
			glVertex3f(p.x, p.y, z);
			glVertex3f(p.x+s.x, p.y, z);
			glVertex3f(p.x+s.x, p.y+bar, z);
			glVertex3f(p.x, p.y+bar, z);
		glEnd();
		glColor3f (0,0,0);
		glBegin(GL_QUADS);
			glVertex3f(p.x, p.y, z);
			glVertex3f(p.x+s.x, p.y, z);
			glVertex3f(p.x+s.x, p.y+s.y, z);
			glVertex3f(p.x, p.y+s.y, z);
		glEnd();
	glEnable (GL_TEXTURE_2D);
	glPopMatrix();

	// Drawing start, Drawing end
	int dStart = drawStart;
	int dSize = (s.y-bar*2)/32;
	int dEnd = dStart+dSize;

	if (dEnd > widgets.size())
		dEnd = widgets.size();

	float h = (s.y-bar*2)/(dEnd-dStart);

	for (int i=dStart;i<dEnd;i++) {
		widgets[i]->draw(vec2(p.x,p.y+bar+bar/2+h*(i-dStart)),
						 vec2(s.x,h),
						 z+0.05);
	}
}

void Panel::setZ (float _z) {
	z = _z;
}

bool Panel::routeEvent(Event* _event) {
	bool process = false;

	if (visible) {
		//! Panel movement
		if (_event->getType() == Event::Mouse) {
			vec3 mp = _event->getMouse()->pos;
			if (mp.x >= p.x && mp.x <= p.x+s.x &&
				mp.y >= p.y && mp.y <= p.y+s.y) {
				process = true;
				if (_event->getMouse()->leftIsShot()) {
					if (!haveFocus) {
						Engine->guiManager->bringFront (this);
					}
				}
			}

			if (haveFocus) {
				if (mp.x >= p.x && mp.x <= p.x+s.x &&
					mp.y >= p.y && mp.y <= p.y+bar) {
					if (_event->getMouse()->leftIsShot()) {
						dragged = true;
						dragDist = vec2(mp.x-p.x,mp.y-p.y);
					}
				}
				if (!_event->getMouse()->left) {
					dragged = false;
				}

				if (dragged) {
					p.x = mp.x-dragDist.x;
					p.y = mp.y-dragDist.y;
					return true;
				}


				if (mp.x >= p.x && mp.x <= p.x+s.x &&
				    mp.y >= p.y+s.y-bar/2 && mp.y <= p.y+s.y &&
				    _event->getMouse()->leftIsShot()) {
					if (drawStart+int((s.y-bar*2)/32) < widgets.size()-1)
						drawStart++;
				}

				if (mp.x >= p.x && mp.x <= p.x+s.x &&
				    mp.y >= p.y+bar && mp.y <= p.y+bar+bar/2 &&
				    _event->getMouse()->leftIsShot()) {
					if (drawStart > 0)
						drawStart--;
				}
			}
		}

		if (haveFocus) {
			if (_event->getType() == ZUI::Event::Keyboard) {
				if (focus) {
					focus->routeEvent(_event);
				}
			} else
			for (auto widget :widgets) {
				widget->routeEvent (_event);
			}
		}
	}

	return process;
}

void Panel::requestFocus (ZUI::Widget *_widget) {
	focus = _widget;
}

void Panel::bringBack () {
	haveFocus = false;
}

void Panel::bringFront () {
	haveFocus = true;
}

//! Speaks
Speak::Speak(string _text) {
	charsToDraw = 0;
	cline = 0;
	lines.push_back (new string(""));
	lastAddTime = 0;
	text = _text;
	textSize = text.size();
}

Speak::~Speak () {
    for (auto *str :lines) {
        delete str;
    }
}

void Speak::draw () {
	// Font size
	int fsize = 32*pw(100)/800.0;
	// Margin
	int m = 32,n;
	// Position
	vec2 p = vec2(10+pw(10), ph(67)-10);
	// Size
	vec2 s = vec2(pw(80)-10, ph(33)-10);

	glColor3f (0.3,0.3,0.3);
	Engine->util->fillRect (p-vec2(10,-10), s, 50);
	glColor3f (1,1,1);
	Engine->util->fillRect (p, s, 51);
	glColor3f (0,0,0);
	Engine->util->strokeRect (p, s, 52);

	if (Engine->realTime->currentTime-lastAddTime > 0.05) {
		if (charsToDraw < textSize) {
		    char newChar = text.c_str()[charsToDraw++];
			lines.at(cline)->push_back(newChar);
			lastAddTime = Engine->realTime->currentTime;
		}
	}

    if (Engine->textManager->measureString(*lines[cline], fsize) > s.x-2*m) {
		int p = lines[cline]->size()-1;

		while (lines[cline]->at(p) != ' ' && p >= lines[cline]->size()-1) {
                        p--;
			charsToDraw--;
		}

        string *oldString = lines[cline];
		lines[cline] = new string(lines[cline]->substr(0, p));
        delete oldString;

		if (lines.size()*fsize+fsize < s.y-2*m) {
			cline++;
			lines.push_back (new string(""));
		} else {
			for (int l=0;l<lines.size()-1;l++)
				lines[l] = lines[l+1];
			lines[cline]->assign("");
		}
    }

	n=0;
	for (auto line :lines) {
		Engine->textManager->setColor(vec3(0,0,0));
		Engine->textManager->drawString (vec3(p.x+m,p.y+fsize*n+m,100),*line,fsize);
		n++;
	}
}

//! UI Manager
ZGUIManager::ZGUIManager () {
	focus = NULL;
}

ZGUIManager::~ZGUIManager () {
	for (auto panel :panels) {
		delete panel;
	}
	for (auto speak :speaks) {
		delete speak;
	}
}

void ZGUIManager::requestFocus (ZUI::Panel *_panel, ZUI::Widget *_widget) {
	focus = _panel;
	focus->requestFocus(_widget);
}

void ZGUIManager::render () {
	glPushMatrix();
	glTranslatef (-pw(50),-ph(50),0);
	float z = 100;
	for (auto panel :panels) {
		panel->setZ(z-=0.1);
		panel->draw();
	}
	for (auto speak :speaks) {
		speak->draw();
	}
	glPopMatrix();
}

void ZGUIManager::addSpeak(ZUI::Speak* _speak) {
	speaks.push_back (_speak);
}

void ZGUIManager::addPanel (ZUI::Panel *_panel) {
	panels.push_back (_panel);
	bringFront(_panel);
}

void ZGUIManager::removePanel (ZUI::Panel *_panel) {
	for (int i=0;i<panels.size();i++)
		if (panels[i] == _panel)
			panels.erase(panels.begin()+i);
	delete _panel;

	for (auto panel :panels) {
		if (panel->isVisible())
			bringFront(panel);
	}
}

void ZGUIManager::routeEvent (ZUI::Event *_event) {
	if (_event->getType() == ZUI::Event::Keyboard) {
		if (focus) {
			focus->routeEvent(_event);
		}
	} else if (_event->getType() == ZUI::Event::Mouse) {
		for (auto panel :panels) {
			if (panel->routeEvent (_event))
				break;
		}
	} else {
		for (auto panel :panels) {
			panel->routeEvent (_event);
		}
	}
}

void ZGUIManager::bringFront (ZUI::Panel *_panel) {
	for (int i=0;i<panels.size();i++) {
		auto panel = panels[i];
		panel->bringBack();
		if (panel == _panel) {
			panels.erase (panels.begin()+i);
			i--;
		}
	}
	panels.insert(panels.begin(),_panel);
	_panel->bringFront();
}

//! Daqui em diante, código de desenho de widgets específicos
wButton::wButton (string _text):
	Widget() {
	text = _text;
}

void wButton::setText (string _text) {
	text = _text;
}

void wButton::draw (vec2 _p, vec2 _s, float _z) {
	Widget::draw(_p,_s,_z);

	glPushMatrix();
	glDisable (GL_TEXTURE_2D);
		glColor3f (1,0,0);
		glBegin(GL_LINE_LOOP);
			glVertex3f(_p.x, _p.y, _z);
			glVertex3f(_p.x+_s.x, _p.y, _z);
			glVertex3f(_p.x+_s.x, _p.y+_s.y, _z);
			glVertex3f(_p.x, _p.y+_s.y, _z);
		glEnd();
	glEnable (GL_TEXTURE_2D);

    int textH = 32;
    float coef = _s.x/Engine->textManager->measureString(text,32);
    if (coef < 1)
        textH *= coef;

	Engine->textManager->setColor (vec3(1,0,0));
	Engine->textManager->drawStringCentered (vec3(_p.x+_s.x/2,
												  _p.y+_s.y/2,
												  _z)
											,text, textH);

	glPopMatrix();
}

wProgressBar::wProgressBar (string _text):
	Widget() {
	text = _text;
	progress = 0;
}

void wProgressBar::setProgress (float _progress) {
	progress = _progress>1.0?1.0:_progress<0?0.0:_progress;
}

void wProgressBar::setText (string _text) {
	text = _text;
}

void wProgressBar::draw (vec2 _p, vec2 _s, float _z) {
	Widget::draw(_p,_s,_z);

	glPushMatrix();
	glDisable (GL_TEXTURE_2D);
		glColor3f (1,0,0);
		glBegin(GL_LINE_LOOP);
			glVertex3f(_p.x, _p.y, _z);
			glVertex3f(_p.x+_s.x, _p.y, _z);
			glVertex3f(_p.x+_s.x, _p.y+_s.y, _z);
			glVertex3f(_p.x, _p.y+_s.y, _z);
		glEnd();
		glColor3f (0.5,0.2,0);
			glBegin(GL_QUADS);
			glVertex3f(_p.x, _p.y, _z);
			glVertex3f(_p.x+_s.x*progress, _p.y, _z);
			glVertex3f(_p.x+_s.x*progress, _p.y+_s.y, _z);
			glVertex3f(_p.x, _p.y+_s.y, _z);
		glEnd();
	glEnable (GL_TEXTURE_2D);

    int textH = 32;
    float coef = _s.x/Engine->textManager->measureString(text,32);
    if (coef < 1)
        textH *= coef;

	Engine->textManager->setColor (vec3(1,0,0));
	Engine->textManager->drawStringCentered (vec3(_p.x+_s.x/2,
												  _p.y+_s.y/2,
												  _z+0.01)
											,text, textH);

	glPopMatrix();
}

wIcon::wIcon (unsigned int _aid):
	Widget() {
	aid = _aid;
}

unsigned int wIcon::getAid () {
	return aid;
}

void wIcon::draw (vec2 _p, vec2 _s, float _z) {
	Widget::draw(_p,_s,_z);

	glPushMatrix();
	glDisable (GL_TEXTURE_2D);
		glColor3f (1,0,0);
		glBegin(GL_LINE_LOOP);
			glVertex3f(_p.x, _p.y, _z);
			glVertex3f(_p.x+_s.x, _p.y, _z);
			glVertex3f(_p.x+_s.x, _p.y+_s.y, _z);
			glVertex3f(_p.x, _p.y+_s.y, _z);
		glEnd();
	glEnable (GL_TEXTURE_2D);

	auto tex = Engine->assetsManager->getAsset<ZTextureAsset*>(aid);

	glColor3f (0.25,0.25,0.25);
	glBindTexture (GL_TEXTURE_2D, tex->id);
	glPushMatrix();
	glTranslatef(_p.x,_p.y,_z);
	glBegin(GL_QUADS);
	glTexCoord2f (0,0); glVertex3f(0, 0, 0);
	glTexCoord2f (_s.x/tex->rwidth,0); glVertex3f(_s.x, 0, 0);
	glTexCoord2f (_s.x/tex->rwidth,_s.y/tex->rheight); glVertex3f(_s.x, _s.y, 0);
	glTexCoord2f (0,_s.y/tex->rheight); glVertex3f(0, _s.y, 0);
	glEnd();
	glPopMatrix();

    /*
	Engine->textManager->setColor (vec3(1,0,0));
	Engine->textManager->drawStringCentered (vec3(_p.x+_s.x/2,
												  _p.y+_s.y/2,
												  _z)
											,text, 32);
    */

	glPopMatrix();
}

wEntry::wEntry(string _text) {
	text = _text;
}

void wEntry::processKeyEvent(Event* _event) {
	if (_event->getType() == ZUI::Event::Keyboard) {
		if (_event->getKeyboard()->getUnicode() > 0) {
			uint16_t unicode = _event->getKeyboard()->getUnicode();
            text += (char) unicode&0xFF;
            if ((uint8_t) (unicode&0xFF) > 192)
                text += (char) unicode>>8;
		}
	}
}

void wEntry::draw (vec2 _p,vec2 _s,float _z) {
	Widget::draw(_p,_s,_z);

	glPushMatrix();
	glDisable (GL_TEXTURE_2D);
		glColor3f (1,0,0);
		glBegin(GL_LINE_LOOP);
			glVertex3f(_p.x, _p.y, _z);
			glVertex3f(_p.x+_s.x, _p.y, _z);
			glVertex3f(_p.x+_s.x, _p.y+_s.y, _z);
			glVertex3f(_p.x, _p.y+_s.y, _z);
		glEnd();
	glEnable (GL_TEXTURE_2D);

    int textH = 32;
    float coef = _s.x/Engine->textManager->measureString(text,32);
    if (coef < 1)
        textH *= coef;

	Engine->textManager->setColor (vec3(1,0,0));
	Engine->textManager->drawStringCentered (vec3(_p.x+_s.x/2,
												  _p.y+_s.y/2,
												  _z)
											,text, textH);
    glPopMatrix();
}
