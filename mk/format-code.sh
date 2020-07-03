#!/bin/bash
set -e
set -o pipefail
cd "$( dirname "${BASH_SOURCE[0]}" )"/..

find -name '*.[hc]pp' -or -name '*.ino' | \
  xargs clang-format-8 -i -style=file
