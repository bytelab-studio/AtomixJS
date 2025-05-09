#!/bin/bash

SCRIPT_DIR="$(cd "$(dirname "${BASH_SOURCE[0]}")" && pwd)"
TARGET_PATH="$(realpath "$SCRIPT_DIR/../atomixc/dist/bin/atomixc.js")"

alias atomixc="node \"$TARGET_PATH\""
export PS1="(atomixc) $PS1"