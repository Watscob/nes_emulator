#!/bin/bash
docker run --rm -v $(pwd):$(pwd) -w $(pwd) -e DISPLAY=$DISPLAY -v /tmp/.X11-unix:/tmp/.X11-unix -it  build-nes-emulator ./build/src/nes_emulator $@
