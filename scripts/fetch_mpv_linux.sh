#!/usr/bin/env bash
set -euo pipefail

# Install mpv dev/runtime via package manager (Ubuntu/Debian example)
if command -v apt-get >/dev/null 2>&1; then
  sudo apt-get update
  sudo apt-get install -y libmpv-dev libmpv1
else
  echo "Please install libmpv-dev and libmpv runtime via your distro package manager." >&2
  exit 1
fi

