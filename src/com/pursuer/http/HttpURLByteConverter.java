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
