#!/bin/bash
# Displexity iOS IPA Builder
# Requires: Xcode Command Line Tools on macOS

set -e

INPUT_FILE="$1"
APP_NAME="${2:-DisplexityApp}"
BUNDLE_ID="${3:-com.displexity.app}"

if [ -z "$INPUT_FILE" ]; then
    echo "Usage: build_ios.sh <input.dis> [AppName] [bundle.id]"
    exit 1
fi

BASENAME=$(basename "$INPUT_FILE" .dis)
BUILD_DIR="ios_build_${BASENAME}"

# Check for Xcode
if [ "$(uname)" != "Darwin" ]; then
    echo "Error: iOS builds require macOS with Xcode"
    echo "Please run this script on a Mac with Xcode installed"
    exit 1
fi

if ! command -v xcodebuild &> /dev/null; then
    echo "Error: Xcode Command Line Tools not found"
    echo "Install with: xcode-select --install"
    exit 1
fi

echo "Building iOS app: ${APP_NAME}.ipa"

# Create Xcode project structure
mkdir -p "$BUILD_DIR/$APP_NAME/$APP_NAME"
mkdir -p "$BUILD_DIR/$APP_NAME/$APP_NAME.xcodeproj"
mkdir -p "$BUILD_DIR/$APP_NAME/$APP_NAME/Base.lproj"

# Generate C code
disp "$INPUT_FILE" -emit-c -o "$BUILD_DIR/$APP_NAME/$APP_NAME/displexity_code.c" -no-log

# Create main.m (Objective-C entry point)
cat > "$BUILD_DIR/$APP_NAME/$APP_NAME/main.m" << 'EOF'
#import <UIKit/UIKit.h>
#import "AppDelegate.h"

int main(int argc, char * argv[]) {
    @autoreleasepool {
        return UIApplicationMain(argc, argv, nil, NSStringFromClass([AppDelegate class]));
    }
}
EOF

# Create AppDelegate.h
cat > "$BUILD_DIR/$APP_NAME/$APP_NAME/AppDelegate.h" << 'EOF'
#import <UIKit/UIKit.h>

@interface AppDelegate : UIResponder <UIApplicationDelegate>
@property (strong, nonatomic) UIWindow *window;
@end
EOF

# Create AppDelegate.m
cat > "$BUILD_DIR/$APP_NAME/$APP_NAME/AppDelegate.m" << EOF
#import "AppDelegate.h"

// Include Displexity generated code
#include "displexity_code.c"

@implementation AppDelegate

- (BOOL)application:(UIApplication *)application didFinishLaunchingWithOptions:(NSDictionary *)launchOptions {
    self.window = [[UIWindow alloc] initWithFrame:[[UIScreen mainScreen] bounds]];
    
    UIViewController *vc = [[UIViewController alloc] init];
    UILabel *label = [[UILabel alloc] initWithFrame:self.window.bounds];
    label.text = @"$APP_NAME Running";
    label.textAlignment = NSTextAlignmentCenter;
    label.backgroundColor = [UIColor whiteColor];
    vc.view = label;
    
    self.window.rootViewController = vc;
    [self.window makeKeyAndVisible];
    
    return YES;
}

@end
EOF

# Create Info.plist
cat > "$BUILD_DIR/$APP_NAME/$APP_NAME/Info.plist" << EOF
<?xml version="1.0" encoding="UTF-8"?>
<!DOCTYPE plist PUBLIC "-//Apple//DTD PLIST 1.0//EN" "http://www.apple.com/DTDs/PropertyList-1.0.dtd">
<plist version="1.0">
<dict>
    <key>CFBundleDevelopmentRegion</key>
    <string>en</string>
    <key>CFBundleExecutable</key>
    <string>$APP_NAME</string>
    <key>CFBundleIdentifier</key>
    <string>$BUNDLE_ID</string>
    <key>CFBundleInfoDictionaryVersion</key>
    <string>6.0</string>
    <key>CFBundleName</key>
    <string>$APP_NAME</string>
    <key>CFBundlePackageType</key>
    <string>APPL</string>
    <key>CFBundleShortVersionString</key>
    <string>1.0</string>
    <key>CFBundleVersion</key>
    <string>1</string>
    <key>LSRequiresIPhoneOS</key>
    <true/>
    <key>UILaunchStoryboardName</key>
    <string>LaunchScreen</string>
    <key>UIRequiredDeviceCapabilities</key>
    <array>
        <string>armv7</string>
    </array>
    <key>UISupportedInterfaceOrientations</key>
    <array>
        <string>UIInterfaceOrientationPortrait</string>
    </array>
</dict>
</plist>
EOF

# Create LaunchScreen.storyboard
cat > "$BUILD_DIR/$APP_NAME/$APP_NAME/Base.lproj/LaunchScreen.storyboard" << 'EOF'
<?xml version="1.0" encoding="UTF-8"?>
<document type="com.apple.InterfaceBuilder3.CocoaTouch.Storyboard.XIB" version="3.0">
    <scenes>
        <scene sceneID="EHf-IW-A2E">
            <objects>
                <viewController id="01J-lp-oVM" sceneMemberID="viewController">
                    <view key="view" contentMode="scaleToFill" id="Ze5-6b-2t3">
                        <rect key="frame" x="0.0" y="0.0" width="375" height="667"/>
                        <color key="backgroundColor" white="1" alpha="1" colorSpace="custom"/>
                    </view>
                </viewController>
                <placeholder placeholderIdentifier="IBFirstResponder" id="iYj-Kq-Ea1" sceneMemberID="firstResponder"/>
            </objects>
        </scene>
    </scenes>
</document>
EOF

# Create project.pbxproj (minimal)
cat > "$BUILD_DIR/$APP_NAME/$APP_NAME.xcodeproj/project.pbxproj" << EOF
// Displexity iOS Project
// Build manually with Xcode or use xcodebuild
EOF

echo "iOS project created in: $BUILD_DIR/$APP_NAME"
echo ""
echo "To build IPA:"
echo "  1. Open $BUILD_DIR/$APP_NAME in Xcode"
echo "  2. Configure signing with your Apple Developer account"
echo "  3. Build for iOS device or Archive for distribution"
echo ""
echo "Or use xcodebuild:"
echo "  cd $BUILD_DIR/$APP_NAME"
echo "  xcodebuild -scheme $APP_NAME -configuration Release"
