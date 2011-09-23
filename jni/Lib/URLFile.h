#ifndef HTTP_CLIENT_H
#define HTTP_CLIENT_H

#include "curl/curl.h"

enum fcurl_state {
	READY = 0, CONNECTED = 1
};

struct fcurl_data {
	enum fcurl_state state;

	CURL *curl;
	char *buffer;
	size_t buffer_len;
	size_t buffer_pos;
	int still_running;
	int readTimeout;
	struct curl_slist *headers;
	CURLM *multi_handle;
};

typedef struct fcurl_data URL_FILE;
typedef enum fcurl_state URL_FILE_STATE;

void URL_file_doWait(URL_FILE *file);
void URL_file_setConnectTimeout(URL_FILE *file, int timeout);
void URL_file_setReadTimeout(URL_FILE *file, int timeout);
void URL_file_connect(URL_FILE *file);
URL_FILE *URL_file_fopen_post(const char *url, const char *data);
URL_FILE *URL_file_fopen_get(const char *url);
int URL_file_fclose(URL_FILE *file);
int URL_file_feof(URL_FILE *file);
int URL_file_fread_char(URL_FILE *file);
size_t URL_file_fread(void *ptr, size_t size, URL_FILE *file);
size_t URL_file_fread_with_callback(size_t size, URL_FILE *file, void(*callback)(void *ptr, size_t size));
void URL_file_rewind(URL_FILE *file);
void URL_file_setHeaders(URL_FILE *file, int dataLength, const char **headers);

#endif
