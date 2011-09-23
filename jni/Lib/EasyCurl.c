#include "EasyCurl.h"

#include <jni.h>

jint JNI_OnLoad(JavaVM *vm, void *reserved){
    curl_global_init(CURL_GLOBAL_ALL);
    return JNI_VERSION_1_4;
}

struct curl_slist *setCurlHeaders(CURL *curl, int dataLength, const char **headers){
    if (dataLength == 0)
        return 0;
    struct curl_slist *slist = 0;
	int i;
	for (i = 0; i < dataLength; i++)
		slist = curl_slist_append(slist, headers[i]);
	curl_easy_setopt(curl, CURLOPT_HTTPHEADER, slist);
    return slist;
}

