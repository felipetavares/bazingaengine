#include "http++.h"

HTTP::HTTP ():
  curl (NULL) {
  curl = curl_easy_init();
  if (curl) {
    curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, receiveData);
    curl_easy_setopt(curl, CURLOPT_WRITEDATA, this);
  }
}

HTTP::~HTTP () {
  if (curl) {
    curl_easy_cleanup(curl);
  }
}

string HTTP::get (string url) {
  CURLcode res;
  
  if (!curl)
    return "";
  
  curl_easy_setopt(curl, CURLOPT_URL, url.c_str());
  curl_easy_setopt(curl, CURLOPT_FOLLOWLOCATION, 1L);

  data = "";
  
  res = curl_easy_perform(curl);
  
  if(res != CURLE_OK)
    fprintf(stderr, "curl_easy_perform() failed: %s\n",
	    curl_easy_strerror(res));
  
  return this->data;
}

size_t receiveData (void *buffer, size_t size, size_t nmemb, void *userp) {
  HTTP *http = (HTTP*)userp;
  size_t realSize = size*nmemb;

  for (int i=0;i<realSize;i++)
    http->data += ((char*)buffer)[i];

  return realSize;
}
