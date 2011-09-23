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
