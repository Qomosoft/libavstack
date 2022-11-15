package com.qomosoft.qomortc;

import android.app.Activity;
import android.content.Intent;
import android.os.Bundle;
import android.support.annotation.NonNull;
import android.support.v7.app.AppCompatActivity;
import android.text.Editable;
import android.text.TextWatcher;
import android.view.Menu;
import android.view.MenuItem;
import android.view.View;
import android.view.inputmethod.InputMethodManager;
import android.widget.Button;
import android.widget.EditText;
import android.widget.ImageView;
import android.widget.TextView;

public class MainActivity extends AppCompatActivity {
    private static final String TAG = "MainActivity";
    private TextView hintTv;
    private EditText userIdEt;
    private ImageView clearInputUidImg;
    private EditText roomIdEt;
    private ImageView clearInputRoomIdImg;
    private Button joinBtn;

    @Override
    protected void onCreate(Bundle savedInstanceState) {
        super.onCreate(savedInstanceState);
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

        joinBtn.setOnClickListener(v -> {
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
            //TODO: start MeetingActivity
            hideSoftKeyboard();
        });
    }

    private void hideSoftKeyboard() {
        if (getCurrentFocus() == null) return;
        InputMethodManager imm = (InputMethodManager) getSystemService(Activity.INPUT_METHOD_SERVICE);
        if (imm == null) return;
        imm.hideSoftInputFromWindow(getCurrentFocus().getWindowToken(), 0);
    }
}