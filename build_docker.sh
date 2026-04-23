#!/bin/bash

DOCKERFILE_PATH="Dockerfile"
IMAGE_NAME="build-nes-emulator"
HASH_FILE=".dockerfile_hash"

current_hash=$(sha256sum "$DOCKERFILE_PATH" | cut -d ' ' -f 1)
image_exists=$(docker images -q "$IMAGE_NAME" 2> /dev/null)

if [ -f "$HASH_FILE" ]; then
    previous_hash=$(cat "$HASH_FILE")
else
    previous_hash=""
fi

if [ -z "$image_exists" ] || [ "$current_hash" != "$previous_hash" ]; then
    docker build -t "$IMAGE_NAME" .
    echo "$current_hash" > "$HASH_FILE"
fi

docker run --rm -v $(pwd):$(pwd) -w $(pwd) --user $(id -u):$(id -g) "$IMAGE_NAME" ./build.sh "$@"
