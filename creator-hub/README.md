# Creator Hub

A personal creator-dashboard app with a clean, card-based look. Everything is editable: the app name and icon, the channel or store name and picture, fonts, currency, app size, stats, labels, status icons, comments and videos.

The whole app is one page, `web/index.html`. The Android and iOS apps are thin native shells around it.

## Folders

| Folder | What it is |
| --- | --- |
| `web/index.html` | The app itself (HTML, CSS and JS in one file). Open it in any browser. |
| `android/` | Android shell (Java WebView). `./build.sh` makes `bin/SmoreHub.apk`. |
| `ios/` | iOS shell (C on the Objective-C runtime, WKWebView). `./build.sh` makes `build/CreatorHub.ipa`. |
| `releases/` | Prebuilt `SmoreHub.apk` (Android) and `CreatorHub.ipa` (iOS, unsigned). |
| `tools/render-icons.js` | Renders the 90 iOS home-screen icons from the app's icon presets (Playwright). |

## Install

- **Android:** copy `releases/SmoreHub.apk` to the phone, open it, and allow installs from unknown sources.
- **iPhone:** open `releases/CreatorHub.ipa` in Sideloadly, sign with your Apple ID, then trust the profile under Settings → General → VPN & Device Management (and turn on Developer Mode on iOS 16+).

## Build

- **Android** (Ubuntu): `apt-get install aapt apksigner zipalign dalvik-exchange android-sdk-platform-23`, then `android/build.sh`. It creates a signing key (`android/smore.p12`) on first run. Keep that file to install updates over an existing install; it is git-ignored.
- **iOS** (Linux, no Xcode needed): `apt-get install clang lld-18`, then `ios/build.sh`.

After editing `web/index.html`, rebuild both shells so the apps get the change.
