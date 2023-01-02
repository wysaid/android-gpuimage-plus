#!/usr/bin/env bash

THIS_DIR="$(dirname $0)"
cd $THIS_DIR
PROJECT_DIR=$(pwd)

if [[ $# -eq 0 ]]; then
    echo "usage: [--clean] [--build] [--run]"
fi

while [[ $# > 0 ]]; do

    PARSE_KEY="$1"

    case "$PARSE_KEY" in
    --clean)
        echo "clean"

        shift # past argument
        ;;
    --build)
        echo "build"
        bash "$PROJECT_DIR/library/src/main/jni/buildJNI"
        . "$PROJECT_DIR/run_android_app.sh" --build-only
        shift # past argument
        ;;
    --run)
        echo "run android demo"
        . "$PROJECT_DIR/run_android_app.sh"
        shift # past argument
        ;;
    *)
        echo "unknown option $PARSE_KEY..."
        exit 1
        ;;

    esac

done
