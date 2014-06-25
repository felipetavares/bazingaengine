#include "ZJSON.h"

ZjValue::ZjValue (const ZjValue& _v) {
	type = _v.type;
	str = _v.str;
	number = _v.number;
	object = _v.object;
	array = _v.array;
}

ZjValue::ZjValue () {
	type = Type::jNull;
	array = NULL;
	object = NULL;
}

ZjValue::ZjValue (string& _str) : str() {
	type = Type::jString;
	str = _str;

	array = NULL;
	object = NULL;
}

ZjValue::ZjValue (float _number) {
	type = Type::jNumber;
	number = _number;

	array = NULL;
	object = NULL;
}

ZjValue::ZjValue (bool _bool) {
	if (_bool)
		type = Type::jTrue;
	else
		type = Type::jFalse;

	array = NULL;
	object = NULL;
}

ZjValue::ZjValue (ZjArray *_array) {
	type = Type::jArray;
	array = _array;

	object = NULL;
}

ZjValue::ZjValue (ZjObject *_object) {
	type = Type::jObject;
	object = _object;

	array = NULL;
}

ZjValue::~ZjValue () {
	if (type == Type::jObject && object != NULL)
		delete object;
	else if (type == Type::jArray && array != NULL)
		delete array;
}

ZjObject::ZjObject () {
}

ZjObject::ZjObject (const ZjObject& _o): keys(_o.keys), values (_o.values) {
}

ZjObject::~ZjObject () {	
}

ZjValue* ZjObject::get (string _key) {
	for (int i=0;i<keys.size();i++) {
		if (_key == keys[i]) {
			return values[i];
		}
	}

	return new ZjValue (false);
}

ZjArray::ZjArray () {
}

ZjArray::ZjArray (const ZjArray& _a): array(_a.array) {
}

ZjArray::~ZjArray () {
}

ZJSON::ZJSON () {

}

ZJSON::~ZJSON () {

}

string ZJSON::dumpJSON (ZjObject* _json) {
	string str;
	str += dumpObject (_json);	
	return str;
}

string ZJSON::dumpObject (ZjObject* _obj) {
	string str;
	str += "{\n";

	int i=0;
	for (auto& e :_obj->keys) {
		str += "\"" + e + "\"";
		str += ": ";
		str += dumpValue (_obj->values[i]);
		if (i<_obj->keys.size()-1)
			str += ",";
		str += "\n";
		i++;
	}
	
	str += "}";
	return str;
}

string ZJSON::dumpArray (ZjArray* _arr) {
	string str;
	str += "[\n";

	int i=0;
	for (auto& e :_arr->array) {
		str += "\"" +dumpValue (e) + "\"";
		if (i<_arr->array.size()-1)
			str += ",";
		str += ",";
		str += "\n";
		i++;
	}
	
	str += "]";

	return str;
}

string ZJSON::dumpValue (ZjValue* _val) {
	string str;
	stringstream ss;

	switch (_val->type) {
		case ZjValue::Type::jString:
			str += "\"" + _val->str +  "\"";
		break;
		case ZjValue::Type::jNumber:
			ss << _val->number;
			str += ss.str();
		break;
		case ZjValue::Type::jObject:
			str += dumpObject(_val->object);		
		break;
		case ZjValue::Type::jArray:
			str += dumpArray(_val->array);
		break;
		case ZjValue::Type::jTrue:
			str += "true";
		break;
		case ZjValue::Type::jFalse:
			str += "false";
		break;
		case ZjValue::Type::jNull:
			str += "null";
		break;
	}

	return str;
}

ZjObject* ZJSON::parse (string& _str) {
	//cout << "[INF] [ZJSON] [PARSING STRING] \"" << _str << "\"" << endl;
	cout << "[INF] [ZJSON] [PARSING STRING]" << endl;

	long int pos = 0; // Position in the string

	return parseObject(pos,_str);
}

ZjObject* ZJSON::parseObject (long int& _pos, string& _str) {
	ZjObject* object = new ZjObject();

	/* A json object starts with a { */
	if (isChar ('{',_pos,_str)) {
		// Following we have a string
		while (isCharLookAhead('"', _pos, _str, 1)) {
			ZjValue *keyName = parseString(_pos,_str);
			if (isChar (':',_pos,_str)) {
				ZjValue *value = parseValue (_pos,_str);

				object->keys.push_back (keyName->str);
				object->values.push_back (value);

				if (isCharLookAhead(',',_pos,_str, 1))
					isChar(',',_pos,_str);
			} else {
				cout << "[JSON] [ERR] expecting ':'" << endl;
			}
		}

		if (isChar('}',_pos,_str)) {
			return object;
		} else {
			cout << "[JSON] [ERR] expecting '}'" << endl;
		}
	} else {
		cout << "[JSON] [ERR] expecting '{'" << endl;
	}
}

ZjValue* ZJSON::parseString (long int& _pos, string& _str) {
	string jsonString = "";

	if (isChar ('"', _pos, _str)) {
		while (_str[_pos] != '"') {
			jsonString += _str[_pos++];
		}

		_pos++;

		ZjValue *v = new ZjValue(jsonString);

		return v;
	} else {
		// Error
	}
}

ZjValue* ZJSON::parseNumber (long int& _pos, string& _str) {
	float jsonNumber = 0;
	string jsonString = "";
	
	advance(_pos,_str); // Consume all white spaces

	if (isCharLookAheadNoWhite ('-', _pos, _str, 1)) {
		jsonString += _str[_pos++];
	}

	if (isCharLookAheadNoWhite ('0', _pos, _str, 1)) {
		jsonString += _str[_pos++];		
	} else if (isCharsLookAheadNoWhite ("123456789", _pos, _str, 1)) {
		jsonString += _str[_pos++];
		while (isCharsLookAheadNoWhite ("0123456789", _pos, _str, 1))
			jsonString += _str[_pos++];
	} else {
		cout << "[JSON] [ERR] expecting '0123456789'" << endl;			
	}

	if (isCharLookAheadNoWhite ('.', _pos, _str, 1)) {
		jsonString += _str[_pos++];
		
		if (isCharsLookAheadNoWhite ("0123456789", _pos, _str, 1)) {
			jsonString += _str[_pos++];
			while (isCharsLookAheadNoWhite ("0123456789", _pos, _str, 1))
				jsonString += _str[_pos++];
		} else {
			cout << "[JSON] [ERR] expecting '0123456789'" << endl;			
		}
	}

	if (isCharsLookAheadNoWhite ("eE", _pos, _str, 1)) {
		jsonString += _str[_pos++];

		if (isCharsLookAheadNoWhite ("+-", _pos, _str, 1))
			jsonString += _str[_pos++];

		if (isCharsLookAheadNoWhite ("0123456789", _pos, _str, 1)) {
			jsonString += _str[_pos++];
			while (isCharsLookAheadNoWhite ("0123456789", _pos, _str, 1))
				jsonString += _str[_pos++];
		} else {
			cout << "[JSON] [ERR] expecting '0123456789'" << endl;			
		}		
	}

	jsonNumber = atof (jsonString.c_str());

	return new ZjValue(jsonNumber);
}

ZjArray* ZJSON::parseArray (long int& _pos,string& _str) {
	ZjArray *array = new ZjArray();

	if (isCharLookAhead('[', _pos, _str, 1)) {
		isChar ('[',_pos,_str);

		while (!isCharLookAhead(']', _pos, _str, 1)) {
			ZjValue* v = parseValue(_pos, _str);
			
			array->array.push_back (v);

			if (isCharLookAhead(',', _pos, _str, 1)) {
				isChar (',', _pos, _str);
			}
		}

		if (isChar (']', _pos, _str)) {
			return array;
		} else {
			// Array dont end with ]???
		}
	} else {
		// Array didn't start with [ ???
	}
}


ZjValue* ZJSON::parseValue (long int& _pos, string& _str) {
	if (isCharLookAhead ('"', _pos, _str, 1)) {
		return parseString(_pos,_str);
	} else if (isCharsLookAhead ("0123456789-", _pos, _str, 1)) {
		return parseNumber(_pos,_str);
	}
	else if (isCharLookAhead ('{', _pos, _str, 1)) {
		return new ZjValue (parseObject(_pos,_str));
	}
	else if (isCharLookAhead ('[', _pos, _str, 1)) {
		return new ZjValue (parseArray(_pos,_str));
	}
	else if (isCharLookAhead ('t', _pos, _str, 1)) {
		advance(_pos,_str);
		string t = "true";
		if (_str.substr (_pos,4) == t) {
			_pos += 4;
			return new ZjValue (true);
		}
		else {
			// Error, not 'true'
		}
	}
	else if (isCharLookAhead ('f', _pos, _str, 1)) {
		advance(_pos,_str);
		string f = "false";
		if (_str.substr (_pos,5) == f) {
			_pos += 5;
			return new ZjValue (false);
		}
		else {
			// Error, not 'false'
		}
	}	
	else if (isCharLookAhead ('n', _pos, _str, 1)) {
		advance(_pos,_str);
		string f = "null";
		if (_str.substr (_pos,4) == f) {
			_pos += 4;
			return new ZjValue ();
		}
		else {
			// Error, not 'false'
		}
	}	
	else {
		// INVALID
	}
}

bool ZJSON::isChar (char _c, long int& _pos, string& _str) {
	return _c == getNextChar(_pos,_str);
}

bool ZJSON::isCharLookAhead (char _c, long int& _pos, string& _str, int _n) {
	long int pos = _pos;
	char c;

	for (int i=0;i<_n;i++)
		c = getNextChar(pos,_str);

	return _c == c;
}

bool ZJSON::isCharsLookAhead (string _s, long int& _pos, string& _str, int _n) {
	long int pos = _pos;
	char c;

	for (int i=0;i<_n;i++)
		c = getNextChar(pos,_str);

	return any_of(_s.begin(),_s.end(), [=](char _c) {return _c == c;});
}

bool ZJSON::isCharLookAheadNoWhite (char _c, long int& _pos, string& _str, int _n) {
	long int pos = _pos;
	char c;

	for (int i=0;i<_n;i++)
		c = getNextCharNoWhite(pos,_str);

	return _c == c;
}

bool ZJSON::isCharsLookAheadNoWhite (string _s, long int& _pos, string& _str, int _n) {
	long int pos = _pos;
	char c;

	for (int i=0;i<_n;i++)
		c = getNextCharNoWhite(pos,_str);

	return any_of(_s.begin(),_s.end(), [=](char _c) {return _c == c;});
}

char ZJSON::getNextChar (long int& _pos, string& _str) {
	char c;
	do {
		c = _str[_pos++];
	} while (c == ' ' || c == '\t' || c == '\n' || c == '\r');

	return c;
}

char ZJSON::getNextCharNoWhite (long int& _pos, string& _str) {
	char c;
	c = _str[_pos++];
	return c;
}

void ZJSON::advance (long int& _pos, string& _str) {
	char c;
	do {
		c = _str[_pos++];
	} while (c == ' ' || c == '\t' || c == '\n' || c == '\r');
	_pos--;
} 