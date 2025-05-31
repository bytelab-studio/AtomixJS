#!/bin/bash

SCRIPT_DIR="$(cd "$(dirname "${BASH_SOURCE[0]}")" && pwd)"
TARGET_PATH="$(realpath "$SCRIPT_DIR/../atomixc/dist/bin/atomixc.js")"
ZIG_DIR="$SCRIPT_DIR/../zig"

if [ -d $ZIG_DIR ]; then
    export PATH="$PATH:$(realpath "$ZIG_DIR")"
fi

alias atomixc="node \"$TARGET_PATH\""
export PS1="(atomixc) $PS1"