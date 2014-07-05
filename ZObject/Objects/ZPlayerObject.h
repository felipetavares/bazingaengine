#ifndef ZPLAYEROBJECT_H
#define ZPLAYEROBJECT_H

#include "ZObject/ZObject.h"
#include "ZAssetsManager/ZAssetsManager.h"

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
		void draw (Vec3);

		Vec2 getSize();
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
	public:
		Inventory();

		void addItem (Item*);
		void removeItem (Item*);
		void draw();

		Item* getItem();
		void nextItem();
		void prevItem();
	};
}

class ZPlayerObject: public ZObject {
	PI::Inventory inventory;
public:
	ZAnimation* anims[4];

	bool reinit = false;

	int playerNum = 0;

	Vec3 dir;
	ZPlayerObject (long int, Vec3=Vec3(), Vec3=Vec3());

	void init();
	void step();
	void shot();
	void draw();
private:
	float getAxis (int);
};

#endif /* ZPLAYEROBJECT_H */
