FROM ubuntu:26.04

SHELL [ "bash", "-c" ]

RUN apt-get update &&   \
    apt-get install -y  \
    build-essential     \
    clang-format        \
    cmake               \
    git                 \
    libgtest-dev        \
    libsdl3-dev         \
    python3             \
    nodejs              \
    npm &&              \
    rm -rf /var/lib/apt/lists/*

RUN git clone https://github.com/emscripten-core/emsdk.git /emsdk && \
    cd /emsdk && \
    ./emsdk install latest && \
    ./emsdk activate latest

ENV EMSDK=/emsdk
ENV EMSDK_NODE=/usr/bin/node
ENV PATH="/emsdk:/emsdk/node/22.16.0_64bit/bin:/emsdk/upstream/bin:/emsdk/upstream/emscripten:$PATH"
