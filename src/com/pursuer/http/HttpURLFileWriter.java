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

public class HttpURLFileWriter {
    static {
        System.loadLibrary("HttpClient");
    }

    /*
     * Used in C
     */
    @SuppressWarnings("unused")
    private long storageRefId;

    public HttpURLFileWriter(final String url, final String fileName) {
        this.storageRefId = 0;
        JNIGetInit(url, fileName);
    }

    public HttpURLFileWriter(final String url, final String fileName, final String data) {
        this.storageRefId = 0;
        JNIPostInit(url, fileName, data);
    }

    public void close() {
        JNIClose();
    }

    @Override
    protected void finalize() {
        JNIClose();
    }

    public int getResponseCode() {
        return JNIGetResponseCode();
    }

    private native void JNIClose();

    private native void JNIGetInit(String url, String fileName);

    private native int JNIGetResponseCode();

    private native int JNIPerform();

    private native void JNIPostInit(String url, String fileName, String data);

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
