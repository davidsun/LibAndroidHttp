#include <jni.h>
#include <time.h>

#include "curl/curl.h"
#include "EasyCurl.h"

struct ByteConverterNode {
    struct curl_slist *headers;
    CURL *curl;
    char *buffer;
    int bufferLen;
    int curLen;
    long responseCode;
};

static struct ByteConverterNode *getJClassByteConverterNode(JNIEnv *env, jclass this){
    jclass jcClass = (*env) -> GetObjectClass(env, this);
    jfieldID sid = (*env) -> GetFieldID(env, jcClass, "storageRefId", "J");
    return (struct ByteConverterNode *)(*env) -> GetLongField(env, this, sid);
}

static void freeByteConverterNode(JNIEnv *env, jclass this) {
	struct ByteConverterNode *p = getJClassByteConverterNode(env, this);
	if (p) {
        if (p -> buffer)
            free(p -> buffer);
		if (p -> curl)
			curl_easy_cleanup(p -> curl);
        if (p -> headers)
            curl_slist_free_all(p -> headers);
		free(p);
        jclass jcClass = (*env) -> GetObjectClass(env, this);
        jfieldID sid = (*env) -> GetFieldID(env, jcClass, "storageRefId", "J");
        (*env) -> SetLongField(env, this, sid, 0);
	}
}

static struct ByteConverterNode *newByteConverterNode(JNIEnv *env, jclass this, CURL *curl) {
	struct ByteConverterNode *ret = (struct ByteConverterNode *) malloc(sizeof(struct ByteConverterNode));
	ret -> curl = curl;
    ret -> headers = 0;
    ret -> responseCode = 0;
    ret -> buffer = 0;
    ret -> curLen = 0;
    ret -> bufferLen = 0;

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

JNIEXPORT void JNICALL Java_com_pursuer_http_HttpURLByteConverter_JNISetConnectTimeout(JNIEnv *env, jobject this, jint timeout){
    struct ByteConverterNode *node = getJClassByteConverterNode(env, this);
    if (node)
        curl_easy_setopt(node -> curl, CURLOPT_CONNECTTIMEOUT, timeout);
}

JNIEXPORT void JNICALL Java_com_pursuer_http_HttpURLByteConverter_JNIGetInit(JNIEnv *env, jobject this, jstring url){
    const jbyte *pageUrl = (*env) -> GetStringUTFChars(env, url, 0); //Got JAVA String
    CURL *curl = initCurlObject(pageUrl);
    curl_easy_setopt(curl, CURLOPT_HTTPGET, 1);
    (*env) -> ReleaseStringUTFChars(env, url, pageUrl); //Release JAVA String
    newByteConverterNode(env, this, curl);
}

JNIEXPORT void JNICALL Java_com_pursuer_http_HttpURLByteConverter_JNIPostInit(JNIEnv *env, jobject this, jstring url, jstring data){
    const char *pageUrl = (*env)->GetStringUTFChars(env, url, 0); //Got JAVA String
	const char *pageData = (*env)->GetStringUTFChars(env, data, 0); //Got JAVA String
	CURL *curl = initCurlObject(pageUrl);
    curl_easy_setopt(curl, CURLOPT_COPYPOSTFIELDS, pageData);
	(*env)->ReleaseStringUTFChars(env, url, pageUrl); //Release JAVA String
	(*env)->ReleaseStringUTFChars(env, data, pageData); //Release JAVA String
	newByteConverterNode(env, this, curl);
}

JNIEXPORT void JNICALL Java_com_pursuer_http_HttpURLByteConverter_JNISetHeaders(JNIEnv* env, jobject this, jobjectArray headers) {
	struct ByteConverterNode *node = getJClassByteConverterNode(env, this);
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

JNIEXPORT jint JNICALL Java_com_pursuer_http_HttpURLByteConverter_JNIPerform(JNIEnv *env, jobject this){
    struct ByteConverterNode *node = getJClassByteConverterNode(env, this);
    size_t callback(void *ptr, size_t size, size_t nmemb, void *stream){
        size *= nmemb;
        if (size > node -> bufferLen - node -> curLen){
            int newSize = node -> bufferLen << 1;
            if (newSize < 16384)
                newSize = 16384;
            while (size > newSize - node -> curLen){
                newSize <<= 1;
            }
            char *newbuff = realloc(node -> buffer, newSize);
            if (newbuff){
                node -> bufferLen = newSize;
                node -> buffer = newbuff;
            } else {
                size = node -> bufferLen - node -> curLen;
            }
        }
        memcpy(node -> buffer + node -> curLen, ptr, size);
        node -> curLen += size;
        return size;
    }
    if (!node)
        return -1;
    curl_easy_setopt(node -> curl, CURLOPT_WRITEFUNCTION, callback);
    curl_easy_perform(node -> curl);
    curl_easy_getinfo(node -> curl, CURLINFO_RESPONSE_CODE, &(node -> responseCode));
    curl_easy_cleanup(node -> curl);
    node -> curl = 0;
    return node -> responseCode;
}

JNIEXPORT jint JNICALL Java_com_pursuer_http_HttpURLByteConverter_JNIGetResponseCode(JNIEnv *env, jobject this){
    struct ByteConverterNode *node = getJClassByteConverterNode(env, this);
    return (node) ? node -> responseCode : -1;
}

JNIEXPORT jbyteArray JNICALL Java_com_pursuer_http_HttpURLByteConverter_JNIGetData(JNIEnv *env, jobject this){
    struct ByteConverterNode *node = getJClassByteConverterNode(env, this);
    int len = (node) ? (node -> curLen) : (0);
    jbyteArray ret = (*env) -> NewByteArray(env, len);
    if (len > 0)
        (*env) -> SetByteArrayRegion(env, ret, 0, len, (jbyte *) node -> buffer);
    return ret;
}

JNIEXPORT void JNICALL Java_com_pursuer_http_HttpURLByteConverter_JNIClose(JNIEnv *env, jobject this){
    freeByteConverterNode(env, this);
}
