package com.qomo.qomoplayer;

import static com.qomo.qomoplayer.Constants.VIDEO_NAME;

import androidx.annotation.NonNull;
import androidx.appcompat.app.AppCompatActivity;

import android.os.Bundle;
import android.util.Log;
import android.view.SurfaceHolder;
import android.view.SurfaceView;
import android.view.View;
import android.widget.Button;

import com.qomo.qomoplayer.databinding.ActivityMainBinding;
import com.qomo.qomoplayer.media.QomoPlayer;
import com.qomo.qomoplayer.utils.AssetUtils;

import java.io.File;

public class MainActivity extends AppCompatActivity implements SurfaceHolder.Callback {
    private static final String TAG = "MainActivity";
    private SurfaceView surfaceView;
    private Button playButton;
    private String videoPath;
    // Used to load the 'qomoplayer' library on application startup.
    static {
        System.loadLibrary("qomoplayer_jni");
    }

    private ActivityMainBinding binding;
    private QomoPlayer player = new QomoPlayer();

    @Override
    protected void onCreate(Bundle savedInstanceState) {
        super.onCreate(savedInstanceState);
        binding = ActivityMainBinding.inflate(getLayoutInflater());
        setContentView(binding.getRoot());
        initViews();
        prepareVideo();

        player.setDataSource(videoPath);
        player.prepare();

        // Example of a call to a native method
//        TextView tv = binding.sampleText;
////        tv.setText(stringFromJNI());
//        tv.setOnClickListener(new View.OnClickListener() {
//            @Override
//            public void onClick(View v) {
//                player.start();
//            }
//        });
    }

    @Override
    public void onBackPressed() {
        super.onBackPressed();
        player.stop();
    }

    private void initViews() {
        surfaceView = findViewById(R.id.surface_content);
        SurfaceHolder holder = surfaceView.getHolder();
        holder.addCallback(this);

        playButton = findViewById(R.id.btn_play);
        playButton.setOnClickListener(v -> player.start());
    }

    private void prepareVideo() {
        videoPath = getExternalFilesDir(null).getAbsolutePath() + File.separator + VIDEO_NAME;
        if (AssetUtils.copyAsset(getAssets(), VIDEO_NAME, videoPath, false) != 0) {
            Log.e(TAG, "copyAsset failed");
        }
    }

    @Override
    public void surfaceCreated(@NonNull SurfaceHolder holder) {
        player.setSurface(holder.getSurface());
    }

    @Override
    public void surfaceChanged(@NonNull SurfaceHolder holder, int format, int width, int height) {
        player.setWindowSize(width, height);
    }

    @Override
    public void surfaceDestroyed(@NonNull SurfaceHolder holder) {

    }
}