#ifndef EASY_CURL_H
#define EASY_CURL_H

#include "curl/curl.h"

void globalInit();

struct curl_slist *setCurlHeaders(CURL *curl, int dataLength, const char **headers);

#endif
