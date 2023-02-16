package com.example.autoreply;

import android.app.NotificationManager;
import android.app.PendingIntent;
import android.content.Intent;
import android.os.Build;
import android.service.notification.NotificationListenerService;
import android.service.notification.StatusBarNotification;
import android.util.Log;

import androidx.annotation.RequiresApi;

public class NotificationService extends NotificationListenerService {
    private static final String WHATSAPP_PACKAGE = "com.whatsapp";
    private String TAG = getClass().getSimpleName();
    NotificationManager notificationManager;

    public void onCreate() {
        super.onCreate();
        this.notificationManager = (NotificationManager) getApplicationContext().getSystemService("notification");
    }

    public void onDestroy() {
        super.onDestroy();
    }

    @RequiresApi(api = Build.VERSION_CODES.LOLLIPOP)
    public void onNotificationPosted(StatusBarNotification statusBarNotification) {
        if (statusBarNotification.getPackageName().equals(WHATSAPP_PACKAGE) && (statusBarNotification.getNotification().flags & 512) == 0) {
            cancelNotification(statusBarNotification.getKey());
            this.notificationManager.cancel(statusBarNotification.getId());
            Action quickReplyAction = NotificationUtils.getQuickReplyAction(statusBarNotification.getNotification(), getPackageName());
            if (quickReplyAction != null) {
                Log.i(this.TAG, "success");
                try {
                    quickReplyAction.sendReply(getApplicationContext(), MainActivity.message);
                } catch (PendingIntent.CanceledException e) {
                    String str = this.TAG;
                    Log.i(str, "CRAP " + e.toString());
                }
            } else {
                Log.i(this.TAG, "not success");
            }
        }
    }

    public void onNotificationRemoved(StatusBarNotification statusBarNotification) {
        super.onNotificationRemoved(statusBarNotification);
    }

    public void onListenerConnected() {
        super.onListenerConnected();
        Log.e("startCommand", "yeah");
    }

    public int onStartCommand(Intent intent, int i, int i2) {
        Log.e("startCommand", "yes");
        return super.onStartCommand(intent, i, i2);
    }
}