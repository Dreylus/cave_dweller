#!/bin/bash
set -eo pipefail
cd "$(dirname "$0")"
mkdir -p gen obj bin assets
W=../web/index.html
{ printf "%s\n" "<!doctype html>" "<html lang=\"en\"><head>" "<meta charset=\"utf-8\">" "<meta name=\"viewport\" content=\"width=device-width, initial-scale=1, viewport-fit=cover\">" "<!--SYS-->" "<style>body{margin:0;font-size:14px}img{max-width:100%}[hidden]{display:none!important}</style>"; cat $W; printf "\n</html>\n"; } > assets/index.html
[ -f smore.p12 ] || keytool -genkeypair -keystore smore.p12 -storetype PKCS12 -storepass smorehub -keypass smorehub -alias smore -keyalg RSA -keysize 2048 -validity 10000 -dname "CN=Smore Hub"
export JAVA_TOOL_OPTIONS=
AJ=/usr/lib/android-sdk/platforms/android-23/android.jar
aapt package -f -m -J gen -M AndroidManifest.xml -S res -I $AJ
javac -nowarn -Xlint:-options -source 8 -target 8 -bootclasspath $AJ -classpath $AJ -d obj gen/com/smore/hub/R.java src/com/smore/hub/MainActivity.java
dalvik-exchange --dex --output=bin/classes.dex obj
aapt package -f -M AndroidManifest.xml -S res -A assets -I $AJ -F bin/unsigned.apk
(cd bin && aapt add unsigned.apk classes.dex >/dev/null)
zipalign -f -p 4 bin/unsigned.apk bin/aligned.apk
apksigner sign --ks smore.p12 --ks-pass pass:smorehub --ks-key-alias smore --out bin/SmoreHub.apk bin/aligned.apk
apksigner verify --verbose bin/SmoreHub.apk | head -6
aapt dump badging bin/SmoreHub.apk | grep -E "package|sdkVersion|application-label|launchable|application-icon-160"
