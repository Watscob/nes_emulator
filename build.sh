#!/bin/bash

BUILD=false
CLEAN=false

BUILD_DIR="build"

help()
{
    echo "./build.sh [OPTION]+"
    echo "      --[b]uild"
    echo "      --[c]lean"
    echo "      --[h]elp"
}

VALID_ARGS=$(getopt -o bch --long build,clean,help -- "$@")
if [[ $? -ne 0 ]]; then
    exit 1;
fi

eval set -- "$VALID_ARGS"
while [ : ]; do
    case "$1" in
        -b | --build)
            BUILD=true
            shift
            ;;
        -c | --clean)
            CLEAN=true
            shift
            ;;
        -h | --help)
            help
            exit 1
            ;;
        --)
            shift
            break
            ;;
    esac
done

if $CLEAN; then
    rm -rf $BUILD_DIR
fi

if $BUILD; then
    if [ ! -d $BUILD_DIR ]; then
        mkdir -p $BUILD_DIR
    fi

    cd $BUILD_DIR
    cmake ..

    cmake --build . -j$(nproc)
fi
