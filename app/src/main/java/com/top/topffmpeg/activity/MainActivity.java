package com.top.topffmpeg.activity;

import android.Manifest;
import android.graphics.PixelFormat;
import android.media.AudioFormat;
import android.media.AudioManager;
import android.media.AudioTrack;
import android.os.Build;
import android.os.Bundle;
import android.os.Environment;
import android.support.annotation.RequiresApi;
import android.support.v7.app.AppCompatActivity;
import android.util.Log;
import android.view.Surface;
import android.view.SurfaceView;
import android.view.View;
import android.widget.Button;
import com.top.topffmpeg.R;

public class MainActivity extends AppCompatActivity implements View.OnClickListener {
    static {
        System.loadLibrary("ffmpeg-main");
    }

    private static final String TAG = "TTT";
    private SurfaceView surfaceView;
    private String videPath;
    private AudioTrack audioTrack;

    native void playVideo(String path, Surface surface);
    native void palySound(String path, AudioTrack track);
    @RequiresApi(api = Build.VERSION_CODES.M)
    @Override
    protected void onCreate(Bundle savedInstanceState) {
        super.onCreate(savedInstanceState);
        setContentView(R.layout.activity_main);
        initVIews();
        initDatas();
        initAudioTrack();
        requestPermissions(new String[]{Manifest.permission.WRITE_EXTERNAL_STORAGE},222);
    }

    private void initAudioTrack() {
        int format = AudioFormat.ENCODING_PCM_16BIT;
        int channel = AudioFormat.CHANNEL_IN_STEREO;
        int minBufferSize = AudioTrack.getMinBufferSize(44100, channel, format);
        audioTrack = new AudioTrack(AudioManager.STREAM_MUSIC, 44100, channel, format, minBufferSize, AudioTrack.MODE_STREAM);

    }

    private void initDatas() {
        videPath= Environment.getExternalStorageDirectory()+"/fftest/multi_vertical.mp4";
    }

    private void initVIews() {
        ((Button) findViewById(R.id.videoBt)).setOnClickListener(this);
        ((Button) findViewById(R.id.soundBt)).setOnClickListener(this);
        surfaceView = (SurfaceView) findViewById(R.id.surface);
        surfaceView.getHolder().setFormat(PixelFormat.RGBA_8888);
    }

    @Override
    public void onClick(View v) {
        int id = v.getId();
        switch (id) {
            case R.id.videoBt:
                playVideo(videPath,surfaceView.getHolder().getSurface());
                break;
            case R.id.soundBt:
                palySound(videPath,audioTrack);
                break;
            default:
                Log.e(TAG, "没有找到id:" + id + "的控件");
                break;
        }
    }
}
