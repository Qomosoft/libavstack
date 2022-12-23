package com.qomosoft.qomortc;

import android.app.Activity;
import android.content.Intent;
import android.content.SharedPreferences;
import android.net.Uri;
import android.os.Bundle;
import android.preference.PreferenceManager;
import android.support.annotation.NonNull;
import android.text.Editable;
import android.text.TextWatcher;
import android.util.Log;
import android.view.Menu;
import android.view.MenuItem;
import android.view.View;
import android.view.inputmethod.InputMethodManager;
import android.webkit.URLUtil;
import android.widget.Button;
import android.widget.EditText;
import android.widget.ImageView;
import android.widget.TextView;

import com.qomosoft.qomortc.activity.BaseActivity;

import java.util.Random;

public class MainActivity extends BaseActivity {
    private static final String TAG = "MainActivity";
    private TextView hintTv;
    private EditText userIdEt;
    private ImageView clearInputUidImg;
    private EditText roomIdEt;
    private ImageView clearInputRoomIdImg;
    private Button joinBtn;
    private SharedPreferences sharedPref;
    private String keyprefResolution;
    private String keyprefFps;
    private String keyprefVideoBitrateType;
    private String keyprefVideoBitrateValue;
    private String keyprefAudioBitrateType;
    private String keyprefAudioBitrateValue;
    private String keyprefRoomServerUrl;
    private String keyprefRoom;
    private String keyprefRoomList;

    @Override
    protected void onCreate(Bundle savedInstanceState) {
        super.onCreate(savedInstanceState);

        // Get setting keys.
        PreferenceManager.setDefaultValues(this, R.xml.preferences, false);
        sharedPref = PreferenceManager.getDefaultSharedPreferences(this);
        keyprefResolution = getString(R.string.pref_resolution_key);
        keyprefFps = getString(R.string.pref_fps_key);
        keyprefVideoBitrateType = getString(R.string.pref_maxvideobitrate_key);
        keyprefVideoBitrateValue = getString(R.string.pref_maxvideobitratevalue_key);
        keyprefAudioBitrateType = getString(R.string.pref_startaudiobitrate_key);
        keyprefAudioBitrateValue = getString(R.string.pref_startaudiobitratevalue_key);
        keyprefRoomServerUrl = getString(R.string.pref_room_server_url_key);
        keyprefRoom = getString(R.string.pref_room_key);
        keyprefRoomList = getString(R.string.pref_room_list_key);

        setContentView(R.layout.activity_main);
        initViews();
    }

    @Override
    public boolean onCreateOptionsMenu(Menu menu) {
        getMenuInflater().inflate(R.menu.menu_main_activity, menu);
        return super.onCreateOptionsMenu(menu);
    }

    @Override
    public boolean onOptionsItemSelected(@NonNull MenuItem item) {
        switch (item.getItemId()) {
            case R.id.setting:
                startActivity(new Intent(this, SettingsActivity.class));
                break;
            default:
                break;
        }
        return super.onOptionsItemSelected(item);
    }

    @Override
    protected void onPause() {
        super.onPause();
        String room = roomIdEt.getText().toString();
        SharedPreferences.Editor editor = sharedPref.edit();
        editor.putString(keyprefRoom, room);
        editor.commit();
    }

    private void initViews() {
        hintTv = findViewById(R.id.tv_hint);
        userIdEt = findViewById(R.id.et_user_id);
        clearInputUidImg = findViewById(R.id.img_clear_input_user_id);
        roomIdEt = findViewById(R.id.et_room_id);
        clearInputRoomIdImg = findViewById(R.id.img_clear_input_room_id);
        joinBtn = findViewById(R.id.btn_join);
        userIdEt.setText(SettingsCenter.getUserId(getBaseContext()).toString());
        roomIdEt.setText(SettingsCenter.getRoomId(getBaseContext())); //TODO: remove this line

        // 输入框为空时才显示清除内容的图标
        userIdEt.addTextChangedListener(new TextWatcher() {
            @Override
            public void beforeTextChanged(CharSequence s, int start, int count, int after) {

            }

            @Override
            public void onTextChanged(CharSequence s, int start, int before, int count) {

            }

            @Override
            public void afterTextChanged(Editable s) {
                clearInputUidImg.setVisibility(s == null || s.length() <= 0 ? View.GONE : View.VISIBLE);
            }
        });
        clearInputUidImg.setOnClickListener(v -> {
            userIdEt.setText("");
        });
        roomIdEt.addTextChangedListener(new TextWatcher() {
            @Override
            public void beforeTextChanged(CharSequence s, int start, int count, int after) {

            }

            @Override
            public void onTextChanged(CharSequence s, int start, int before, int count) {

            }

            @Override
            public void afterTextChanged(Editable s) {
                clearInputRoomIdImg.setVisibility(s == null || s.length() <= 0 ? View.GONE : View.VISIBLE);
            }
        });

        clearInputRoomIdImg.setOnClickListener(v -> {
            roomIdEt.setText("");
        });

        joinBtn.setOnClickListener(connectListener);
    }

    private void hideSoftKeyboard() {
        if (getCurrentFocus() == null) return;
        InputMethodManager imm = (InputMethodManager) getSystemService(Activity.INPUT_METHOD_SERVICE);
        if (imm == null) return;
        imm.hideSoftInputFromWindow(getCurrentFocus().getWindowToken(), 0);
    }

    private void connectToRoom(String roomId, boolean loopback) {
        Editable userIdEdit = userIdEt.getText();
        Editable roomIdEdit = roomIdEt.getText();
        if (userIdEdit == null || userIdEt.length() <= 0) {
            userIdEt.setText("0");
        }
        int uid = Integer.parseInt(userIdEt.getText().toString());
        SettingsCenter.setUserId(getBaseContext(), uid);
        if (roomIdEdit == null || roomIdEt.length() <= 0) {
            hintTv.setVisibility(View.VISIBLE);
            return;
        }
        SettingsCenter.setRoomId(getBaseContext(), roomIdEt.getText().toString());
        hintTv.setVisibility(View.GONE);
        hideSoftKeyboard();

        // roomId is random for loopback.
        if (loopback) {
            roomId = Integer.toString((new Random()).nextInt(100000000));
        }

        String roomUrl = sharedPref.getString(
                keyprefRoomServerUrl, getString(R.string.pref_room_server_url_default));
        // Start AppRTCMobile activity.
        Log.d(TAG, "Connecting to room " + roomId + " at URL " + roomUrl);
        if (validateUrl(roomUrl)) {
            Uri uri = Uri.parse(roomUrl);
            Intent intent = new Intent(this, CallActivity.class);
            intent.setData(uri);
            intent.putExtra(CallActivity.EXTRA_ROOMID, roomId);
            intent.putExtra(CallActivity.EXTRA_LOOPBACK, loopback);
            startActivity(intent);
        }
    }

    private boolean validateUrl(String url) {
        if (URLUtil.isHttpsUrl(url) || URLUtil.isHttpUrl(url)) {
            return true;
        }

        showLongToast(getString(R.string.invalid_url_text));
        return false;
    }

    private final View.OnClickListener connectListener = new View.OnClickListener() {
        @Override
        public void onClick(View view) {
            connectToRoom(roomIdEt.getText().toString(), false);
        }
    };
}