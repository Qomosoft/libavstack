package com.qomo.qomoopensl;

import android.support.v7.app.AppCompatActivity;
import android.os.Bundle;
import android.util.Log;
import android.view.View;
import android.widget.TextView;
import android.widget.Toast;

import com.qomo.qomoopensl.databinding.ActivityMainBinding;
import com.qomo.qomoopensl.utils.AssetUtils;

import java.io.File;

public class MainActivity extends AppCompatActivity {
    private static final String TAG = "MainActivity";
    private static final String pcmFileName = "out.pcm";
    private String pcmFilePath;
    private final QomoOpenSl qomoOpenSl = new QomoOpenSl();

    private ActivityMainBinding binding;

    @Override
    protected void onCreate(Bundle savedInstanceState) {
        super.onCreate(savedInstanceState);

        binding = ActivityMainBinding.inflate(getLayoutInflater());
        setContentView(binding.getRoot());
        preparePcmData();
        initView();

        if (qomoOpenSl.init(pcmFilePath, 48000, 2, 2) != 0) {
            showToast("init failed");
        }
    }

    private void initView() {
        binding.btnStart.setOnClickListener(view -> qomoOpenSl.start());
        binding.btnStop.setOnClickListener(view -> qomoOpenSl.stop());
    }

    private void preparePcmData() {
        pcmFilePath = getExternalFilesDir(null) + File.separator + pcmFileName;
        AssetUtils.copyAsset(getAssets(), pcmFileName, pcmFilePath);
    }

    @Override
    protected void onDestroy() {
        super.onDestroy();
        qomoOpenSl.release();
    }

    protected void showToast(String msg) {
        Toast.makeText(this, msg, Toast.LENGTH_SHORT).show();
    }

    static {
        System.loadLibrary("qomoopensles");
    }

}