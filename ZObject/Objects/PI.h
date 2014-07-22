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