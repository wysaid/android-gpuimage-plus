#!/usr/bin/env bash

if ! command -v adb &>/dev/null; then

    if [[ -n "$ANDROID_HOME" ]] && [[ -d "$ANDROID_HOME/platform-tools" ]]; then
        export PATH=$PATH:$ANDROID_HOME/platform-tools
    elif [[ -n "$ANDROID_SDK_ROOT" ]] && [[ -d "$ANDROID_SDK_ROOT/platform-tools" ]]; then
        export PATH=$PATH:$ANDROID_SDK_ROOT/platform-tools
    elif [[ -n "$ANDROID_SDK_HOME" ]]; then
        if [[ -d "$ANDROID_SDK_HOME/platform-tools" ]]; then
            export PATH=$PATH:$ANDROID_SDK_HOME/platform-tools
        elif [[ -d "$ANDROID_SDK_HOME/../platform-tools" ]]; then
            export PATH=$PATH:$(realpath $ANDROID_SDK_HOME/../platform-tools)
        fi
    fi

    if ! command -v adb &>/dev/null; then
        echo "$(pwd): Can't find adb!'"
        exit 1
    fi
fi

if [[ $# -gt 0 ]]; then
    adb $@
else
    command -v adb
fi
