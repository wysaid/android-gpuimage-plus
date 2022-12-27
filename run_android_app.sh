#!/usr/bin/env bash

cd "$(dirname "$0")"
THIS_DIR="$(pwd)"
PROJECT_DIR="$THIS_DIR"
ADB_COMMAND="$PROJECT_DIR/utils/adb_command.sh"

export PACKAGE_NAME="org.wysaid.cgeDemo"
export LAUNCH_ACTIVITY="MainActivity"
GRADLEW_RUN_TASK="installDebug"

function setupProject() {
    bash "$PROJECT_DIR/library/src/main/jni/buildJNI"
}

while [[ $# > 0 ]]; do

    PARSE_KEY="$1"

    case "$PARSE_KEY" in
    --build-only)
        GRADLEW_RUN_TASK=""
        shift
        ;;
    --run-only)
        SKIP_GRADLEW_TASKS=true
        shift
        ;;
    --rebuild-android-demo)
        cd "$PROJECT_DIR" && git clean -ffdx
        setupProject
        CLEAN_GRADLEW_TASKS=true
        GRADLEW_RUN_TASK="${GRADLEW_RUN_TASK} --refresh-dependencies"
        shift
        ;;
    --setup-project)
        setupProject
        exit 0 # setup 的情况下不编译.
        ;;
    *)
        echo "Invalid argument $PARSE_KEY..."
        shift
        ;;
    esac

done

if [[ ! -f "local.properties" ]]; then
    if [[ -n "$ANDROID_SDK_HOME" ]]; then
        echo "sdk.dir=$ANDROID_SDK_HOME" >>local.properties
    elif [[ -n "$ANDROID_SDK_ROOT" ]]; then
        echo "sdk.dir=$ANDROID_SDK_ROOT" >>local.properties
    elif [[ -n "$ANDROID_SDK" ]]; then
        echo "sdk.dir=$ANDROID_SDK" >>local.properties
    fi
fi

if [[ -z "$SKIP_GRADLEW_TASKS" ]]; then

    if [[ -n "$CLEAN_GRADLEW_TASKS" ]]; then
        ./gradlew clean
    fi

    if ! ./gradlew -p cgeDemo assembleDebug $GRADLEW_RUN_TASK; then

        echo "Failed to run..."
        echo "Please run the following command and try again:"
        echo "---"
        echo "$THIS_DIR/run_android_app.sh --setup-project"
        echo "---"
        setupProject
        exit 1
    fi
fi

if [[ -n "$GRADLEW_RUN_TASK" ]] || [[ -n $SKIP_GRADLEW_TASKS ]]; then
    . "$ADB_COMMAND" -d shell am start -n "$PACKAGE_NAME/$PACKAGE_NAME.$LAUNCH_ACTIVITY" &&
        . "$ADB_COMMAND" -d logcat | grep -iE "(cge|demo|wysaid)"
else
    echo "apk generated at:"
    find "$THIS_DIR/cgeDemo/build" -iname "*.apk"
fi
