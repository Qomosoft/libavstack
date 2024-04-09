package com.qomo.qomoplayer;

import static com.qomo.qomoplayer.Constants.VIDEO_NAME;

import android.os.Bundle;
import android.util.Log;
import android.view.SurfaceHolder;
import android.view.SurfaceView;
import android.view.View;
import android.view.WindowManager;
import android.widget.Button;

import androidx.annotation.NonNull;
import androidx.appcompat.app.AppCompatActivity;

import com.qomo.qomoplayer.databinding.ActivityMainBinding;
import com.qomo.qomoplayer.media.OnCompletionListener;
import com.qomo.qomoplayer.media.QomoPlayer;
import com.qomo.qomoplayer.utils.AssetUtils;

import java.io.File;

public class MainActivity extends AppCompatActivity implements SurfaceHolder.Callback, View.OnClickListener, OnCompletionListener {
    private static final String TAG = "MainActivity";
    private SurfaceView surfaceView;
    private Button playButton;
    private Button pauseButton;
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
        getWindow().addFlags(WindowManager.LayoutParams.FLAG_KEEP_SCREEN_ON);
        initViews();
        prepareVideo();

        player.setDataSource(videoPath);
        player.setOnCompletionListener(this);
        player.prepare();
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
        playButton.setOnClickListener(this);

        pauseButton = findViewById(R.id.btn_pause);
        pauseButton.setOnClickListener(this);
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

    @Override
    public void onCompletion() {
        Log.e(TAG, "onCompletion");
    }

    @Override
    public void onClick(View v) {
        switch (v.getId()) {
            case R.id.btn_play:
                player.start();
                break;
            case R.id.btn_pause:
                player.pause();
                break;
            default:
                break;
        }
    }
}