package com.top.topffmpeg.activity;

import android.app.Activity;
import android.graphics.PixelFormat;
import android.os.Bundle;
import android.os.Environment;
import android.support.annotation.Nullable;
import android.util.Log;
import android.view.Surface;
import android.view.SurfaceView;
import android.view.View;

import com.top.topffmpeg.R;
import java.io.File;

/**
 * Description:SecondActivity
 *
 * @author lihaitao
 * @version 1.0
 * @since 2019/5/20 4:00 PM
 */
public class SecondActivity extends Activity {
    static {
        System.loadLibrary("ffmpeg-main");
        System.loadLibrary("ffmpeg");
    }

    private static String videoFile = "multi_vertical.mp4";
    private static final String TAG = "TTT";
    public static String videoPath = Environment.getExternalStorageDirectory() +
            File.separator +
            "fftest" +
            File.separator +
            videoFile;

    @Override
    protected void onCreate(@Nullable Bundle savedInstanceState) {
        super.onCreate(savedInstanceState);
        setContentView(R.layout.activity_second);
        final SurfaceView surfaceView = (SurfaceView) findViewById(R.id.surface);
        surfaceView.getHolder().setFormat(PixelFormat.RGBA_8888);
        findViewById(R.id.bt_start).setOnClickListener(new View.OnClickListener() {
            @Override
            public void onClick(View v) {
                Log.e(TAG, videoPath);
                playVideo(videoPath,surfaceView.getHolder().getSurface());
            }
        });

    }

    public native void playVideo(String path,Surface view);

}
