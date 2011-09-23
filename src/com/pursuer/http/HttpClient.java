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
