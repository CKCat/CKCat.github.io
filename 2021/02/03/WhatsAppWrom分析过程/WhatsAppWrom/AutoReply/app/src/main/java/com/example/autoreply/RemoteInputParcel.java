package com.example.autoreply;

import android.os.Bundle;
import android.os.Parcel;
import android.os.Parcelable.Creator;
import android.os.Parcelable;
import androidx.core.app.RemoteInput;

public class RemoteInputParcel implements Parcelable {
    public static final Parcelable.Creator CREATOR = new Parcelable.Creator() {
        public RemoteInputParcel createFromParcel(Parcel arg2) {
            return new RemoteInputParcel(arg2);
        }

        public RemoteInputParcel[] newArray(int arg1) {
            return new RemoteInputParcel[arg1];
        }
    };
    private boolean allowFreeFormInput;
    private String[] choices;
    private Bundle extras;
    private String label;
    private String resultKey;


    public RemoteInputParcel(Parcel arg3) {
        boolean v0 = false;
        this.choices = new String[0];
        this.label = arg3.readString();
        this.resultKey = arg3.readString();
        this.choices = arg3.createStringArray();
        if(arg3.readByte() != 0) {
            v0 = true;
        }

        this.allowFreeFormInput = v0;
        this.extras = (Bundle)arg3.readParcelable(Bundle.class.getClassLoader());
    }

    public RemoteInputParcel(RemoteInput arg2) {
        this.choices = new String[0];
        this.label = arg2.getLabel().toString();
        this.resultKey = arg2.getResultKey();
        this.charSequenceToStringArray(arg2.getChoices());
        this.allowFreeFormInput = arg2.getAllowFreeFormInput();
        this.extras = arg2.getExtras();
    }

    public void charSequenceToStringArray(CharSequence[] arg5) {
        if(arg5 != null) {
            this.choices = new String[arg5.length];
            int v1;
            for(v1 = 0; v1 < arg5.length; ++v1) {
                this.choices[v1] = arg5[v1].toString();
            }
        }
    }

    @Override  // android.os.Parcelable
    public int describeContents() {
        return 0;
    }

    public CharSequence[] getChoices() {
        return this.choices;
    }

    public Bundle getExtras() {
        return this.extras;
    }

    public String getLabel() {
        return this.label;
    }

    public String getResultKey() {
        return this.resultKey;
    }

    public boolean isAllowFreeFormInput() {
        return this.allowFreeFormInput;
    }

    @Override  // android.os.Parcelable
    public void writeToParcel(Parcel arg2, int arg3) {
        arg2.writeString(this.label);
        arg2.writeString(this.resultKey);
        arg2.writeStringArray(this.choices);
        arg2.writeByte(this.allowFreeFormInput ? (byte) 1 : 0);
        arg2.writeParcelable(this.extras, arg3);
    }
}


