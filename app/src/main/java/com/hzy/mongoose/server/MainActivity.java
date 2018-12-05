package com.hzy.mongoose.server;

import android.Manifest;
import android.annotation.TargetApi;
import android.os.Build;
import android.os.Bundle;
import android.os.Environment;
import android.support.v7.app.AppCompatActivity;
import android.widget.TextView;

import com.blankj.utilcode.util.NetworkUtils;
import com.blankj.utilcode.util.PermissionUtils;
import com.blankj.utilcode.util.ToastUtils;
import com.hzy.mongoose.lib.MongooseNative;

public class MainActivity extends AppCompatActivity {

    private static final String SERVER_PORT = "19999";
    private TextView mTextVersion;
    private TextView mTextStatus;

    @Override
    protected void onCreate(Bundle savedInstanceState) {
        super.onCreate(savedInstanceState);
        setContentView(R.layout.activity_main);
        mTextVersion = findViewById(R.id.text_version);
        mTextStatus = findViewById(R.id.text_status);
        mTextVersion.setText(MongooseNative.getVersionInfo());
        startFileServer();
    }

    @Override
    protected void onDestroy() {
        stopServer();
        super.onDestroy();
    }

    @TargetApi(Build.VERSION_CODES.JELLY_BEAN)
    private void startFileServer() {
        if (NetworkUtils.getNetworkType() != NetworkUtils.NetworkType.NETWORK_WIFI) {
            ToastUtils.showShort("Please Connect the Wifi!");
            return;
        }
        String docRoot;
        if (Environment.getExternalStorageState().equals(Environment.MEDIA_MOUNTED)) {
            docRoot = Environment.getExternalStorageDirectory().getPath();
        } else {
            docRoot = getFilesDir().getPath();
        }
        PermissionUtils.permission(Manifest.permission.READ_EXTERNAL_STORAGE)
                .callback(new PermissionUtils.SimpleCallback() {
                    @Override
                    public void onGranted() {
                        startServerAsync(docRoot);
                    }

                    @Override
                    public void onDenied() {

                    }
                }).request();
        mTextStatus.setText(String.format("Server On http://%s:%s",
                NetworkUtils.getIpAddressByWifi(), SERVER_PORT));
    }

    /**
     * Start Server on some thread
     *
     * @param docRoot the root of file path
     */
    private void startServerAsync(String docRoot) {
        new Thread() {
            @Override
            public void run() {
                MongooseNative.nStartServer(SERVER_PORT, docRoot);
            }
        }.start();
    }

    private void stopServer() {
        MongooseNative.nStopServer();
    }
}
