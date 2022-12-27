#!/usr/bin/env bash

if ! command -v adb &>/dev/null; then

    if [[ -n "$ANDROID_SDK_HOME" ]]; then
        export PATH=$PATH:$ANDROID_SDK_HOME/platform-tools
    elif [[ -n "$ANDROID_SDK_ROOT" ]]; then
        export PATH=$PATH:$ANDROID_SDK_ROOT/platform-tools
    fi

    if ! command -v adb &>/dev/null; then
        echo "can't find adb!'"
        exit 1
    fi
fi

if [[ $# -gt 0 ]]; then
    adb $@
else
    command -v adb
fi
