package com.qomosoft.qomortc;

import android.support.annotation.Nullable;
import android.util.Log;

import java.net.URI;
import java.net.URISyntaxException;

import de.tavendo.autobahn.WebSocket;
import de.tavendo.autobahn.WebSocketConnection;
import de.tavendo.autobahn.WebSocketException;

public class WebSocketChannelClient {
    private static final String TAG = "WebSocketChannelClient";
    private WebSocketConnection ws;
    private String wsServerUrl;
    private String postServerUrl;
    @Nullable
    private String roomID;
    @Nullable
    private String clientID;
    private WebSocketConnectionState state;
    private WebSocketHandler wsHander;

    /**
     * Possible WebSocket connection states.
     */
    public enum WebSocketConnectionState { NEW, CONNECTED, REGISTERED, CLOSED, ERROR }

    void connect(final String wsUrl, final String postUrl) {
        if (state != WebSocketConnectionState.NEW) {
            Log.e(TAG, "WebSocket is already connected.");
            return;
        }
        wsServerUrl = wsUrl;
        postServerUrl = postUrl;
//        closeEvent = false;

        Log.d(TAG, "Connecting WebSocket to: " + wsUrl + ". Post URL: " + postUrl);
        ws = new WebSocketConnection();
        wsHander = new WebSocketHandler();
        try {
            ws.connect(new URI(wsServerUrl), wsHander);
        } catch (WebSocketException e) {
            Log.e(TAG, "WebSocket connection error: " + e.getMessage());
        } catch (URISyntaxException e) {
            Log.e(TAG, "URI exception: " + e.getMessage());
        }
    }

    private class WebSocketHandler implements WebSocket.WebSocketConnectionObserver {
        @Override
        public void onOpen() {

        }

        @Override
        public void onClose(WebSocketCloseNotification webSocketCloseNotification, String s) {

        }

        @Override
        public void onTextMessage(String s) {

        }

        @Override
        public void onRawTextMessage(byte[] bytes) {

        }

        @Override
        public void onBinaryMessage(byte[] bytes) {

        }
    }
}
