#!/bin/bash
# Displexity Android APK Builder
# Requires: Android NDK, SDK, and build tools

set -e

INPUT_FILE="$1"
PACKAGE_NAME="${2:-com.displexity.app}"
APP_NAME="${3:-DisplexityApp}"

if [ -z "$INPUT_FILE" ]; then
    echo "Usage: build_apk.sh <input.dis> [package.name] [AppName]"
    exit 1
fi

BASENAME=$(basename "$INPUT_FILE" .dis)
BUILD_DIR="apk_build_${BASENAME}"

# Check for Android NDK
if [ -z "$ANDROID_NDK_HOME" ]; then
    echo "Error: ANDROID_NDK_HOME not set"
    echo "Please install Android NDK and set the environment variable"
    exit 1
fi

echo "Building Android APK: ${APP_NAME}.apk"

# Create project structure
mkdir -p "$BUILD_DIR/app/src/main/java/$(echo $PACKAGE_NAME | tr '.' '/')"
mkdir -p "$BUILD_DIR/app/src/main/jni"
mkdir -p "$BUILD_DIR/app/src/main/res/values"
mkdir -p "$BUILD_DIR/app/src/main/res/layout"

# Generate C code
disp "$INPUT_FILE" -emit-c -o "$BUILD_DIR/app/src/main/jni/native.c" -no-log

# Create Android.mk
cat > "$BUILD_DIR/app/src/main/jni/Android.mk" << EOF
LOCAL_PATH := \$(call my-dir)
include \$(CLEAR_VARS)
LOCAL_MODULE := displexity
LOCAL_SRC_FILES := native.c
LOCAL_LDLIBS := -llog
include \$(BUILD_SHARED_LIBRARY)
EOF

# Create Application.mk
cat > "$BUILD_DIR/app/src/main/jni/Application.mk" << EOF
APP_ABI := armeabi-v7a arm64-v8a x86 x86_64
APP_PLATFORM := android-21
EOF

# Create AndroidManifest.xml
cat > "$BUILD_DIR/app/src/main/AndroidManifest.xml" << EOF
<?xml version="1.0" encoding="utf-8"?>
<manifest xmlns:android="http://schemas.android.com/apk/res/android"
    package="$PACKAGE_NAME">
    <application
        android:label="$APP_NAME"
        android:theme="@android:style/Theme.NoTitleBar">
        <activity android:name=".MainActivity"
            android:exported="true">
            <intent-filter>
                <action android:name="android.intent.action.MAIN" />
                <category android:name="android.intent.category.LAUNCHER" />
            </intent-filter>
        </activity>
    </application>
</manifest>
EOF

# Create MainActivity.java
cat > "$BUILD_DIR/app/src/main/java/$(echo $PACKAGE_NAME | tr '.' '/')/MainActivity.java" << EOF
package $PACKAGE_NAME;

import android.app.Activity;
import android.os.Bundle;
import android.widget.TextView;

public class MainActivity extends Activity {
    static {
        System.loadLibrary("displexity");
    }
    
    public native String runDisplexity();
    
    @Override
    protected void onCreate(Bundle savedInstanceState) {
        super.onCreate(savedInstanceState);
        TextView tv = new TextView(this);
        tv.setText("Displexity App Running");
        setContentView(tv);
    }
}
EOF

echo "Android project created in: $BUILD_DIR"
echo "To build APK:"
echo "  1. cd $BUILD_DIR"
echo "  2. ndk-build"
echo "  3. Use Android Studio or gradle to build APK"
echo ""
echo "Note: Full APK build requires Android SDK and build tools"
