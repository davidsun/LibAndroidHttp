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
#include <jni.h>
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <errno.h>
#include <time.h>

#include "curl_config.h"
#include "curl/curl.h"
#include "EasyCurl.h"
#include "URLFile.h"

void URL_file_setConnectTimeout(URL_FILE *file, int timeout) {
	if (file -> state == READY) {
		curl_easy_setopt(file -> curl, CURLOPT_CONNECTTIMEOUT, timeout);
	}
}
void URL_file_setReadTimeout(URL_FILE *file, int timeout) {
	if (file -> state == READY) {
		file -> readTimeout = timeout;
	}
}

static size_t URL_file_writeCallback(char *buffer, size_t size, size_t nitems, void *userp) {
	URL_FILE *url = (URL_FILE *) userp;
	size *= nitems;

	size_t rembuff = url->buffer_len - url->buffer_pos;
	if (size > rembuff) {
		int newSize = url->buffer_len << 1;
		if (newSize < 16384)
			newSize = 16384;
		while (size > newSize - url->buffer_pos) {
			newSize <<= 1;
		}
		char *newbuff = realloc(url->buffer, newSize);
		if (newbuff) {
			url->buffer_len = newSize;
			url->buffer = newbuff;
		} else {
			size = rembuff;
		}
	}

	memcpy(url->buffer + url->buffer_pos, buffer, size);
	url->buffer_pos += size;

	return size;
}

static URL_FILE *URL_file_init(const char *url) {
	URL_FILE *file = malloc(sizeof(URL_FILE));
    file -> state = READY;
    file -> buffer = 0;
    file -> buffer_len = 0;
    file -> buffer_pos = 0;
    file -> still_running = 0;
    file -> headers = 0;
    file -> multi_handle = 0;
    file -> readTimeout = 30;

	file -> curl = curl_easy_init();
	curl_easy_setopt(file->curl, CURLOPT_URL, url);
	curl_easy_setopt(file->curl, CURLOPT_WRITEDATA, file);
	curl_easy_setopt(file->curl, CURLOPT_SSL_VERIFYPEER, 0);
	curl_easy_setopt(file->curl, CURLOPT_ACCEPT_ENCODING, "gzip,deflate");
    curl_easy_setopt(file->curl, CURLOPT_CONNECTTIMEOUT, 30);
	curl_easy_setopt(file->curl, CURLOPT_WRITEFUNCTION, URL_file_writeCallback);
    curl_easy_setopt(file->curl, CURLOPT_NOSIGNAL, 1);
	return file;
}

/* use to attempt to fill the read buffer up to requested number of bytes */
static int URL_file_fill_buffer(URL_FILE *file, size_t want) {
	if (!file->still_running || file->buffer_pos > want) {
		return 0;
	}
	long endwait = clock() + file -> readTimeout * CLOCKS_PER_SEC;
	do {
		URL_file_doWait(file);
	} while (file->still_running && file->buffer_pos < want && clock() < endwait);
	return 1;
}

/* use to remove want bytes from the front of a files buffer */
static int URL_file_use_buffer(URL_FILE *file, int want) {
	if (want == 0)
		return 0;
	if (file->buffer_pos <= want) {
		if (file -> buffer){
			free(file -> buffer);
    		file->buffer = 0;
        }
		file -> buffer_pos = 0;
		file -> buffer_len = 0;
	} else {
		memmove(file->buffer, file->buffer + want, file->buffer_pos - want);
		file->buffer_pos -= want;
	}
	return 0;
}

void URL_file_doWait(URL_FILE *file){
    int maxfd = -1;
	long curl_timeo = -1;

    fd_set fdread;
	fd_set fdwrite;
	fd_set fdexcep;

	FD_ZERO(&fdread);
	FD_ZERO(&fdwrite);
	FD_ZERO(&fdexcep);

	struct timeval timeout;
	timeout.tv_sec = 20;
	timeout.tv_usec = 0;

	curl_multi_timeout(file->multi_handle, &curl_timeo);
	if (curl_timeo >= 0) {
		timeout.tv_sec = curl_timeo / 1000;
		if (timeout.tv_sec > 1)
			timeout.tv_sec = 1;
		else
			timeout.tv_usec = (curl_timeo % 1000) * 1000;
	}
	curl_multi_fdset(file->multi_handle, &fdread, &fdwrite, &fdexcep, &maxfd);
	int rc = select(maxfd + 1, &fdread, &fdwrite, &fdexcep, &timeout);

	switch (rc) {
	case -1:
		break;
	case 0:
	default:
		curl_multi_perform(file->multi_handle, &file->still_running);
		break;
    }
}

URL_FILE *URL_file_fopen_get(const char *url) {
	URL_FILE *file = URL_file_init(url);
	if (file)
		curl_easy_setopt(file->curl, CURLOPT_HTTPGET, 1);
	return file;
}

URL_FILE *URL_file_fopen_post(const char *url, const char *data) {
	URL_FILE *file = URL_file_init(url);
	curl_easy_setopt(file->curl, CURLOPT_COPYPOSTFIELDS, data);
	return file;
}

void URL_file_setHeaders(URL_FILE *file, int dataLength, const char **headers) {
    file -> headers = setCurlHeaders(file -> curl, dataLength, headers);
}

void URL_file_connect(URL_FILE *file) {
	if (file -> state == READY) {
		file->multi_handle = curl_multi_init();
		curl_multi_add_handle(file -> multi_handle, file -> curl);
		curl_multi_perform(file -> multi_handle, &file -> still_running);
		file -> state = CONNECTED;
	}
}

int URL_file_fclose(URL_FILE *file) {
	curl_multi_remove_handle(file -> multi_handle, file -> curl);
    if (file -> curl)
    	curl_easy_cleanup(file->curl);
	if (file->buffer)
		free(file->buffer);
	if (file->headers)
		curl_slist_free_all(file->headers);
	if (file->multi_handle)
		curl_multi_cleanup(file->multi_handle);
	free(file);
	return 0;
}

int URL_file_feof(URL_FILE *file) {
	return (file->buffer_pos == 0) && (!file->still_running);
}

int URL_file_fread_char(URL_FILE *file){
    if (file -> state == READY)
        return 0;
    URL_file_fill_buffer(file, 1);
    if (!file -> buffer_pos)
        return EOF;
    int ret = file -> buffer[0];
    URL_file_use_buffer(file, 1);
    return ret;
}

size_t URL_file_fread(void *ptr, size_t size, URL_FILE *file) {
	if (file -> state == READY)
		return 0;
	URL_file_fill_buffer(file, size);
	if (!file->buffer_pos)
		return EOF;
	if (file->buffer_pos < size)
		size = file->buffer_pos;
	if (size) {
		memcpy(ptr, file->buffer, size);
		URL_file_use_buffer(file, size);
	}
	return size;
}

size_t URL_file_fread_with_callback(size_t size, URL_FILE *file, void(*callback)(void *ptr, size_t size)) {
	if (file -> state == READY)
		return 0;
	URL_file_fill_buffer(file, size);
	if (!file -> buffer_pos) 
		return EOF;
	if (file -> buffer_pos < size)
		size = file -> buffer_pos;
	if (callback)
		(*callback)(file -> buffer, size);
	URL_file_use_buffer(file, size);
	return size;
}

void URL_file_rewind(URL_FILE *file) {
	curl_multi_remove_handle(file->multi_handle, file->curl);
	curl_multi_add_handle(file->multi_handle, file->curl);
	if (file->buffer){
		free(file->buffer);
    	file->buffer = 0;
    }
	file->buffer_pos = 0;
	file->buffer_len = 0;
}
