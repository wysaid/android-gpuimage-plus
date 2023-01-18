#!/usr/bin/env bash

if ! command -v realpath &>/dev/null; then
    function realpath() {
        if [[ $# -ne 0 ]]; then
            pushd . >/dev/null
            cd $@ && pwd
            popd >/dev/null
        fi
    }
fi

REPO_DIR=$(git rev-parse --show-toplevel)

function isWsl() {
    [[ -d "/mnt/c" ]] || command -v wslpath &>/dev/null
}

function isMingW() {
    [[ -d "/c" ]]
}

function isCygwin() {
    [[ -d "/cygdrive/c" ]]
}

function isWindows() {
    # check mingw and cygwin
    isWsl || isMingW || isCygwin
}

function isMacOSX() {
    [[ "$(uname -s)" == "Darwin" ]]
}

function isLinux() {
    [[ "$(uname -s)" == "Linux" ]]
}

function getEnvironmentVariable() {
    VAR_NAME="$@"
    RET_VALUE=
    if isWindows && command -v cmd &>/dev/null; then
        # Detect native var first
        RET_VALUE=$(cmd /C set "$VAR_NAME" | sed 's/.*=//')
    fi

    if [[ -z "$RET_VALUE" ]]; then
        RET_VALUE=${VAR_NAME}
    fi

    if [[ -n "$RET_VALUE" ]]; then
        echo $RET_VALUE
    fi
}

if isWindows && ! command -v cmd &>/dev/null; then
    if [[ -f "/mnt/c/Windows/system32/cmd.exe" ]]; then
        function cmd() {
            /mnt/c/Windows/system32/cmd.exe $@
        }
    elif [[ -f "/c/Windows/system32/cmd.exe" ]]; then
        function cmd() {
            /c/Windows/system32/cmd.exe $@
        }
    elif [[ -f "/cygdrive/c/Windows/system32/cmd.exe" ]]; then
        function cmd() {
            /cygdrive/c/Windows/system32/cmd.exe $@
        }
    fi
fi

function runGradleCommand() {

    if isWindows && [[ -f "$REPO_DIR/local.properties" ]] && [[ -n "$(grep -i "sdk.dir=" "$REPO_DIR/local.properties" | grep -E ':(\\|/)')" ]]; then
        # Run with Windows Native
        echo "Perform Windows Native Gradle Func: "
        cmd /C gradlew $@ && exit 0
        # Go ahead if failed.
        echo "Perform Windows Native Gradle Failed, Go with current shell env"
        export GRADLE_RUN_WIN_FAILED=true
    fi

    if ! ./gradlew $@ && [[ -z "$GRADLE_RUN_WIN_FAILED" ]]; then
        # Try again with Windows Native
        command -v cmd &>/dev/null && cmd /C gradlew $@
    fi
}
