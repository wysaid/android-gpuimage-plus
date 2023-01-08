#!/usr/bin/env bash

if [[ $# -eq 0 ]]; then
    echo "usage: [--clean] [--build] [--run]"
fi

cd "$(dirname "$0")"
THIS_DIR="$(pwd)"
PROJECT_DIR="$THIS_DIR"
ADB_COMMAND="$PROJECT_DIR/utils/adb_command.sh"

export PACKAGE_NAME="org.wysaid.cgeDemo"
export LAUNCH_ACTIVITY="MainActivity"
export GRADLEW_RUN_TASK="installDebug"
export ANDROID_BUILD_TYPE="assembleDebug"

function setupProject() {
    if [[ -f "$PROJECT_DIR/local.properties" ]] && grep -E '^usingCMakeCompile=true' "$PROJECT_DIR/local.properties"; then
        echo "Using cmake, skip ndk build..."
    else
        bash "$PROJECT_DIR/library/src/main/jni/buildJNI"
    fi
}

function runAndroidApp() {
    . "$ADB_COMMAND" -d shell am start -n "$PACKAGE_NAME/$PACKAGE_NAME.$LAUNCH_ACTIVITY" &&
        . "$ADB_COMMAND" -d logcat | grep -iE "(cge|demo|wysaid)"
}

function cleanProject() {
    ./gradlew clean --refresh-dependencies
}

function buildProject() {
    if ! ./gradlew -p cgeDemo "$ANDROID_BUILD_TYPE"; then

        echo "Failed to run..."
        echo "Please run the following command and try again:"
        echo "---"
        echo "$THIS_DIR/vscode_tasks.sh --setup-project"
        echo "---"
        exit 1
    fi

    echo "apk generated at:"
    find "$THIS_DIR/cgeDemo/build" -iname "*.apk" | grep -i "${ANDROID_BUILD_TYPE/assemble/}"

    if [[ -n "$GRADLEW_RUN_TASK" ]] && [[ $(. "$ADB_COMMAND" -d devices | grep -v 'List' | tr -d ' \n' | wc -l) -ne 0 ]]; then
        if ! ./gradlew -p cgeDemo "$GRADLEW_RUN_TASK"; then
            echo "Install failed." >&2
        fi
    else
        echo "No device connected, skip installation."
    fi
}

if [[ ! -f "local.properties" ]]; then
    if [[ -n "$ANDROID_SDK_HOME" ]]; then
        echo "sdk.dir=$ANDROID_SDK_HOME" >>local.properties
    elif [[ -n "$ANDROID_SDK_ROOT" ]]; then
        echo "sdk.dir=$ANDROID_SDK_ROOT" >>local.properties
    elif [[ -n "$ANDROID_SDK" ]]; then
        echo "sdk.dir=$ANDROID_SDK" >>local.properties
    else
        echo "Can't find ANDROID_SDK, Please setup 'local.properties'" >&2
    fi
fi

while [[ $# > 0 ]]; do

    PARSE_KEY="$1"

    case "$PARSE_KEY" in
    --run-only)
        runAndroidApp
        shift
        ;;
    --rebuild-android-demo)
        cd "$PROJECT_DIR" && git clean -ffdx
        cleanProject
        setupProject
        buildProject
        shift
        ;;
    --release)
        ANDROID_BUILD_TYPE="assembleRelease"
        GRADLEW_RUN_TASK="installRelease"
        shift
        ;;
    --debug)
        ANDROID_BUILD_TYPE="assembleDebug"
        GRADLEW_RUN_TASK="installDebug"
        shift
        ;;
    --setup-project)
        setupProject
        exit 0 # setup 的情况下不编译.
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
        if [[ -f "local.properties" ]] && grep -E '^usingCMakeCompile=' "local.properties" >/dev/null; then
            # Stupid diff between the command.
            if [[ "$(uname -s)" == "Darwin" ]]; then # Mac OS
                sed -I "" -E 's/usingCMakeCompile=.*/usingCMakeCompile=true/' "local.properties"
            else
                sed -i"" -E 's/usingCMakeCompile=.*/usingCMakeCompile=true/' "local.properties"
            fi
        else
            echo "usingCMakeCompile=true" >>"local.properties"
        fi
        shift # past argument
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
