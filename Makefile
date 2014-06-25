DBG=-g

ifeq ($(OS),linux)
COMP=g++
LIBS=`sdl-config --cflags --libs` -lGL -lSDL_mixer -lSDL_image -lSDL_ttf -lBox2D
OUTPUT=Engine
else
ifeq ($(OS),windows)
COMP=i686-pc-mingw32-g++
LIBS=-lSDL_mixer -lmikmod -lpthread -lvorbisfile -lvorbis -logg -lsmpeg `i686-pc-mingw32-sdl-config --cflags --libs` -lglu32 -lopengl32
OUTPUT=Engine.exe
endif
endif

Engine: Main.cpp ZEngine/ZEngine.o ZMath/Vec3.o ZMath/Vec2.o ZObject/ZObject.o ZObject/ZGraphics.o ZAudioManager/ZAudioManager.o ZAssetsManager/ZAssetsManager.o ZVideoManager/ZVideoManager.o ZFilesystemManager/ZFilesystemManager.o ZInputManager/ZInputManager.o ZJSON/ZJSON.o ZUtil/ZUtil.o ZTextManager/ZTextManager.o ZCamera/ZCamera.o ZEditor/ZEditor.o ZGUIManager/ZGUIManager.o
	$(COMP) -std=c++11 Main.cpp Z*/*.o -o $(OUTPUT) $(LIBS) $(DBG)
ZEngine/ZEngine.o: ZEngine/ZEngine.cpp ZEngine/ZEngine.h
	$(COMP) -std=c++11 ZEngine/ZEngine.cpp -o ZEngine/ZEngine.o -c $(DBG)
ZMath/Vec3.o: ZMath/Vec3.cpp ZMath/Vec3.h
	$(COMP) -std=c++11 ZMath/Vec3.cpp -o ZMath/Vec3.o -c $(DBG)
ZMath/Vec2.o: ZMath/Vec2.cpp ZMath/Vec2.h
	$(COMP) -std=c++11 ZMath/Vec2.cpp -o ZMath/Vec2.o -c $(DBG)
ZObject/ZObject.o: ZObject/ZObject.cpp ZObject/ZObject.h
	$(COMP) -std=c++11 ZObject/ZObject.cpp -o ZObject/ZObject.o -c $(DBG)
ZObject/ZGraphics.o: ZObject/ZGraphics.cpp ZObject/ZGraphics.h
	$(COMP) -std=c++11 ZObject/ZGraphics.cpp -o ZObject/ZGraphics.o -c $(DBG)
ZAudioManager/ZAudioManager.o: ZAudioManager/ZAudioManager.cpp ZAudioManager/ZAudioManager.h
	$(COMP) -std=c++11 ZAudioManager/ZAudioManager.cpp -o ZAudioManager/ZAudioManager.o -c $(DBG)
ZAssetsManager/ZAssetsManager.o: ZAssetsManager/ZAssetsManager.cpp ZAssetsManager/ZAssetsManager.h
	$(COMP) -std=c++11 ZAssetsManager/ZAssetsManager.cpp -o ZAssetsManager/ZAssetsManager.o -c $(DBG)
ZVideoManager/ZVideoManager.o: ZVideoManager/ZVideoManager.cpp ZVideoManager/ZVideoManager.h
	$(COMP) -std=c++11 ZVideoManager/ZVideoManager.cpp -o ZVideoManager/ZVideoManager.o -c $(DBG)
ZFilesystemManager/ZFilesystemManager.o: ZFilesystemManager/ZFilesystemManager.cpp ZFilesystemManager/ZFilesystemManager.h
	$(COMP) -std=c++11 ZFilesystemManager/ZFilesystemManager.cpp -o ZFilesystemManager/ZFilesystemManager.o -c $(DBG)
ZInputManager/ZInputManager.o: ZInputManager/ZInputManager.cpp ZInputManager/ZInputManager.h
	$(COMP) -std=c++11 ZInputManager/ZInputManager.cpp -o ZInputManager/ZInputManager.o -c $(DBG)
ZJSON/ZJSON.o: ZJSON/ZJSON.cpp ZJSON/ZJSON.h
	$(COMP) -std=c++11 ZJSON/ZJSON.cpp -o ZJSON/ZJSON.o -c $(DBG)
ZUtil/ZUtil.o: ZUtil/ZUtil.cpp ZUtil/ZUtil.h
	$(COMP) -std=c++11 ZUtil/ZUtil.cpp -o ZUtil/ZUtil.o -c $(DBG)
ZTextManager/ZTextManager.o: ZTextManager/ZTextManager.cpp ZTextManager/ZTextManager.h
	$(COMP) -std=c++11 ZTextManager/ZTextManager.cpp -o ZTextManager/ZTextManager.o -c $(DBG)
ZCamera/ZCamera.o: ZCamera/ZCamera.cpp ZCamera/ZCamera.h
	$(COMP) -std=c++11 ZCamera/ZCamera.cpp -o ZCamera/ZCamera.o -c $(DBG)
ZEditor/ZEditor.o: ZEditor/ZEditor.cpp ZEditor/ZEditor.h
	$(COMP) -std=c++11 ZEditor/ZEditor.cpp -o ZEditor/ZEditor.o -c $(DBG)
ZGUIManager/ZGUIManager.o: ZGUIManager/ZGUIManager.cpp ZGUIManager/ZGUIManager.h
	$(COMP) -std=c++11 ZGUIManager/ZGUIManager.cpp -o ZGUIManager/ZGUIManager.o -c $(DBG)