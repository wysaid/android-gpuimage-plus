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

if isMingW; then
    echo "Detected MingW, fix cmd.exe path..." >&2
    # Bug issue: <https://stackoverflow.com/questions/11865085/out-of-a-git-console-how-do-i-execute-a-batch-file-and-then-return-to-git-conso>
    # Tested on Git Bash - Git for Windows v2.42.0(2)
    # Do not work: cmd /C $@
    # Do not work too: cmd "/C $@"
    # Workaround below:
    function cmd() {
        echo "mingw call cmd start: 'cmd.exe $(echo $@)'" >&2
        cmd.exe "$(echo $@)"
        echo "mingw call cmd end: 'cmd.exe $(echo $@)'" >&2
    }

elif isWindows && ! command -v cmd &>/dev/null; then
    echo "cmd not found, detect windows native cmd.exe"
    if [[ -f "/mnt/c/Windows/system32/cmd.exe" ]]; then
        function cmd() {
            /mnt/c/Windows/system32/cmd.exe $@
        }
    elif [[ -f "/cygdrive/c/Windows/system32/cmd.exe" ]]; then
        # Not tested
        function cmd() {
            /cygdrive/c/Windows/system32/cmd.exe $@
        }
    fi
fi

function getEnvironmentVariable() {
    echo "getEnvironmentVariable $@" >&2
    VAR_NAME=$@
    RET_VALUE=
    if isWindows && command -v cmd &>/dev/null; then
        RET_VALUE=$(cmd /C set $VAR_NAME | sed 's/.*=//')
    fi

    if [[ -z "$RET_VALUE" ]]; then
        RET_VALUE=${!VAR_NAME}
    fi

    if [[ -n "$RET_VALUE" ]]; then
        echo $RET_VALUE
    fi
}

function runGradleCommand() {

    if isWindows && [[ -f "$REPO_DIR/local.properties" ]] && [[ -n "$(grep -i "sdk.dir=" "$REPO_DIR/local.properties" | grep -E ':(\\|/)')" ]]; then
        # Run with Windows Native
        echo "Perform Windows Native Gradle Func: "
        cmd /C gradlew.bat $@ && exit 0

        # Go ahead if failed.
        echo "Perform Windows Native Gradle Failed, Go with current shell env"
        export GRADLE_RUN_WIN_FAILED=true
    fi

    echo "Perform Gradle Func: "
    if ! ./gradlew $@ && [[ -z "$GRADLE_RUN_WIN_FAILED" ]]; then
        # Try again with Windows Native
        command -v cmd &>/dev/null && cmd /C gradlew.bat $@
    fi
}
