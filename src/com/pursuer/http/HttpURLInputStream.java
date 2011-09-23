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

import java.io.IOException;
import java.io.InputStream;
import java.util.List;

public class HttpURLInputStream extends InputStream {
    static {
        System.loadLibrary("HttpClient");
    }

    /*
     * Used in C
     */
    @SuppressWarnings("unused")
    private long storageRefId;

    public HttpURLInputStream(final String url) {
        this.storageRefId = 0;
        JNIGetInit(url);
    }

    public HttpURLInputStream(final String url, final String postData) {
        this.storageRefId = 0;
        JNIPostInit(url, postData);
    }

    @Override
    public void close() throws IOException {
        JNIClose();
    }

    public void connect() {
        JNIConnect();
    }

    @Override
    protected void finalize() {
        JNIClose();
    }

    public int getResponseCode() {
        return JNIGetResponseCode();
    }

    private native void JNIClose();

    private native void JNIConnect();

    private native void JNIGetInit(String url);

    private native int JNIGetResponseCode();

    private native void JNIPostInit(String url, String data);

    private native int JNIReadByte();

    private native int JNIReadBytes(byte buffer[], int offset, int len);

    private native void JNISetConnectTimeout(int timeout);

    private native int JNISetHeaders(String[] headers);

    private native void JNISetReadTimeout(int timeout);

    @Override
    public int read() throws IOException {
        return JNIReadByte();
    }

    @Override
    public int read(final byte[] buffer, final int offset, final int len) throws IOException {
        return JNIReadBytes(buffer, offset, len);
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

    public void setReadTimeout(int timeout) {
        JNISetReadTimeout(timeout);
    }
}
