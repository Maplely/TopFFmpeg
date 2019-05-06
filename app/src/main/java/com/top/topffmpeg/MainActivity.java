package com.top.topffmpeg;

import android.Manifest;
import android.os.Build;
import android.os.Bundle;
import android.os.Environment;
import android.support.annotation.RequiresApi;
import android.support.v7.app.AppCompatActivity;
import android.util.Log;
import android.view.View;
import android.widget.Button;

import java.io.File;

public class MainActivity extends AppCompatActivity {

    // Used to load the 'native-lib' library on application startup.
    private static final String TAG = "TTT";
    static {
        System.loadLibrary("ffmpeg-main");
        System.loadLibrary("ffmpeg");
    }

    public static String videoFile = "multi_vertical.mp4";
    public static String videoPath = Environment.getExternalStorageDirectory() +
                                    File.separator +
                                    "fftest" +
                                    File.separator +
                                    videoFile;

    @RequiresApi(api = Build.VERSION_CODES.M)
    @Override
    protected void onCreate(Bundle savedInstanceState) {
        super.onCreate(savedInstanceState);
        setContentView(R.layout.activity_main);

        // Example of a call to a native method
        requestPermissions(new String[]{Manifest.permission.WRITE_EXTERNAL_STORAGE},10);
        Button tv = (Button) findViewById(R.id.sample_text);
        Log.e(TAG, "path:" + videoPath);
        tv.setOnClickListener(new View.OnClickListener() {
            @Override
            public void onClick(View v) {
                stringFromJNI(videoPath);
            }
        });

    }

    /**
     * A native method that is implemented by the 'native-lib' native library,
     * which is packaged with this application.
     */
    public native void stringFromJNI(String path);
}
