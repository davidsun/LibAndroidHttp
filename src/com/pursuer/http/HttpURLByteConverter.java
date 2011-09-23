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

import java.util.List;

public class HttpURLByteConverter {
    static {
        System.loadLibrary("HttpClient");
    }

    /*
     * Used in C
     */
    @SuppressWarnings("unused")
    private long storageRefId;

    public HttpURLByteConverter(final String url) {
        this.storageRefId = 0;
        JNIGetInit(url);
    }

    public HttpURLByteConverter(final String url, final String data) {
        this.storageRefId = 0;
        JNIPostInit(url, data);
    }

    public void close() {
        JNIClose();
    }

    @Override
    protected void finalize() {
        JNIClose();
    }

    public byte[] getData() {
        return JNIGetData();
    }

    public int getResponseCode() {
        return JNIGetResponseCode();
    }

    private native void JNIClose();

    private native byte[] JNIGetData();

    private native void JNIGetInit(String url);

    private native int JNIGetResponseCode();

    private native int JNIPerform();

    private native void JNIPostInit(String url, String data);

    private native void JNISetConnectTimeout(int timeout);

    private native int JNISetHeaders(String[] headers);

    public int perform() {
        return JNIPerform();
    }

    public void setConnectTimeout(int timeout) {
        JNISetConnectTimeout(timeout);
    }

    public void setHeaders(final List<String> headers) {
        JNISetHeaders((String[]) headers.toArray());
    }

    public void setHeaders(String[] headers) {
        JNISetHeaders(headers);
    }
}
