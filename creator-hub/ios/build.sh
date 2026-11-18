#!/bin/bash
# Builds the iOS app on Linux (needs clang + lld 18). Output: build/CreatorHub.ipa (unsigned; sign with Sideloadly).
set -eo pipefail
cd "$(dirname "$0")"
clang -target arm64-apple-ios15.0 -O2 -Wall -Wextra -fno-stack-protector -c src/main.c -o main.o
ld64.lld-18 -arch arm64 -platform_version ios 15.0 17.0 -o CreatorHub main.o stubs/libSystem.tbd stubs/libobjc.tbd stubs/Foundation.tbd stubs/UIKit.tbd stubs/WebKit.tbd -headerpad_max_install_names -dead_strip
python3 package.py
