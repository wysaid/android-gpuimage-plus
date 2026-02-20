#!/usr/bin/env bash

if [[ $# -eq 0 ]]; then
    echo "usage: [--debug|--release] [--clean] [--enable-cmake] [--setup-project] [--build] [--run]"
    exit 0
fi

cd "$(dirname "$0")"
THIS_DIR="$(pwd)"
PROJECT_DIR="$THIS_DIR"
ADB_COMMAND="$PROJECT_DIR/utils/adb_command.sh"

source "$PROJECT_DIR/utils/platform_utils.sh"

export PACKAGE_NAME="org.wysaid.cgeDemo"
export LAUNCH_ACTIVITY="MainActivity"

if [[ -z "$GRADLEW_RUN_TASK" ]]; then
    export GRADLEW_RUN_TASK="installDebug"
fi

if [[ -z "$ANDROID_BUILD_TYPE" ]]; then
    export ANDROID_BUILD_TYPE="assembleDebug"
fi

function setupProject() {
    if [[ -f "$PROJECT_DIR/local.properties" ]] && grep -E '^usingCMakeCompile=true' "$PROJECT_DIR/local.properties"; then
        echo "Using cmake, skip ndk build..."
    else
        bash "$PROJECT_DIR/library/src/main/jni/buildJNI" || exit 1
    fi
}

# Resolve the adb target device and store as an array in ADB_TARGET_ARGS.
# Priority: ADB_SERIAL env var > physical device (USB/wireless) > emulator.
# If only one device is connected, no -s flag is needed.
# Usage: resolveAdbTarget && . "$ADB_COMMAND" "${ADB_TARGET_ARGS[@]}" shell ...
function resolveAdbTarget() {
    ADB_TARGET_ARGS=()

    # Allow manual override via environment variable
    if [[ -n "$ADB_SERIAL" ]]; then
        ADB_TARGET_ARGS=(-s "$ADB_SERIAL")
        echo "Using device specified by ADB_SERIAL: $ADB_SERIAL"
        return 0
    fi

    # Parse connected devices (exclude header and empty lines, keep only "device" state)
    local devices_output
    devices_output=$(. "$ADB_COMMAND" devices 2>/dev/null | grep -v 'List of devices' | grep -v '^$' | grep $'\tdevice$')

    local device_count=0
    [[ -n "$devices_output" ]] && device_count=$(echo "$devices_output" | wc -l | tr -d ' ')

    if [[ $device_count -eq 0 ]]; then
        echo "No devices connected." >&2
        return 1
    fi

    if [[ $device_count -eq 1 ]]; then
        # Only one device: adb auto-selects, no -s needed
        return 0
    fi

    # Multiple devices: prefer physical devices (non-emulator)
    # Emulator serials always start with "emulator-"
    local physical_serials
    physical_serials=$(echo "$devices_output" | grep -v '^emulator-' | awk '{print $1}')
    local physical_count=0
    [[ -n "$physical_serials" ]] && physical_count=$(echo "$physical_serials" | wc -l | tr -d ' ')

    local selected_serial
    if [[ $physical_count -ge 1 ]]; then
        selected_serial=$(echo "$physical_serials" | head -1)
        if [[ $physical_count -gt 1 ]]; then
            echo "Warning: Multiple physical devices found. Using: $selected_serial" >&2
            echo "  Set ADB_SERIAL env var to override. All devices:" >&2
            echo "$physical_serials" | sed 's/^/    /' >&2
        fi
    else
        # No physical device, fall back to first emulator
        selected_serial=$(echo "$devices_output" | awk '{print $1}' | head -1)
        echo "No physical device found, using emulator: $selected_serial" >&2
    fi

    ADB_TARGET_ARGS=(-s "$selected_serial")
}

function runAndroidApp() {
    resolveAdbTarget || return 1

    if . "$ADB_COMMAND" "${ADB_TARGET_ARGS[@]}" shell am start -n "$PACKAGE_NAME/$PACKAGE_NAME.$LAUNCH_ACTIVITY"; then
        if [[ -z "$(ps -ef | grep -i adb | grep -v grep | grep logcat)" ]]; then
            . "$ADB_COMMAND" "${ADB_TARGET_ARGS[@]}" logcat -c
            if [[ $(uname -s) == "Linux" ]] || [[ $(uname -s) == "Darwin" ]]; then
                APP_PROC_ID=$(. "$ADB_COMMAND" "${ADB_TARGET_ARGS[@]}" shell ps | grep org.wysaid.cgeDemo | tr -s ' ' | cut -d' ' -f2)
                if [[ -n "$APP_PROC_ID" ]]; then
                    . "$ADB_COMMAND" "${ADB_TARGET_ARGS[@]}" logcat | grep -F "$APP_PROC_ID"
                else
                    echo "Can not find proc id of org.wysaid.cgeDemo"
                fi
            else
                . "$ADB_COMMAND" "${ADB_TARGET_ARGS[@]}" logcat | grep -iE "(cge|demo|wysaid| E |crash)"
            fi
        fi
    else
        echo "Launch $PACKAGE_NAME/$PACKAGE_NAME.$LAUNCH_ACTIVITY failed."
    fi
}

function cleanProject() {
    runGradleCommand clean --refresh-dependencies
}

function buildProject() {
    if ! runGradleCommand -p cgeDemo "$ANDROID_BUILD_TYPE"; then

        echo "Failed to run: ./gradlew -p cgeDemo $ANDROID_BUILD_TYPE"
        echo "Please run the following command and try again:"
        echo "---"
        echo "$THIS_DIR/tasks.sh --setup-project"
        echo "---"
        exit 1
    fi

    GENERATED_APK_FILE=$(find "$THIS_DIR/cgeDemo/build" -iname "*.apk" | grep -i "${ANDROID_BUILD_TYPE/assemble/}")
    echo "apk generated at: $GENERATED_APK_FILE"

    resolveAdbTarget
    if [[ -n "$GRADLEW_RUN_TASK" ]] && [[ ${#ADB_TARGET_ARGS[@]} -gt 0 || $(. "$ADB_COMMAND" devices | grep -v 'List' | grep -vE '^$' | grep $'\tdevice$' | wc -l | tr -d ' ') -ne 0 ]]; then
        if [[ "$GRADLEW_RUN_TASK" == "installRelease" ]]; then
            # release can not be installed directly. do adb install.
            . "$ADB_COMMAND" "${ADB_TARGET_ARGS[@]}" install "$GENERATED_APK_FILE"
        else
            if ! runGradleCommand -p cgeDemo "$GRADLEW_RUN_TASK"; then
                echo "Install failed." >&2
            fi
        fi
    else
        echo "No device connected, skip installation."
    fi
}

function patchAndroidSDKLocation() {
    VAR_ENV_VALUE=$(getEnvironmentVariable $@)
    echo VAR_ENV_VALUE=$VAR_ENV_VALUE
    if [[ -n "$VAR_ENV_VALUE" ]]; then
        VAR_ENV_VALUE=$(echo $VAR_ENV_VALUE | tr '\\' '/')
        echo "sdk.dir=$VAR_ENV_VALUE" >>local.properties
        return 0
    fi
    return 1
}

if [[ ! -f "local.properties" ]] && ! grep "sdk.dir=" local.properties &>/dev/null; then
    if ! (patchAndroidSDKLocation ANDROID_HOME ||
        patchAndroidSDKLocation ANDROID_SDK_ROOT); then
        echo "sdk.dir is missing in 'local.properties' and env var ANDROID_HOME is not defined." >&2
    fi
fi

function changeProperty() {
    TARGET_FILE="$1"
    MATCH_PATTERN="$2"
    SED_PATTERN="$3"
    DEFAULT_VALUE="$4"
    if [[ -f "$TARGET_FILE" ]] && grep -E "$MATCH_PATTERN" "$TARGET_FILE" >/dev/null; then
        echo "Change property in $TARGET_FILE, pattern $SED_PATTERN"
        # Stupid diff between the command.
        if [[ "$(uname -s)" == "Darwin" ]]; then # Mac OS
            sed -I "" -E "$SED_PATTERN" "$TARGET_FILE"
        else
            sed -i"" -E "$SED_PATTERN" "$TARGET_FILE"
        fi
    else
        echo "$DEFAULT_VALUE" >>"$TARGET_FILE"
    fi
}

while [[ $# > 0 ]]; do

    PARSE_KEY="$1"

    case "$PARSE_KEY" in
    --rebuild-android-demo)
        cd "$PROJECT_DIR" && git clean -ffdx
        cleanProject
        setupProject
        buildProject
        shift
        ;;
    --no-install)
        export GRADLEW_RUN_TASK=""
        shift
        ;;
    --release)
        export ANDROID_BUILD_TYPE="assembleRelease"
        # ANDROID_BUILD_TYPE="assembleDebug" # use this if the release apk can not be installed.
        export GRADLEW_RUN_TASK="installRelease"
        changeProperty "local.properties" '^usingCMakeCompileDebug=' 's/usingCMakeCompileDebug=.*/usingCMakeCompileDebug=false/' 'usingCMakeCompileDebug=false'
        shift
        ;;
    --debug)
        export ANDROID_BUILD_TYPE="assembleDebug"
        export GRADLEW_RUN_TASK="installDebug"
        changeProperty "local.properties" '^usingCMakeCompileDebug=' 's/usingCMakeCompileDebug=.*/usingCMakeCompileDebug=true/' 'usingCMakeCompileDebug=true'
        shift
        ;;
    --setup-project)
        setupProject
        runGradleCommand signingReport
        exit 0 # setup then stop.
        ;;
    --clean)
        echo "clean"
        cleanProject
        shift # past argument
        ;;
    --build)
        echo "build"
        setupProject
        buildProject
        shift # past argument
        ;;
    --run)
        echo "run android demo"
        runAndroidApp
        shift # past argument
        ;;
    --enable-cmake)
        changeProperty "local.properties" '^usingCMakeCompile=' 's/usingCMakeCompile=.*/usingCMakeCompile=true/' 'usingCMakeCompile=true'
        shift # past argument
        ;;
    --enable-video-module)
        changeProperty "local.properties" '^disableVideoModule=' 's/disableVideoModule=.*/disableVideoModule=false/' 'disableVideoModule=false'
        shift # past argument
        ;;
    --disable-video-module)
        changeProperty "local.properties" '^disableVideoModule=' 's/disableVideoModule=.*/disableVideoModule=true/' 'disableVideoModule=true'
        shift # past argument
        ;;
    --enable-16kb-page-size)
        changeProperty "local.properties" '^enable16kPageSizes=' 's/enable16kPageSizes=.*/enable16kPageSizes=true/' 'enable16kPageSizes=true'
        shift # past argument
        ;;
    --disable-16kb-page-size)
        changeProperty "local.properties" '^enable16kPageSizes=' 's/enable16kPageSizes=.*/enable16kPageSizes=false/' 'enable16kPageSizes=false'
        shift # past argument
        ;;
    --enable-gradle-libs-test)
        changeProperty "local.properties" '^testDemoWithGradleLibs=' 's/testDemoWithGradleLibs=.*/testDemoWithGradleLibs=true/' 'testDemoWithGradleLibs=true'
        shift # past argument
        ;;
    --disable-gradle-libs-test)
        changeProperty "local.properties" '^testDemoWithGradleLibs=' 's/testDemoWithGradleLibs=.*/testDemoWithGradleLibs=false/' 'testDemoWithGradleLibs=false'
        shift # past argument
        ;;
    --publish)
        echo "publish"
        git clean -fdx library
        changeProperty "local.properties" '^usingCMakeCompileDebug=' 's/usingCMakeCompileDebug=.*/usingCMakeCompileDebug=false/' 'usingCMakeCompileDebug=false'
        changeProperty "local.properties" '^deployArtifacts=' 's/deployArtifacts=.*/deployArtifacts=true/' 'disableVideoModule=true'
        ./gradlew assembleRelease publish
        shift
        ;;
    --publish-all)
        echo "publish all (normal, min, 16k, 16k-min)"
        set -e
        # 1. Normal (with video module, no 16k)
        bash "$0" --enable-cmake --enable-video-module --disable-16kb-page-size --publish || exit 1
        # 2. Min (no video module, no 16k)
        bash "$0" --enable-cmake --disable-video-module --disable-16kb-page-size --publish || exit 1
        # 3. 16k (with video module, 16k)
        bash "$0" --enable-cmake --enable-video-module --enable-16kb-page-size --publish || exit 1
        # 4. 16k-min (no video module, 16k)
        bash "$0" --enable-cmake --disable-video-module --enable-16kb-page-size --publish || exit 1

        # --- Validate all artifacts ---
        PUBLISH_VERSION=$(grep 'versionName' "$PROJECT_DIR/build.gradle" | grep -oE '[0-9]+\.[0-9]+\.[0-9]+' | head -1)
        LOCAL_REPO_DIR=$(grep '^localRepoDir=' "$PROJECT_DIR/local.properties" | cut -d'=' -f2)
        ARTIFACT_BASE_DIR="$LOCAL_REPO_DIR/org/wysaid/gpuimage-plus"
        ARTIFACT_ID="gpuimage-plus"

        declare -a EXPECTED_VERSIONS=(
            "$PUBLISH_VERSION"
            "${PUBLISH_VERSION}-min"
            "${PUBLISH_VERSION}-16k"
            "${PUBLISH_VERSION}-16k-min"
        )

        echo ""
        echo "=== Validating published artifacts (version: $PUBLISH_VERSION) ==="
        VALIDATION_FAILED=0
        for VER in "${EXPECTED_VERSIONS[@]}"; do
            AAR_FILE="$ARTIFACT_BASE_DIR/$VER/${ARTIFACT_ID}-${VER}.aar"
            POM_FILE="$ARTIFACT_BASE_DIR/$VER/${ARTIFACT_ID}-${VER}.pom"
            if [[ -f "$AAR_FILE" ]] && [[ -f "$POM_FILE" ]]; then
                AAR_SIZE=$(wc -c <"$AAR_FILE" | tr -d ' ')
                echo "  [OK] $VER  (aar: ${AAR_SIZE} bytes)"
            else
                echo "  [MISSING] $VER"
                [[ ! -f "$AAR_FILE" ]] && echo "       missing: $AAR_FILE"
                [[ ! -f "$POM_FILE" ]] && echo "       missing: $POM_FILE"
                VALIDATION_FAILED=1
            fi
        done

        if [[ $VALIDATION_FAILED -ne 0 ]]; then
            echo ""
            echo "ERROR: Some artifacts are missing! See above for details." >&2
            exit 1
        fi

        echo ""
        echo "All 4 artifacts published and validated successfully."
        shift
        ;;
    *)
        echo "Invalid argument $PARSE_KEY..."
        exit 1
        ;;

    esac

done

# if [[ -z "$SKIP_GRADLEW_TASKS" ]]; then

#     if [[ -n "$CLEAN_GRADLEW_TASKS" ]]; then
#         ./gradlew clean
#     fi

#     buildProject
# fi

# if [[ -n "$GRADLEW_RUN_TASK" ]] || [[ -n $SKIP_GRADLEW_TASKS ]]; then
#     runAndroidApp
# else
#     echo "apk generated at:"
#     find "$THIS_DIR/cgeDemo/build" -iname "*.apk"
# fi
