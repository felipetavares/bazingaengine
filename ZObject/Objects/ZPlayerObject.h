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
		bool display;
		
		ZTextureAsset *back;
		ZTextureAsset *top;

		li y;
		li angle;
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
	bool interacting;
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

class ShotCallback : public b2RayCastCallback {
public:
	bool c = false;

	ShotCallback();

	float32 ReportFixture(b2Fixture*, const b2vec2&,const b2vec2&, float32);

	b2Fixture* m_fixture;
	b2vec2 m_point;
	b2vec2 m_normal;
	float32 m_fraction = 99999;
};

class InteractCallback : public b2RayCastCallback {
public:
	bool c = false;
	b2Fixture* m_fixture;
	b2vec2 m_point;
	b2vec2 m_normal;
	float32 m_fraction = 99999;

	InteractCallback();

	float32 ReportFixture(b2Fixture*, const b2vec2&,const b2vec2&, float32);
};

#endif /* ZPLAYEROBJECT_H */
