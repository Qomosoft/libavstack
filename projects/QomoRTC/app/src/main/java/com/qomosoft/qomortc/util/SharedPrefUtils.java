package com.qomosoft.qomortc.util;

import android.content.Context;
import android.content.SharedPreferences;
import android.content.SharedPreferences.Editor;
import android.preference.PreferenceManager;

import com.qomosoft.qomortc.R;

public class SharedPrefUtils {

    private static final String DEFAULT_PREFERENCE = "preferences";

    private SharedPrefUtils() {
        throw new UnsupportedOperationException(
            "Should not create instance of Util class. Please use as static..");
    }

    /**
     * Gets boolean data.
     *
     * @param context the context
     * @param key     the key
     * @return the boolean data
     */
    static public boolean getBooleanData(Context context, String key) {
        PreferenceManager.setDefaultValues(context, R.xml.preferences, false);
        SharedPreferences pref = PreferenceManager.getDefaultSharedPreferences(context);
        //return context.getSharedPreferences(DEFAULT_PREFERENCE, Context.MODE_PRIVATE).getBoolean(key, false);
        return pref.getBoolean(key, false);
    }

    /**
     * Gets int data.
     *
     * @param context the context
     * @param key     the key
     * @return the int data
     */
    static public int getIntData(Context context, String key) {
        PreferenceManager.setDefaultValues(context, R.xml.preferences, false);
        SharedPreferences pref = PreferenceManager.getDefaultSharedPreferences(context);
        return pref.getInt(key, 0);
        //return context.getSharedPreferences(DEFAULT_PREFERENCE, Context.MODE_PRIVATE).getInt(key, 0);
    }

    /**
     * Gets string data.
     *
     * @param context the context
     * @param key     the key
     * @return the string data
     */
    // Get Data
    static public String getStringData(Context context, String key) {
        PreferenceManager.setDefaultValues(context, R.xml.preferences, false);
        SharedPreferences pref = PreferenceManager.getDefaultSharedPreferences(context);
        return pref.getString(key, null);
        //return context.getSharedPreferences(DEFAULT_PREFERENCE, Context.MODE_PRIVATE).getString(key, null);
    }

    /**
     * Save data.
     *
     * @param context the context
     * @param key     the key
     * @param val     the val
     */
    // Save Data
    static public void saveData(Context context, String key, String val) {
        PreferenceManager.setDefaultValues(context, R.xml.preferences, false);
        SharedPreferences pref = PreferenceManager.getDefaultSharedPreferences(context);
        pref.edit().putString(key, val).apply();
        //context.getSharedPreferences(DEFAULT_PREFERENCE, Context.MODE_PRIVATE).edit().putString(key, val).apply();
    }

    /**
     * Save data.
     *
     * @param context the context
     * @param key     the key
     * @param val     the val
     */
    static public void saveData(Context context, String key, int val) {
        PreferenceManager.setDefaultValues(context, R.xml.preferences, false);
        SharedPreferences pref = PreferenceManager.getDefaultSharedPreferences(context);
        pref.edit().putInt(key, val).apply();
        //context.getSharedPreferences(DEFAULT_PREFERENCE, Context.MODE_PRIVATE).edit().putInt(key, val).apply();
    }

    /**
     * Save data.
     *
     * @param context the context
     * @param key     the key
     * @param val     the val
     */
    static public void saveData(Context context, String key, boolean val) {
        PreferenceManager.setDefaultValues(context, R.xml.preferences, false);
        SharedPreferences pref = PreferenceManager.getDefaultSharedPreferences(context);
        pref.edit().putBoolean(key, val).apply();
        //context.getSharedPreferences(DEFAULT_PREFERENCE, Context.MODE_PRIVATE)
        //    .edit()
        //    .putBoolean(key, val)
        //    .apply();
    }

    static public Editor getSharedPrefEditor(Context context, String pref) {
        return context.getSharedPreferences(pref, Context.MODE_PRIVATE).edit();
    }

    static public void saveData(Editor editor) {
        editor.apply();
    }
}