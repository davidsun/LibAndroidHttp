#-------------------------------------------------------------------------------
# Copyright 2011 Zheng Sun
# 
# Licensed under the Apache License, Version 2.0 (the "License");
# you may not use this file except in compliance with the License.
# You may obtain a copy of the License at
# 
#   http://www.apache.org/licenses/LICENSE-2.0
# 
# Unless required by applicable law or agreed to in writing, software
# distributed under the License is distributed on an "AS IS" BASIS,
# WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
# See the License for the specific language governing permissions and
# limitations under the License.
#-------------------------------------------------------------------------------
#include <jni.h>
#include <time.h>

#include "curl/curl.h"
#include "EasyCurl.h"

struct FileWriterNode {
    struct curl_slist *headers;
    char *fileName;
    CURL *curl;
    long responseCode;
};

static struct FileWriterNode *getJClassFileWriterNode(JNIEnv *env, jclass this){
    jclass jcClass = (*env) -> GetObjectClass(env, this);
    jfieldID sid = (*env) -> GetFieldID(env, jcClass, "storageRefId", "J");
    return (struct FileWriterNode *)(*env) -> GetLongField(env, this, sid);
}

static void freeFileWriterNode(JNIEnv *env, jclass this) {
    struct FileWriterNode *p = getJClassFileWriterNode(env, this);
    if (p){
        if (p -> headers)
            curl_slist_free_all(p -> headers);
        if (p -> fileName)
            free(p -> fileName);
		if (p -> curl)
			curl_easy_cleanup(p -> curl);
		free(p);
        jclass jcClass = (*env) -> GetObjectClass(env, this);
        jfieldID sid = (*env) -> GetFieldID(env, jcClass, "storageRefId", "J");
        (*env) -> SetLongField(env, this, sid, 0);
	}
}

static struct FileWriterNode *newFileWriterNode(JNIEnv *env, jclass this, CURL *curl, jstring fileName) {
	struct FileWriterNode *ret = (struct FileWriterNode *) malloc(sizeof(struct FileWriterNode));

    const char *outFileName = (*env) -> GetStringUTFChars(env, fileName, 0);
    int outLen = strlen(outFileName);
    ret -> fileName = malloc(outLen + 1);
    memcpy(ret -> fileName, outFileName, outLen);
    ret -> fileName[outLen] = 0;
    (*env) -> ReleaseStringUTFChars(env, fileName, outFileName);

	ret -> curl = curl;
    ret -> headers = 0;
    ret -> responseCode = 0;

    jclass jcClass = (*env) -> GetObjectClass(env, this);
    jfieldID sid = (*env) -> GetFieldID(env, jcClass, "storageRefId", "J");
    (*env) -> SetLongField(env, this, sid, (long)ret);
	return ret;
}

static CURL *initCurlObject(const char *url){
    CURL *ret = curl_easy_init();
    curl_easy_setopt(ret, CURLOPT_URL, url);
    curl_easy_setopt(ret, CURLOPT_SSL_VERIFYPEER, 0);
    curl_easy_setopt(ret, CURLOPT_ACCEPT_ENCODING, "gzip,deflate");
    curl_easy_setopt(ret, CURLOPT_CONNECTTIMEOUT, 30);
    curl_easy_setopt(ret, CURLOPT_NOSIGNAL, 1);
    return ret;
}

JNIEXPORT void JNICALL Java_com_pursuer_http_HttpURLFileWriter_JNISetConnectTimeout(JNIEnv *env, jobject this, jint timeout){
    struct FileWriterNode *node = getJClassFileWriterNode(env, this);
    if (node)
        curl_easy_setopt(node -> curl, CURLOPT_CONNECTTIMEOUT, timeout);
}

JNIEXPORT void JNICALL Java_com_pursuer_http_HttpURLFileWriter_JNIGetInit(JNIEnv *env, jobject this, jstring url, jstring fileName){
    const jbyte *pageUrl = (*env) -> GetStringUTFChars(env, url, 0); //Got JAVA String
    CURL *curl = initCurlObject(pageUrl);
    curl_easy_setopt(curl, CURLOPT_HTTPGET, 1);
    (*env) -> ReleaseStringUTFChars(env, url, pageUrl); //Release JAVA String
    newFileWriterNode(env, this, curl, fileName);
}

JNIEXPORT void JNICALL Java_com_pursuer_http_HttpURLFileWriter_JNIPostInit(JNIEnv *env, jobject this, jstring url, jstring fileName, jstring data){
    const char *pageUrl = (*env)->GetStringUTFChars(env, url, 0); //Got JAVA String
	const char *pageData = (*env)->GetStringUTFChars(env, data, 0); //Got JAVA String
	CURL *curl = initCurlObject(pageUrl);
    curl_easy_setopt(curl, CURLOPT_COPYPOSTFIELDS, pageData);
	(*env)->ReleaseStringUTFChars(env, url, pageUrl); //Release JAVA String
	(*env)->ReleaseStringUTFChars(env, data, pageData); //Release JAVA String
	newFileWriterNode(env, this, curl, fileName);
}

JNIEXPORT void JNICALL Java_com_pursuer_http_HttpURLFileWriter_JNISetHeaders(JNIEnv* env, jobject this, jobjectArray headers) {
	struct FileWriterNode *node = getJClassFileWriterNode(env, this);
	if (!node)
		return;
	int len = (*env)->GetArrayLength(env, headers);
	const char **realHeaders = (const char **) malloc(sizeof(char *) * len); //Allocate
	int i;
	for (i = 0; i < len; i++) {
		jstring jsHeader = (jstring)(*env)->GetObjectArrayElement(env, headers, i);
		realHeaders[i] = (*env)->GetStringUTFChars(env, jsHeader, 0); //Got JAVA String
	}
    node -> headers = setCurlHeaders(node -> curl, len, realHeaders);
	for (i = 0; i < len; i++) {
		jstring jsHeader = (jstring)(*env)->GetObjectArrayElement(env, headers, i);
		(*env)->ReleaseStringUTFChars(env, jsHeader, realHeaders[i]); //Release JAVA String
	}
	free(realHeaders); //Free
}

JNIEXPORT jint JNICALL Java_com_pursuer_http_HttpURLFileWriter_JNIPerform(JNIEnv *env, jobject this){
    struct FileWriterNode *node = getJClassFileWriterNode(env, this);
    if (!node)
        return -1;
    FILE *fout = fopen(node -> fileName, "w");
    if (!fout)
        return -1;
    size_t callback(void *ptr, size_t size, size_t nmemb, void *stream){
        fwrite(ptr, size, nmemb, fout);
        return size * nmemb;
    }
    curl_easy_setopt(node -> curl, CURLOPT_WRITEFUNCTION, callback);
    curl_easy_perform(node -> curl);
    curl_easy_getinfo(node -> curl, CURLINFO_RESPONSE_CODE, &(node -> responseCode));
    curl_easy_cleanup(node -> curl);
    node -> curl = 0;
    fclose(fout);
    return node -> responseCode;
}

JNIEXPORT jint JNICALL Java_com_pursuer_http_HttpURLFileWriter_JNIGetResponseCode(JNIEnv *env, jobject this){
    struct FileWriterNode *node = getJClassFileWriterNode(env, this);
    return (node) ? node -> responseCode : -1;
}

JNIEXPORT void JNICALL Java_com_pursuer_http_HttpURLFileWriter_JNIClose(JNIEnv *env, jobject this){
    freeFileWriterNode(env, this);
}
