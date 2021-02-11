package com.example.autoreply;

import android.app.PendingIntent;
import android.app.RemoteInput;
import android.content.Context;
import android.content.Intent;
import android.os.Build;
import android.os.Bundle;
import android.os.Parcel;
import android.os.Parcelable;
import android.util.Log;

import androidx.annotation.RequiresApi;
import androidx.core.app.NotificationCompat;
import java.util.ArrayList;
import java.util.Iterator;

public class Action implements Parcelable {
    public static final Parcelable.Creator CREATOR = new Parcelable.Creator() {
        /* class com.respondernew.my.Action.AnonymousClass1 */

        @Override // android.os.Parcelable.Creator
        public Action createFromParcel(Parcel parcel) {
            return new Action(parcel);
        }

        @Override // android.os.Parcelable.Creator
        public Action[] newArray(int i) {
            return new Action[i];
        }
    };
    private final boolean isQuickReply;
    private final PendingIntent p;
    private final String packageName;
    private final ArrayList<RemoteInputParcel> remoteInputs = new ArrayList<>();
    private final String text;

    public int describeContents() {
        return 0;
    }

    public Action(Parcel parcel) {
        this.text = parcel.readString();
        this.packageName = parcel.readString();
        this.p = (PendingIntent) parcel.readParcelable(PendingIntent.class.getClassLoader());
        this.isQuickReply = parcel.readByte() != 0;
        parcel.readTypedList(this.remoteInputs, RemoteInputParcel.CREATOR);
    }

    public void writeToParcel(Parcel parcel, int i) {
        parcel.writeString(this.text);
        parcel.writeString(this.packageName);
        parcel.writeParcelable(this.p, i);
        parcel.writeByte(this.isQuickReply ? (byte) 1 : 0);
        parcel.writeTypedList(this.remoteInputs);
    }

    public Action(NotificationCompat.Action action, String str, boolean z) {
        this.text = action.title.toString();
        this.packageName = str;
        this.p = action.actionIntent;
        if (action.getRemoteInputs() != null) {
            int length = action.getRemoteInputs().length;
            for (int i = 0; i < length; i++) {
                this.remoteInputs.add(new RemoteInputParcel(action.getRemoteInputs()[i]));
            }
        }
        this.isQuickReply = z;
    }

    @RequiresApi(api = Build.VERSION_CODES.KITKAT_WATCH)
    public void sendReply(Context context, String str) throws PendingIntent.CanceledException {
        Log.i("Vishu", "inside sendReply");
        Intent intent = new Intent();
        Bundle bundle = new Bundle();
        ArrayList arrayList = new ArrayList();

        for (RemoteInputParcel next : this.remoteInputs) {
            Log.i("", "RemoteInput: " + next.getLabel());
            bundle.putCharSequence(next.getResultKey(), str);
            RemoteInput.Builder builder = new RemoteInput.Builder(next.getResultKey());
            builder.setLabel(next.getLabel());
            builder.setChoices(next.getChoices());
            builder.setAllowFreeFormInput(next.isAllowFreeFormInput());
            builder.addExtras(next.getExtras());
            arrayList.add(builder.build());
        }
        RemoteInput.addResultsToIntent((RemoteInput[]) arrayList.toArray(new RemoteInput[arrayList.size()]), intent, bundle);
        this.p.send(context, 0, intent);
    }
}
