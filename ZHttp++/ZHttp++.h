#ifndef HTTPPP_H
#define HTTPPP_H

#include <string>
#include <iostream>
#include <stdio.h>
//! Used for http connection
#include <curl/curl.h>

using namespace std;

size_t receiveData (void *, size_t, size_t, void *);

class HTTP {
CURL* curl;
public:
string data;

HTTP ();
~HTTP();

string get(string);
};

#endif /* HTTPPP_H */
