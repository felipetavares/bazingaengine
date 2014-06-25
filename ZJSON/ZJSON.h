#ifndef ZJSON_H
#define ZJSON_H

#include <iostream>
#include <sstream>
#include <vector>
#include <algorithm>
using namespace std;

class ZjArray;
class ZjObject;

class ZjValue {
public:
	enum Type {jString,jNumber,jObject,jArray,jTrue,jFalse,jNull};
	Type type;

	string 		str;
	float 		number;
	ZjArray 	*array;
	ZjObject 	*object;

	ZjValue ();
	ZjValue (const ZjValue&);
	ZjValue (string&);
	ZjValue (float);
	ZjValue (bool);
	ZjValue (ZjArray*);
	ZjValue (ZjObject*);
	~ZjValue ();
};

class ZjArray {
public:
	vector <ZjValue*> array;

	ZjArray ();
	ZjArray (const ZjArray&);
	~ZjArray ();
};

class ZjObject {
public:
	vector <string> keys;
	vector <ZjValue*> values;

	ZjObject ();
	ZjObject (const ZjObject&);
	~ZjObject ();

	ZjValue* get(string);
};

class ZJSON {
public:
	ZJSON ();
	~ZJSON ();

	ZjObject* parse (string&);
	ZjObject* parseObject (long int&,string&);
	ZjValue* parseString (long int&,string&);
	ZjValue* parseValue (long int&,string&);
	ZjValue* parseNumber (long int&,string&);
	ZjArray* parseArray (long int&,string&);

	string dumpJSON (ZjObject*);
	string dumpObject (ZjObject*);
	string dumpArray (ZjArray*);
	string dumpValue (ZjValue*);
private:
	bool isChar (char,long int&,string&);
	bool isCharLookAhead (char,long int&,string&,int);
	bool isCharsLookAhead (string,long int&,string&,int);
	bool isCharLookAheadNoWhite (char,long int&,string&,int);
	bool isCharsLookAheadNoWhite (string,long int&,string&,int);
	void advance (long int&,string&);
	char getNextChar (long int&, string&);
	char getNextCharNoWhite (long int&, string&);
};

#endif /* JSON_H */