#ifndef ZPLAYEROBJECT_H
#define ZPLAYEROBJECT_H

#include "ZObject/ZObject.h"
#include "ZAssetsManager/ZAssetsManager.h"

class ZPlayerObject;

/**
    Player Objects
*/
namespace PO {
	class Object {
	protected:
		ZTextureAsset *texture;
	public:
		Object (ZTextureAsset*);
		virtual void action (ZPlayerObject*, vector <Object*>) = 0;
		virtual string getName() = 0;
		void draw (Vec3);
	};

	class Matches: public Object {
	public:
		Matches();
		void action (ZPlayerObject*, vector <Object*>);
		string getName();
	};
}

class ZPlayerObject: public ZObject {
	vector <PO::Object*> inventory;
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
	void drawInventory();
};

#endif /* ZPLAYEROBJECT_H */
