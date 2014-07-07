#include "ZInputManager.h"
#include "../ZEngine/ZEngine.h"

ZJoystickButton::ZJoystickButton (string _name) {
	name = _name;
	state = false;
	shot = state;
}

ZJoystickButton::~ZJoystickButton () {

}

bool ZJoystickButton::isShot () {
	if (shot) {
		shot = false;
		return true;
	}
	return false;
}

string ZJoystickButton::getName () {
	return name;
}

ZJoystick::ZJoystick (long int _jid) {
	jid = _jid;
}

ZJoystick::~ZJoystick () {

}

void ZJoystick::press (int _bid) {
	if (_bid >= buttons.size()) {
		cout << "[ERR] [INTERNAL INPUT ERROR]" << endl;
		return;
	}

	buttons[_bid].state = true;
	buttons[_bid].shot = true;
	cout << "[JOY] " << jid << " [" << buttons[_bid].getName() << "] [Z]" << endl;

	sendEvent();
}

void ZJoystick::unpress (int _bid) {
	if (_bid >= buttons.size()) {
		cout << "[ERR] [INTERNAL INPUT ERROR]" << endl;
		return;
	}

	buttons[_bid].state = false;
	buttons[_bid].shot = false;

	if (Engine->debug)
		cout << "[JOY] " << jid << " [" << buttons[_bid].getName() << "] [ ]" << endl;

	sendEvent();
}

void ZJoystick::move (int _aid, int _val) {
	if (_aid >= axis.size()) {
		cout << "[ERR] [INTERNAL INPUT ERROR]" << endl;
		return;
	}

	axis [_aid].value = ((float)_val)/32768.0f;

	if (Engine->debug)
		cout << "[JOY] " << jid << " " << axis [_aid].value << endl;

	sendEvent();
}

long int ZJoystick::getJid () {
	return jid;
}

void ZJoystick::sendEvent () {
	ZUI::Event *event = new ZUI::Event(ZUI::Event::Joystick, NULL, NULL,this);
	Engine->guiManager->routeEvent(event);
	delete event;
}

ZKeyboard::ZKeyboard (long int _kid) {
	kid = _kid;
	keys = new bool[1024]; // 1024 Keys?
	skeys = new bool[1024]; // 1024 Keys?
	for (int i=0;i<1024;i++) {
		keys[i] = false;
		skeys[i] = false;
	}
}

ZKeyboard::~ZKeyboard () {
	delete[] keys;
	delete[] skeys;
}

bool ZKeyboard::isShot (int _k) {
	if (skeys[_k]) {
		skeys[_k] = false;
		return true;
	}
	return false;
}

void ZKeyboard::press (int _k) {
	if (_k == SDLK_EQUALS) {
		if (Engine->inputManager->getMice().size() > 0) {
			Engine->inputManager->getMice()[0]->scroll (1);
		}
	}
	if (_k == SDLK_MINUS) {
		if (Engine->inputManager->getMice().size() > 0) {
			Engine->inputManager->getMice()[0]->scroll (-1);
		}
	}

	keys[_k] = true;
	skeys[_k] = true;
	cout << "[KBD] [PRESS] " << SDL_GetKeyName ((SDLKey)_k) << endl;
	sendEvent();
}

void ZKeyboard::unpress (int _k) {
	keys[_k] = false;
	skeys[_k] = false;
	cout << "[KBD] [UNPRESS] " << SDL_GetKeyName ((SDLKey)_k) << endl;
	sendEvent();
}

void ZKeyboard::setUnicode (int _unicode) {
	unicode = _unicode;
	sendEvent();
}

int ZKeyboard::getUnicode () {
	return unicode;
}

long int ZKeyboard::getKid () {
	return kid;
}

void ZKeyboard::sendEvent () {
	ZUI::Event *event = new ZUI::Event(ZUI::Event::Keyboard, NULL, this, NULL);
	Engine->guiManager->routeEvent(event);
	delete event;
}

ZMouse::ZMouse (long int _mid) {
	mid = _mid;
	pos = vec3();
}

ZMouse::~ZMouse () {
}

void ZMouse::move (float _x, float _y) {
	pos.x = _x;
	pos.y = _y;

	if (Engine->debug)
		cout << "[MOUSE] [POS] " << pos.x << "," << pos.y << endl;

	sendEvent();
}

void ZMouse::scroll (float _scroll) {
	if (abs (pos.z) < 1)
		pos.z += _scroll;

	cout << "[MOUSE] [SCROLL] " << pos.z << endl;

	sendEvent();
}

void ZMouse::press (int _b) {
	switch (_b) {
		case SDL_BUTTON_LEFT:
			left = true;
			lshot = true;
		break;
		case SDL_BUTTON_RIGHT:
			right = true;
			rshot = true;
		break;
		case SDL_BUTTON_MIDDLE:
			center = true;
			cshot = true;
		break;
		case 4:
			scrollUp = true;
			sUpShot = true;
			scroll (0.5);
		break;
		case 5:
			scrollDown = true;
			sDownShot = true;
			scroll (-0.5);
		break;
	}

	cout << "[MOUSE] [PRESS] " << _b << endl;

	sendEvent();
}

void ZMouse::unpress (int _b) {
	switch (_b) {
		case SDL_BUTTON_LEFT:
			left = false;
			lshot = false;
		break;
		case SDL_BUTTON_RIGHT:
			right = false;
			rshot = false;
		break;
		case SDL_BUTTON_MIDDLE:
			center = false;
			cshot = false;
		break;
		case 4:
			scrollUp = false;
			sUpShot = false;
		break;
		case 5:
			scrollDown = false;
			sDownShot = false;
		break;
	}

	cout << "[MOUSE] [UNPRESS] " << _b << endl;

	sendEvent();
}

bool ZMouse::leftIsShot () {
	if (lshot) {
		return true;
	}
	return false;
}

bool ZMouse::rightIsShot () {
	if (rshot) {
		return true;
	}
	return false;
}

bool ZMouse::centerIsShot () {
	if (cshot) {
		return true;
	}
	return false;
}

void ZMouse::clear () {
	cshot = false;
	rshot = false;
	lshot = false;
}

void ZMouse::sendEvent () {
	ZUI::Event *event = new ZUI::Event(ZUI::Event::Mouse, this, NULL, NULL);
	Engine->guiManager->routeEvent(event);
	delete event;
}

ZInputManager::ZInputManager () {
	mJid = 0;
	mMid = 0;
	mKid = 0;

	joysticks = new vector <ZJoystick*> ();
	mice      = new vector <ZMouse*> ();
	keyboards = new vector <ZKeyboard*> ();

	cout << "[INF] [DETECTED " << SDL_NumJoysticks() << " JOYSTICKS ]" << endl;
	cout << "[INF] [CONFIGURING JOYSTICKS]" << endl;

	SDL_JoystickEventState (SDL_ENABLE);

	for (int i=0;i<SDL_NumJoysticks();i++) {
		auto joy = new ZJoystick (mJid++);
		stringstream ss;
		ss << SDL_JoystickName (i);
		joy->deviceName = ss.str();
		joy->sdlJoystick = SDL_JoystickOpen (i);

		for (int j=0;j<SDL_JoystickNumButtons(joy->sdlJoystick);j++) {
			// Only madness to generate readable joystick button ID
			joy->buttons.push_back (ZJoystickButton("B"));
		}

		for (int j=0;j<SDL_JoystickNumAxes(joy->sdlJoystick);j++) {
			joy->axis.push_back (ZJoystickAxis());
		}

		joysticks->push_back (joy);
		cout << "[INF] [JOY] [" << joy->deviceName << "]" << endl;
	}

	keyboards->push_back (new ZKeyboard(mKid++));
	mice->push_back (new ZMouse (mMid++));
}

ZInputManager::~ZInputManager () {
	for (auto j :(*joysticks)) {
		SDL_JoystickClose (j->sdlJoystick);
		delete j;
	}

	for (auto m :(*mice)) {
		delete m;
	}

	for (auto k :(*keyboards)) {
		delete k;
	}

	delete joysticks;
	delete mice;
	delete keyboards;
}

void ZInputManager::clear () {
	for (auto m :(*mice)) {
		m->clear();
	}
}

void ZInputManager::joystickAxisEvent (SDL_Event& _evt) {
	int jid = (int)_evt.jaxis.which;
	int aid = (int)_evt.jaxis.axis;
	int val = (int)_evt.jaxis.value;

	if (jid < joysticks->size()) {
		(*joysticks)[jid]->move(aid,val);
	} else {
		cout << "[ERR] [INTERNAL INPUT ERROR]" << endl;
	}
}

void ZInputManager::joystickButtonEvent (SDL_Event& _evt) {
	int jid = (int)_evt.jbutton.which;
	int bid = (int)_evt.jbutton.button;

	if (jid < joysticks->size()) {
		if (_evt.type == SDL_JOYBUTTONDOWN) {
			(*joysticks)[jid]->press(bid);
		}
		else { // UP
			(*joysticks)[jid]->unpress(bid);
		}
	} else {
		cout << "[ERR] [INTERNAL INPUT ERROR]" << endl;
	}
}

void ZInputManager::keyboardKeyEvent (SDL_Event& _evt) {
	int kid = (int)_evt.key.keysym.sym;

	if (_evt.type == SDL_KEYDOWN) {
		int unicode = _evt.key.keysym.unicode;
		(*keyboards)[0]->press (kid);
		(*keyboards)[0]->setUnicode (unicode);
	} else {
		(*keyboards)[0]->setUnicode (-1);
		(*keyboards)[0]->unpress (kid);
	}
}

void ZInputManager::mouseMoveEvent (SDL_Event& _evt) {
	(*mice)[0]->move (	(float)_evt.motion.x,
						(float)_evt.motion.y);
}

void ZInputManager::mouseButtonEvent (SDL_Event& _evt) {
	if (_evt.button.type == SDL_MOUSEBUTTONDOWN)
		(*mice)[0]->press ((int)_evt.button.button);
	else
		(*mice)[0]->unpress ((int)_evt.button.button);
}

vector<ZJoystick*> ZInputManager::getJoysticks () {
	return *joysticks;
}

vector<ZKeyboard*> ZInputManager::getKeyboards () {
	return *keyboards;
}

vector<ZMouse*> ZInputManager::getMice () {
	return *mice;
}
