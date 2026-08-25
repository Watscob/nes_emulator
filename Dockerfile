FROM ubuntu:26.04

SHELL [ "bash", "-c" ]

RUN apt-get update &&   \
    apt-get install -y  \
    build-essential     \
    clang-format        \
    cmake               \
    git &&              \
    rm -rf /var/lib/apt/lists/*
