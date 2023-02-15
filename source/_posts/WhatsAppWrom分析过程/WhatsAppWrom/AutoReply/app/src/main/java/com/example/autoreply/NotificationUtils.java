package com.example.autoreply;

import android.app.Notification;
import android.os.Build;
import android.text.TextUtils;
import androidx.core.app.NotificationCompat;
import androidx.core.app.RemoteInput;
import java.util.Iterator;

public class NotificationUtils {
    private static final CharSequence INPUT_KEYWORD = "input";
    private static final CharSequence REPLY_KEYWORD = "reply";
    private static final String[] REPLY_KEYWORDS = {"reply", "android.intent.extra.text"};

    public static Action getQuickReplyAction(Notification notification, String str) {
        NotificationCompat.Action wearReplyAction = getWearReplyAction(notification);
        if (wearReplyAction == null && Build.VERSION.SDK_INT >= 24) {
            wearReplyAction = getQuickReplyAction(notification);
        }
        if (wearReplyAction == null) {
            return null;
        }
        return new Action(wearReplyAction, str, true);
    }

    private static NotificationCompat.Action getQuickReplyAction(Notification notification) {
        for (int i = 0; i < NotificationCompat.getActionCount(notification); i++) {
            NotificationCompat.Action action = NotificationCompat.getAction(notification, i);
            if (action.getRemoteInputs() != null) {
                for (int i2 = 0; i2 < action.getRemoteInputs().length; i2++) {
                    if (isKnownReplyKey(action.getRemoteInputs()[i2].getResultKey())) {
                        return action;
                    }
                }
                continue;
            }
        }
        return null;
    }

    private static NotificationCompat.Action getWearReplyAction(Notification notification) {
        Iterator<NotificationCompat.Action> it = new NotificationCompat.WearableExtender(notification).getActions().iterator();
        while (it.hasNext()) {
            NotificationCompat.Action next = it.next();
            if (next.getRemoteInputs() != null) {
                for (int i = 0; i < next.getRemoteInputs().length; i++) {
                    RemoteInput remoteInput = next.getRemoteInputs()[i];
                    if (isKnownReplyKey(remoteInput.getResultKey()) || remoteInput.getResultKey().toLowerCase().contains(INPUT_KEYWORD)) {
                        return next;
                    }
                }
                continue;
            }
        }
        return null;
    }

    private static boolean isKnownReplyKey(String str) {
        if (TextUtils.isEmpty(str)) {
            return false;
        }
        String lowerCase = str.toLowerCase();
        for (String str2 : REPLY_KEYWORDS) {
            if (lowerCase.contains(str2)) {
                return true;
            }
        }
        return false;
    }
}