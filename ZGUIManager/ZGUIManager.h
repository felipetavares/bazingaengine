#ifndef GUI_MAN_H
#define GUI_MAN_H

#include <functional>
#include <vector>
#include <map>
#include "../ZMath/Vec3.h"
#include "../ZMath/Vec2.h"

#include "../ZInputManager/ZInputManager.h"

// Macros for percents
#define pw(x) Engine->videoManager->windowWidth*(x)/100
#define ph(x) Engine->videoManager->windowHeight*(x)/100
#define ww Engine->videoManager->windowWidth
#define wh Engine->videoManager->windowHeight

//! Todas as classes relacionadas a UI estão neste namespace
namespace ZUI {

class Widget;

/**
	Classe para representar qualquer tipo de evento
*/
class Event {
public:
	enum Type {Mouse, Keyboard, Joystick};
private:
	Type type;
	ZJoystick *joystick = NULL;
	ZKeyboard *keyboard = NULL;
	ZMouse *mouse = NULL;
public:
	Event (Type, ZMouse*, ZKeyboard*, ZJoystick*);

	Type getType();
	ZMouse *getMouse();
	ZKeyboard *getKeyboard();
	ZJoystick *getJoystick();
};

/**
	Ações (causadas por eventos)
*/
class Action {
	function <void (Widget*, Event*)> action;
public:
	Action (function <void (Widget*, Event*)>);

	void act(Widget*, Event*);
};

/**
	Filtragem de eventos
*/
class Filter {
	function <bool (Widget*, ZMouse*)> mouse;
	function <bool (Widget*, ZKeyboard*)> keyboard;
	function <bool (Widget*, ZJoystick*)> joystick;
	Widget *widget;
	Event::Type type;
public:
	Filter (Widget*, function <bool (Widget*, ZMouse*)>);
	Filter (Widget*, function <bool (Widget*, ZKeyboard*)>);
	Filter (Widget*, function <bool (Widget*, ZJoystick*)>);

	bool filter (Event*);
};

/**
	Um Widget é um elemento de interface genérico
	(el. de interf. = botões, imagens, scroolers)
*/
class Widget {
	map <Filter*,Action*> *actions = NULL;
public:
	Vec2 p,s;
	float z;
public:
	//! Construtor
	Widget();
	//! Desconstrutor
	~Widget();

	//! Adiciona uma ação, para um determinado filtro
	void addAction (Filter*, Action*);

	//! Desenha o widget
	virtual void draw(Vec2,Vec2,float);
	//! Roteia um evento
	void routeEvent(Event*);
};

/**
	Widget básico: botão
	Todos os widgets precisam se iniciar com "w"
*/
class wButton: public Widget {
	string text;
public:
	wButton(string);
	void setText(string);
	void draw (Vec2,Vec2,float);
};

/**
	Barra de progresso
*/
class wProgressBar: public Widget {
	string text;
	float progress;
public:
	wProgressBar(string);
	void setText(string);
	void setProgress(float);
	void draw (Vec2,Vec2,float);
};

/**
	Entrada de texto
*/
class wEntry: public Widget {
	string text;
public:
	wEntry(string);
	void processKeyEvent(Event*);
	void draw (Vec2,Vec2,float);
};

/**
	Ícone
*/
class wIcon: public Widget {
	unsigned int aid;
public:
	wIcon(unsigned int);
	void draw (Vec2,Vec2,float);
    unsigned int getAid();
};

/**
	Um panel é como uma janela, e contém vários
	componentes internos (widgets)
*/
class Panel {
	float z;
	Vec2 p,s;

	//! True se o panel estiver visível
	bool visible;
	bool haveFocus;
	//! Vetor com todos os widgets
	vector <Widget*> widgets;
	//! Tamanho da barra de título
	int bar;

	//! True se o panel estiver sendo arrastado
	bool dragged;

	//! Distância do topo da janela até o ponto do clique
	Vec2 dragDist;

	Widget* focus;
public:
	Panel(Vec2,Vec2,int);
	~Panel();

	//! Funções para controlar o estado de visibilidade
	bool isVisible();
	void setVisibility(bool);

	void addWidget (Widget*);

	void draw();

	bool routeEvent(Event*);

	void bringBack();
	void bringFront();

	void setZ(float);

	void requestFocus(Widget*);
};

}

class ZGUIManager {
	vector <ZUI::Panel*> panels;
	ZUI::Panel *focus;

public:
	function <bool (ZUI::Widget*,ZMouse*)> onclick;
	function <bool (ZUI::Widget*,ZKeyboard*)> keydown;

	ZGUIManager();
	~ZGUIManager();

	void addPanel(ZUI::Panel*);
	void removePanel(ZUI::Panel*);
	void render ();
	void requestFocus(ZUI::Panel*, ZUI::Widget*);

	void bringFront(ZUI::Panel*);
	void routeEvent(ZUI::Event*);
};

#endif /* GUI_MAN_H */
