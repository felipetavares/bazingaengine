#ifndef ZFILESYSTEMMANAGER_H
#define ZFILESYSTEMMANAGER_H


// Macros to detect in what plataform I'm building
#ifdef MINGW32
#define WIN32
#endif
#ifdef __WIN32__
#define WIN32
#endif
#ifdef _WIN32
#define WIN32
#endif

//#define WIN32

#include <iostream>
#include <fcntl.h>	  // Low level file creation
#include <sys/types.h> // For Linux/Windows
#include <sys/stat.h> // For Linux/Windows
#include <cstdio>     // Files
#include <unistd.h>   // Deleting
#include <cstring>   // Memcpy
#include <algorithm>
using namespace std;

class ZFilePath {
	string path; // plataform-specific string
	string oPath; // non plataform-specific string
public:
	ZFilePath (string="");
	~ZFilePath ();

	void setPath(string);   // set path in non-plataform-specific way (using ':' )
	string getPath(); // get plataform specific path
	string getExtension();
	string getOriginalPath();
	bool isEqual(ZFilePath);
private:
	void setWindowsPath(string);
	void setLinuxPath(string);
	//setMacOsPath();
	//setAndroidPath();
	//setiOSPath();
};

class ZFilesystemManager {
	const int bufferSize = 4096; // 4 K of memory
public:
	ZFilesystemManager ();
	~ZFilesystemManager ();

	bool fileExists (ZFilePath);
	bool copyFile (ZFilePath,ZFilePath);
	bool renameFile (ZFilePath,ZFilePath);
	bool deleteFile (ZFilePath);
	bool createFile (ZFilePath);
	size_t getFileSize (ZFilePath);
	char* getFileData (ZFilePath);
	bool setFileData(ZFilePath, const char*, size_t);
};

#endif /* ZFILESYSTEMMANAGER_H */
