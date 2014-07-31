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

	class VerticalList {
		vector <ZTextureAsset*> icons;
		function <void (int)> mOnSelect;
		int position;

		li translation;
	public:
		VerticalList();
		VerticalList (vector<ZTextureAsset*>);
		void onSelect (function <void (int)>);
		void addIcon (ZTextureAsset*);
		void draw (vec2, VerticalList*);

		void up();
		void down();
		void select ();
	
		int getPosition();

		void set (vector<ZTextureAsset*>);
	};

	class Category {
		ZTextureAsset *icon;
		vector <Item*> items;
		static long nid;
		bool mIsExtern;
	public:
		long id;

		Category(string, bool=false);
		vector<ZTextureAsset*> getList ();
		ZTextureAsset* getIcon();

		void addItem (Item*);

		bool isExtern();
	};

	class InventoryTree {
		vector <Category> categories;
	public:
		InventoryTree();
		vector<ZTextureAsset*> getList ();
		vector<ZTextureAsset*> getItemList (int);
		vector<ZTextureAsset*> getExternList ();
	
		void addCategory (Category);
		void removeCategory (long);
	};

	class Inventory {
		bool display;

		int position;
		VerticalList category;
		VerticalList items;
		VerticalList actions;

	public:
		InventoryTree tree;

		Inventory();

		void draw ();

		void up();
		void down();
		void left();
		void right();

		void show();
		void hide();
	private:
		VerticalList* getList();
	};
}

#endif /* PI_H */