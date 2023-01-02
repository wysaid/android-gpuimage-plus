#!/usr/bin/env bash

cd "$(dirname "$0")"

THIS_DIR="$(pwd)"
cd ..

REPO_DIR="$(pwd)"

if ! [ -x "$(command -v clang-format)" ]; then
    echo 'clang-format is not installed.' >&2
    exit 1
fi
CF_VER=$(clang-format --version | sed -E "s/([^0-9]*)([0-9]+)\.(.*)/\2/")
if (($CF_VER < 11)); then
    echo '[ERROR] clang-format version at least 11, please run [brew update && brew upgrade clang-format]'
    exit 1
fi

if [[ "$1" == "--only-changed" ]]; then
    # 仅修改当前改变了的文件

    declare -a CHANGED_FILES=()

    while IFS=$'\n' read -r -a line; do
        echo "Find cached file: $line"
        CHANGED_FILES+=("$line")
    done < <(git diff --cached --name-only | grep -E '(.*)\.(h|hpp|cpp|m|mm)$' | grep -ivE "\.(vert|frag|sl)\..*\.(h|cpp)$")

    while IFS=$'\n' read -r -a line; do
        echo "Find changed file: $line"
        CHANGED_FILES+=("$line")
    done < <(git diff --name-only | grep -E '(.*)\.(h|hpp|cpp|m|mm)$' | grep -ivE "\.(vert|frag|sl)\..*\.(h|cpp)$")

    declare -a UNIQUE_CHANGED_FILES=()

    if [[ "${#CHANGED_FILES[@]}" -ne 0 ]]; then
        while IFS=$'\n' read -r -a line; do
            UNIQUE_CHANGED_FILES+=("$line")
            echo "Processing file $line"
            clang-format -i --style=file $line
        done < <(printf "%s\n" ${CHANGED_FILES[@]} | sort | uniq)
    fi

    echo "Total changed files: ${#UNIQUE_CHANGED_FILES[@]}"
else
    cd "$REPO_DIR"
    subDir="library/src/main/jni"
    findDir=(cge interface custom)
    for name in ${findDir[@]}; do
        find -E "$subDir/$name" -type f -regex '(.*)\.(h|hpp|cpp|mm)$' | grep -ivE "\.(vert|frag|sl)\..*\.(h|cpp)$" | xargs clang-format -i --style=file
    done
fi
