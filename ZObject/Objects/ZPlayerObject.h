#ifndef ZPLAYEROBJECT_H
#define ZPLAYEROBJECT_H

#include "ZObject/ZObject.h"
#include "ZAssetsManager/ZAssetsManager.h"
#include "ZMath/math/li/li.h"

class ZPlayerObject;

/**
    Player Items
*/
namespace PI {
	class Item {
	protected:
		ZTextureAsset *texture;
	public:
		Item (ZTextureAsset*);
		virtual void action (ZPlayerObject*, vector <Item*>) = 0;
		virtual string getName() = 0;
		void draw (vec3);

		vec2 getSize();
	};

	class Matches: public Item {
	public:
		Matches();
		void action (ZPlayerObject*, vector <Item*>);
		string getName();
	};

	class Inventory {
		vector <Item*> items;
		int currentItem;
		float currentPos;
		bool display;

		li y;

	public:
		Inventory();

		void addItem (Item*);
		void removeItem (Item*);
		void draw();

		Item* getItem();
		void nextItem();
		void prevItem();
	
		void setDisplay (bool);
	};
}

class ZPlayerObject: public ZObject {
	PI::Inventory inventory;
public:
	ZAnimation* anims[4];

	bool reinit = false;

	int playerNum = 0;

	vec3 dir;
	ZPlayerObject (long int, vec3=vec3(), vec3=vec3());

	void init();
	void step();
	void shot();
	void draw();
private:
	float getAxis (int);
};

#endif /* ZPLAYEROBJECT_H */
