package com.qomo.qomoplayer;

import static com.qomo.qomoplayer.Constants.VIDEO_NAME;

import androidx.appcompat.app.AppCompatActivity;

import android.media.MediaPlayer;
import android.os.Bundle;
import android.util.Log;
import android.view.View;
import android.widget.TextView;

import com.qomo.qomoplayer.databinding.ActivityMainBinding;
import com.qomo.qomoplayer.media.QomoPlayer;
import com.qomo.qomoplayer.utils.AssetUtils;

import java.io.File;

public class MainActivity extends AppCompatActivity {
    private static final String TAG = "MainActivity";
    private String videoPath;
    // Used to load the 'qomoplayer' library on application startup.
    static {
        System.loadLibrary("qomoplayer_jni");
    }

    private ActivityMainBinding binding;
    private QomoPlayer player = new QomoPlayer();

    public MainActivity() {
    }

    @Override
    protected void onCreate(Bundle savedInstanceState) {
        super.onCreate(savedInstanceState);
        binding = ActivityMainBinding.inflate(getLayoutInflater());
        setContentView(binding.getRoot());
        prepareVideo();

        player.setDataSource(videoPath);
        player.prepare();

        // Example of a call to a native method
        TextView tv = binding.sampleText;
//        tv.setText(stringFromJNI());
        tv.setOnClickListener(new View.OnClickListener() {
            @Override
            public void onClick(View v) {
                player.start();
            }
        });
    }

    private void prepareVideo() {
        videoPath = getExternalFilesDir(null).getAbsolutePath() + File.separator + VIDEO_NAME;
        if (AssetUtils.copyAsset(getAssets(), VIDEO_NAME, videoPath, false) != 0) {
            Log.e(TAG, "copyAsset failed");
        }
    }
}