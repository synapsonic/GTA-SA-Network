$ErrorActionPreference = "Stop"
cmake -S . -B build-client -A Win32 -DGTANET_BUILD_SERVER=OFF -DGTANET_BUILD_CLIENT=ON
cmake --build build-client --config Release
