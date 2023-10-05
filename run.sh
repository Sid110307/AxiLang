#!/usr/bin/env bash

if [ -z "$(which cmake)" ]; then
    echo "\033[31m[\033[1;31mERROR\033[31m]: It doesn't look like you have cmake installed. Please install it and try again.\033[0m"
    exit 1
fi

cmake -S . -B bin && cmake --build bin --target all -j4 && ./bin/axilang "$@"

