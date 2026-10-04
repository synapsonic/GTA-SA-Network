#!/usr/bin/env bash
set -euo pipefail
cmake -S . -B build-server -DGTANET_BUILD_CLIENT=OFF -DGTANET_BUILD_LAUNCHER=OFF -DGTANET_BUILD_SERVER=ON -DGTANET_BUILD_TESTS=ON -DGTANET_WITH_SQLITE=OFF -DGTANET_WITH_LUA=OFF
cmake --build build-server -j"$(nproc)"
ctest --test-dir build-server --output-on-failure
