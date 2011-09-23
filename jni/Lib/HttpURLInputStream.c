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

#include "URLFile.h"

struct InputStreamNode {
	URL_FILE *file;
};

static struct InputStreamNode *getJClassInputStreamNode(JNIEnv *env, jclass this) {
    jclass jcClass = (*env) -> GetObjectClass(env, this);
    jfieldID sid = (*env) -> GetFieldID(env, jcClass, "storageRefId", "J");
    return (struct InputStreamNode *)(*env) -> GetLongField(env, this, sid);
}

static void freeInputStreamNode(JNIEnv *env, jclass this) {
	struct InputStreamNode *p = getJClassInputStreamNode(env, this);
	if (p) {
		if (p -> file)
			URL_file_fclose(p -> file);
		free(p);
        jclass jcClass = (*env) -> GetObjectClass(env, this);
        jfieldID sid = (*env) -> GetFieldID(env, jcClass, "storageRefId", "J");
        (*env) -> SetLongField(env, this, sid, 0); 
	}
}

static struct InputStreamNode *newInputStreamNode(JNIEnv *env, jobject this, URL_FILE *file) {
	struct InputStreamNode *ret = (struct InputStreamNode *) malloc(sizeof(struct InputStreamNode));
	ret -> file = file;
    jclass jcClass = (*env) -> GetObjectClass(env, this);
    jfieldID sid = (*env) -> GetFieldID(env, jcClass, "storageRefId", "J");
    (*env) -> SetLongField(env, this, sid, (long)ret);
	return ret;
}

JNIEXPORT void JNICALL Java_com_pursuer_http_HttpURLInputStream_JNIGetInit(JNIEnv* env, jobject this, jstring url) {
	const jbyte *pageUrl = (*env)->GetStringUTFChars(env, url, 0);
	URL_FILE *fGet = URL_file_fopen_get(pageUrl);
	(*env)->ReleaseStringUTFChars(env, url, pageUrl);
	newInputStreamNode(env, this, fGet);
}

JNIEXPORT void JNICALL Java_com_pursuer_http_HttpURLInputStream_JNIPostInit(JNIEnv* env, jobject this, jstring url, jstring data) {
	const jbyte *pageUrl = (*env)->GetStringUTFChars(env, url, 0); //Got JAVA String
	const jbyte *pageData = (*env)->GetStringUTFChars(env, data, 0); //Got JAVA String
	URL_FILE *fPost = URL_file_fopen_post(pageUrl, pageData);
	(*env)->ReleaseStringUTFChars(env, url, pageUrl); //Release JAVA String
	(*env)->ReleaseStringUTFChars(env, url, pageData); //Release JAVA String
	newInputStreamNode(env, this, fPost);
}

JNIEXPORT void JNICALL Java_com_pursuer_http_HttpURLInputStream_JNISetHeaders(JNIEnv* env, jobject this, jobjectArray headers) {
	struct InputStreamNode *node = getJClassInputStreamNode(env, this);
	if (!node)
		return;
	int len = (*env)->GetArrayLength(env, headers);
	const char **realHeaders = (const char **) malloc(sizeof(char *) * len); //Allocate
	int i;
	for (i = 0; i < len; i++) {
		jstring jsHeader = (jstring)(*env)->GetObjectArrayElement(env, headers, i);
		realHeaders[i] = (*env)->GetStringUTFChars(env, jsHeader, 0); //Got JAVA String
	}
	URL_file_setHeaders(node -> file, len, realHeaders);
	for (i = 0; i < len; i++) {
		jstring jsHeader = (jstring)(*env)->GetObjectArrayElement(env, headers, i);
		(*env)->ReleaseStringUTFChars(env, jsHeader, realHeaders[i]); //Release JAVA String
	}
	free(realHeaders); //Free
}

JNIEXPORT void JNICALL Java_com_pursuer_http_HttpURLInputStream_JNIConnect(JNIEnv* env, jobject this) {
	struct InputStreamNode *node = getJClassInputStreamNode(env, this);
	if (node)
		URL_file_connect(node -> file);
}

JNIEXPORT jint JNICALL Java_com_pursuer_http_HttpURLInputStream_JNIGetResponseCode(JNIEnv* env, jobject this) {
	struct InputStreamNode *node = getJClassInputStreamNode(env, this);
	long ret = 0;
	if (node) {
		while (ret == 0 && node -> file -> still_running) {
            URL_file_doWait(node -> file);
			curl_easy_getinfo(node -> file -> curl, CURLINFO_RESPONSE_CODE, &ret);
		}
	}
	return ret;
}

JNIEXPORT void JNICALL Java_com_pursuer_http_HttpURLInputStream_JNISetConnectTimeout(JNIEnv* env, jobject this, jint timeout) {
	struct InputStreamNode *node = getJClassInputStreamNode(env, this);
	if (node) {
		URL_file_setConnectTimeout(node -> file, timeout);
	}
}

JNIEXPORT void JNICALL Java_com_pursuer_http_HttpURLInputStream_JNISetReadTimeout(JNIEnv* env, jobject this, jint timeout) {
	struct InputStreamNode *node = getJClassInputStreamNode(env, this);
	if (node) {
		URL_file_setReadTimeout(node -> file, timeout);
	}
}

JNIEXPORT jint JNICALL Java_com_pursuer_http_HttpURLInputStream_JNIReadBytes(JNIEnv* env, jobject this, jbyteArray buffer, jint offset,
		jint len) {
	struct InputStreamNode *node = getJClassInputStreamNode(env, this);
	if (!node)
		return EOF;
	void callback(void *ptr, size_t size) {
		if (size > 0)
			(*env)->SetByteArrayRegion(env, buffer, offset, size, (jbyte *) ptr);
	}
	int ret = URL_file_fread_with_callback(len, node -> file, &callback);
	return ret;
}

JNIEXPORT void JNICALL Java_com_pursuer_http_HttpURLInputStream_JNIClose(JNIEnv* env, jobject this) {
    freeInputStreamNode(env, this);
}

JNIEXPORT jint JNICALL Java_com_pursuer_http_HttpURLInputStream_JNIReadByte(JNIEnv* env, jobject this){
    struct InputStreamNode *node = getJClassInputStreamNode(env, this);
    return (node) ? (URL_file_fread_char(node -> file)) : (EOF);
}


