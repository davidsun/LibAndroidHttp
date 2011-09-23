/*******************************************************************************
 * Copyright 2011 Zheng Sun
 *
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 *   http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 ******************************************************************************/
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

