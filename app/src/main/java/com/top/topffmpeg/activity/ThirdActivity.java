package com.top.topffmpeg.activity;

import android.media.MediaPlayer;
import android.net.Uri;
import android.os.Bundle;
import android.support.annotation.Nullable;
import android.support.v7.app.AppCompatActivity;
import android.util.Log;
import android.view.SurfaceView;
import android.view.View;

import com.top.topffmpeg.R;
import java.io.IOException;

/**
 * Description:ThirdActivity
 *
 * @author lihaitao
 * @version 1.0
 * @since 2019/5/28 3:46 PM
 */
public class ThirdActivity extends AppCompatActivity {
    public static final String url = "http://clips.vorwaerts-gmbh.de/big_buck_bunny.mp4";
    private static final String TAG = "TTT_media";
    private MediaPlayer mMediaPlayer;

    @Override
    protected void onCreate(@Nullable Bundle savedInstanceState) {
        super.onCreate(savedInstanceState);
        setContentView(R.layout.activity_third);
        final SurfaceView view = (SurfaceView) findViewById(R.id.surface);
        findViewById(R.id.bt_play).setOnClickListener(new View.OnClickListener() {
            @Override
            public void onClick(View v) {
                mMediaPlayer.start();

            }
        });
        mMediaPlayer = new MediaPlayer();
        try {
            mMediaPlayer.setDataSource(this, Uri.parse(url));
            mMediaPlayer.setOnPreparedListener(new MediaPlayer.OnPreparedListener() {
                @Override
                public void onPrepared(MediaPlayer mp) {
                    Log.e(TAG, "surfaceview准备完成\"");
                    mMediaPlayer.setSurface(view.getHolder().getSurface());
                    Log.e(TAG, "durition:"+mMediaPlayer.getDuration());

                }

            });
            mMediaPlayer.setOnErrorListener(new MediaPlayer.OnErrorListener() {
                @Override
                public boolean onError(MediaPlayer mp, int what, int extra) {
                    Log.e(TAG, "what:" + what + "extra:" + extra);
                    return false;
                }
            });
            mMediaPlayer.prepareAsync();

        } catch (IOException e) {
            e.printStackTrace();
        }
    }

    @Override
    protected void onDestroy() {
        super.onDestroy();
        if (mMediaPlayer.isPlaying()) {
            mMediaPlayer.stop();
        }
        mMediaPlayer.reset();
        mMediaPlayer.release();
    }
}
