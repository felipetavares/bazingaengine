#ifndef PI_H
#define PI_H

class ZPlayerObject;

/**
    Player Items
*/
namespace PI {
	class Item {
	public:
		ZTextureAsset *texture;

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

	class Category {
		ZTextureAsset* icon;
		vector <Item*> items;
		int position;

	public:
		Category(string);

		void addItem(Item*);
		void draw();
		ZTextureAsset* getIcon();
	};

	class Inventory {
		vector <Category> categories;
		int position;
		li translation;

	public:
		Inventory ();

		void addCategory (Category);
		void draw ();
		void up ();
		void down ();
	};
}

#endif /* PI_H */