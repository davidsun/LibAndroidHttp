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
package com.pursuer.http;

final public class HttpClient {
    public static HttpURLByteConverter doGetByteConverter(final String url) {
        final HttpURLByteConverter ret = new HttpURLByteConverter(url);
        ret.setConnectTimeout(30);
        ret.perform();
        return ret;
    }

    public static HttpURLByteConverter doGetByteConverter(final String url, final String[] headers) {
        final HttpURLByteConverter ret = new HttpURLByteConverter(url);
        ret.setConnectTimeout(30);
        ret.setHeaders(headers);
        ret.perform();
        return ret;
    }

    public static HttpURLFileWriter doGetFileWriter(final String url, final String fileName) {
        final HttpURLFileWriter ret = new HttpURLFileWriter(url, fileName);
        ret.setConnectTimeout(30);
        ret.perform();
        return ret;
    }

    public static HttpURLFileWriter doGetFileWriter(final String url, final String fileName, final String[] headers) {
        final HttpURLFileWriter ret = new HttpURLFileWriter(url, fileName);
        ret.setConnectTimeout(30);
        ret.setHeaders(headers);
        ret.perform();
        return ret;
    }

    public static HttpURLInputStream doGetStream(final String url) {
        final HttpURLInputStream ret = new HttpURLInputStream(url);
        ret.setConnectTimeout(30);
        ret.setReadTimeout(20);
        ret.connect();
        return ret;
    }

    public static HttpURLInputStream doGetStream(final String url, final String[] headers) {
        final HttpURLInputStream ret = new HttpURLInputStream(url);
        ret.setConnectTimeout(30);
        ret.setReadTimeout(20);
        ret.setHeaders(headers);
        ret.connect();
        return ret;
    }

    public static HttpURLByteConverter doPostByteConverter(final String url, final String postData) {
        final HttpURLByteConverter ret = new HttpURLByteConverter(url, postData);
        ret.setConnectTimeout(30);
        ret.perform();
        return ret;
    }

    public static HttpURLByteConverter doPostByteConverter(final String url, final String postData,
            final String[] headers) {
        final HttpURLByteConverter ret = new HttpURLByteConverter(url, postData);
        ret.setConnectTimeout(30);
        ret.setHeaders(headers);
        ret.perform();
        return ret;
    }

    public static HttpURLFileWriter doPostFileWriter(final String url, final String fileName, final String postData) {
        final HttpURLFileWriter ret = new HttpURLFileWriter(url, fileName, postData);
        ret.setConnectTimeout(30);
        ret.perform();
        return ret;
    }

    public static HttpURLFileWriter doPostFileWriter(final String url, final String fileName, final String postData,
            final String[] headers) {
        final HttpURLFileWriter ret = new HttpURLFileWriter(url, fileName, postData);
        ret.setConnectTimeout(30);
        ret.setHeaders(headers);
        ret.perform();
        return ret;
    }

    public static HttpURLInputStream doPostStream(final String url, final String postData) {
        final HttpURLInputStream ret = new HttpURLInputStream(url, postData);
        ret.setConnectTimeout(30);
        ret.setReadTimeout(20);
        ret.connect();
        return ret;
    }

    public static HttpURLInputStream doPostStream(final String url, final String postData, final String[] headers) {
        final HttpURLInputStream ret = new HttpURLInputStream(url, postData);
        ret.setConnectTimeout(30);
        ret.setReadTimeout(20);
        ret.setHeaders(headers);
        ret.connect();
        return ret;
    }
}
