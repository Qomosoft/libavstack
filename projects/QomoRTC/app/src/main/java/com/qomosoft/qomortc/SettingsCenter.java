package com.qomosoft.qomortc;

import android.content.Context;
import android.text.TextUtils;

import com.qomosoft.qomortc.util.SharedPrefUtils;

import java.util.Random;

public class SettingsCenter {
    private static final String TAG = "SettingsCenter";

    public static Integer getUserId(Context context) {
        String key = context.getString(R.string.pref_user_id);
        int uid = SharedPrefUtils.getIntData(context, key);

        if (uid == 0) {
            uid = new Random().nextInt(1000000);
            SharedPrefUtils.saveData(context, key, uid);
            return uid;
        }

        return uid;
    }

    public static void setUserId(Context context, int uid) {
        String key = context.getString(R.string.pref_user_id);
        SharedPrefUtils.saveData(context, key, uid);
    }

    public static String getRoomId(Context context) {
        String key = context.getString(R.string.pref_room_id);
        String roomId = SharedPrefUtils.getStringData(context, key);

        if (TextUtils.isEmpty(roomId)) {
            roomId = Integer.toString(new Random().nextInt(1000000));
            SharedPrefUtils.saveData(context, key, roomId);
        }

        return roomId;
    }

    public static void setRoomId(Context context, String roomId) {
        String key = context.getString(R.string.pref_room_id);
        SharedPrefUtils.saveData(context, key, roomId);
    }
}
