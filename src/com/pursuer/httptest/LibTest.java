package com.pursuer.httptest;

import java.io.File;
import java.io.IOException;
import java.io.InputStream;
import java.net.HttpURLConnection;
import java.net.URL;
import java.util.ArrayList;

import com.pursuer.http.HttpClient;
import com.pursuer.http.HttpURLByteConverter;
import com.pursuer.http.HttpURLFileWriter;
import com.pursuer.http.HttpURLInputStream;
import com.pursuer.http.R;

import android.app.Activity;
import android.os.Bundle;
import android.util.Log;
import android.widget.TextView;

public class LibTest extends Activity {
    final private static String page = "http://www.fudan.edu.cn";

    /** Called when the activity is first created. */
    @Override
    public void onCreate(Bundle savedInstanceState) {
        super.onCreate(savedInstanceState);
        setContentView(R.layout.main);
        TextView tv = new TextView(this);

        {
            Log.w("start", "p1");
            long time = System.currentTimeMillis();
            int len = 0;
            try {
                URL url = new URL(page);
                HttpURLConnection conn = (HttpURLConnection) url.openConnection();
                conn.setUseCaches(false);
                conn.connect();
                InputStream s = conn.getInputStream();
                byte buffer[] = new byte[8192];
                int count;
                while ((count = s.read(buffer, 0, 8192)) != -1) {
                    len += count;
                }
            } catch (IOException e) {
                // TODO Auto-generated catch block
                e.printStackTrace();
            }
            Log.w("end", "p1 Time: " + String.valueOf(System.currentTimeMillis() - time) + ", Len: " + len);
        }

        {
            Log.w("start", "p2");
            long time = System.currentTimeMillis();

            HttpURLByteConverter s = new HttpURLByteConverter(page);
            s.setConnectTimeout(5);
            int state = s.perform();
            int len = s.getData().length;
            Log.w("end", "p2 Time: " + String.valueOf(System.currentTimeMillis() - time) + ", Len: " + len + "State: "
                    + state);
        }

        {
            ArrayList<Thread> ts = new ArrayList<Thread>();
            for (int i = 0; i < 200; i++) {
                final int x = i;
                final Thread t = new Thread() {
                    @Override
                    public void run() {
                        for (int i = 0; i < 5; i++) {
                            try {
                                Thread.sleep((int) Math.random() * 100000);
                            } catch (InterruptedException e) {
                                e.printStackTrace();
                            }
                            final String path = android.os.Environment.getExternalStorageDirectory().getAbsolutePath()
                                    + File.separator + x + ".out";
                            Log.w("start", String.valueOf(x));
                            HttpURLFileWriter s = HttpClient.doGetFileWriter(page, path);
                            Log.w("end", String.valueOf(x) + " " + String.valueOf(i) + " state:" + s.getResponseCode());
                            s.close();
                        }
                    }
                };
                t.start();
                ts.add(t);
            }
            for (int i = 0; i < 200; i++) {
                try {
                    ts.get(i).join();
                } catch (InterruptedException e) {
                    // TODO Auto-generated catch block
                    e.printStackTrace();
                }
            }
        }

        {
            ArrayList<Thread> ts = new ArrayList<Thread>();
            for (int i = 0; i < 50; i++) {
                final int x = i;
                final Thread t = new Thread() {
                    @Override
                    public void run() {
                        try {
                            Thread.sleep((long) (Math.random() * 1000));
                        } catch (InterruptedException e1) {
                            // TODO Auto-generated catch block
                            e1.printStackTrace();
                        }
                        for (int i = 0; i < 500; i++) {
                            Log.w("start", String.valueOf(x) + " " + String.valueOf(i));
                            int state;
                            if (x % 3 < 3) {
                                HttpURLInputStream s = HttpClient.doGetStream(page);
                                state = s.getResponseCode();
                                int len = 0;
                                int count;
                                byte buffer[] = new byte[8192];
                                try {
                                    while ((count = s.read(buffer, 0, 8192)) != -1) {
                                        len += count;
                                    }
                                } catch (IOException e) {
                                    // TODO Auto-generated catch block
                                    e.printStackTrace();
                                }
                            } else if (x % 3 < -3) {
                                final String path = android.os.Environment.getExternalStorageDirectory()
                                        .getAbsolutePath()
                                        + File.separator + x + ".out";
                                HttpURLFileWriter s = HttpClient.doGetFileWriter(page, path);
                                state = s.getResponseCode();
                                s.close();
                            } else {
                                HttpURLByteConverter s = HttpClient.doGetByteConverter(page);
                                state = s.perform();
                                s.close();
                            }
                            Log.w("end", String.valueOf(x) + " " + String.valueOf(i) + " state:" + state);
                        }
                    }
                };
                t.start();
                ts.add(t);
            }
            for (Thread t : ts) {
                try {
                    t.join();
                } catch (InterruptedException e) {
                    // TODO Auto-generated catch block
                    e.printStackTrace();
                }
            }

        }

        setContentView(tv);
    }
}
