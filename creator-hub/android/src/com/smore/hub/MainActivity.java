package com.smore.hub;

import android.app.Activity;
import android.content.Intent;
import android.content.res.Configuration;
import android.graphics.drawable.ColorDrawable;
import android.net.Uri;
import android.os.Build;
import android.os.Bundle;
import android.view.View;
import android.view.Window;
import android.webkit.JavascriptInterface;
import android.webkit.ValueCallback;
import android.webkit.WebChromeClient;
import android.webkit.WebSettings;
import android.webkit.WebView;
import android.webkit.WebViewClient;
import java.io.ByteArrayOutputStream;
import java.io.InputStream;

public class MainActivity extends Activity {
    private static final String BASE = "https://smore-hub.local/";
    private static final int PICK_IMAGE = 41;
    private WebView web;
    private ValueCallback<Uri[]> fileCallback;

    @Override
    protected void onCreate(Bundle state) {
        super.onCreate(state);
        web = new WebView(this);
        setContentView(web);

        WebSettings s = web.getSettings();
        s.setJavaScriptEnabled(true);
        s.setDomStorageEnabled(true);
        s.setDatabaseEnabled(true);
        s.setAllowFileAccess(false);
        s.setTextZoom(100);
        web.setOverScrollMode(View.OVER_SCROLL_NEVER);
        web.addJavascriptInterface(new Bridge(), "SmoreNative");

        web.setWebViewClient(new WebViewClient() {
            @Override
            public boolean shouldOverrideUrlLoading(WebView view, String url) {
                if (url.startsWith(BASE)) return false;
                try { startActivity(new Intent(Intent.ACTION_VIEW, Uri.parse(url))); } catch (Exception ignored) { }
                return true;
            }
        });

        web.setWebChromeClient(new WebChromeClient() {
            @Override
            public boolean onShowFileChooser(WebView view, ValueCallback<Uri[]> callback, FileChooserParams params) {
                if (fileCallback != null) fileCallback.onReceiveValue(null);
                fileCallback = callback;
                Intent pick = new Intent(Intent.ACTION_GET_CONTENT);
                pick.addCategory(Intent.CATEGORY_OPENABLE);
                pick.setType("image/*");
                try {
                    startActivityForResult(Intent.createChooser(pick, "Choose a picture"), PICK_IMAGE);
                } catch (Exception e) {
                    fileCallback = null;
                    return false;
                }
                return true;
            }
        });

        boolean dark = systemDark();
        setBars(dark);
        web.setBackgroundColor(dark ? 0xFF0F0F0F : 0xFFFFFFFF);
        web.loadDataWithBaseURL(BASE, page(dark), "text/html", "utf-8", null);
    }

    private boolean systemDark() {
        int mode = getResources().getConfiguration().uiMode & Configuration.UI_MODE_NIGHT_MASK;
        return mode == Configuration.UI_MODE_NIGHT_YES;
    }

    private String page(boolean dark) {
        try {
            InputStream in = getAssets().open("index.html");
            ByteArrayOutputStream out = new ByteArrayOutputStream();
            byte[] buf = new byte[16384];
            int n;
            while ((n = in.read(buf)) > 0) out.write(buf, 0, n);
            in.close();
            String html = out.toString("UTF-8");
            return html.replace("<!--SYS-->", "<script>window.__SYS_DARK=" + dark + ";</script>");
        } catch (Exception e) {
            return "<p style='font-family:sans-serif;padding:24px'>Couldn't open the app. Try reinstalling it.</p>";
        }
    }

    private void setBars(boolean dark) {
        Window w = getWindow();
        int bg = dark ? 0xFF0F0F0F : 0xFFFFFFFF;
        w.setBackgroundDrawable(new ColorDrawable(bg));
        w.setStatusBarColor(bg);
        w.setNavigationBarColor(bg);
        int flags = 0;
        if (!dark) {
            flags |= View.SYSTEM_UI_FLAG_LIGHT_STATUS_BAR;
            if (Build.VERSION.SDK_INT >= 26) flags |= 0x00000010; // light navigation bar
        }
        w.getDecorView().setSystemUiVisibility(flags);
    }

    @Override
    public void onConfigurationChanged(Configuration c) {
        super.onConfigurationChanged(c);
        boolean dark = systemDark();
        web.evaluateJavascript("window.__setSysDark&&window.__setSysDark(" + dark + ")", null);
    }

    @Override
    protected void onActivityResult(int request, int result, Intent data) {
        super.onActivityResult(request, result, data);
        if (request != PICK_IMAGE || fileCallback == null) return;
        Uri[] picked = null;
        if (result == RESULT_OK && data != null && data.getData() != null) picked = new Uri[]{data.getData()};
        fileCallback.onReceiveValue(picked);
        fileCallback = null;
    }

    @Override
    public void onBackPressed() {
        web.evaluateJavascript("(window.smoreBack&&window.smoreBack())?'1':'0'", new ValueCallback<String>() {
            @Override
            public void onReceiveValue(String v) {
                if (!"\"1\"".equals(v)) finish();
            }
        });
    }

    private class Bridge {
        @JavascriptInterface
        public void setDark(final boolean dark) {
            runOnUiThread(new Runnable() {
                @Override
                public void run() {
                    setBars(dark);
                    web.setBackgroundColor(dark ? 0xFF0F0F0F : 0xFFFFFFFF);
                }
            });
        }
    }
}
