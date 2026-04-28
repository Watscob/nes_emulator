#!/bin/bash

BUILD=false
CLEAN=false
WASM=""

BUILD_DIR="build"

help()
{
    echo "./build.sh [OPTION]+"
    echo "      --[b]uild"
    echo "      --[c]lean"
    echo "      --[w]asm"
    echo "      --[h]elp"
}

VALID_ARGS=$(getopt -o bcwh --long build,clean,wasm,help -- "$@")
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
        -w | --wasm)
            WASM=emcmake
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

# Always run clang-format
find include src tests -name "*.hpp" -o -name "*.cpp" | xargs clang-format -i

if $CLEAN; then
    rm -rf $BUILD_DIR
fi

if $BUILD; then
    if [ ! -d $BUILD_DIR ]; then
        mkdir -p $BUILD_DIR
    fi

    cd $BUILD_DIR
    $WASM cmake ..

    cmake --build . -j$(nproc)
fi
