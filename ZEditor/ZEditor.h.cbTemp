#ifndef EDITOR_H
#define EDITOR_H

class ZEditor {
	Vec3 dragPos;
	bool drag;
	ZObject* subject;
public:
	bool grid;
	Vec3 *gridColor;
	Vec3 *gridSize;

	ZEditor ();
	~ZEditor ();

	void run ();
	void drawHUD();

	void createObject(unsigned int);

	long int findNextTextureAsset(long int);
	long int findPrevTextureAsset(long int);
};

#endif /* EDITOR_H */
