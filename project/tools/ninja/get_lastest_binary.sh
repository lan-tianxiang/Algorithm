#!/bin/bash
# This script downloads the latest GN binary for Linux AMD64 architecture.

wget https://github.com/ninja-build/ninja/releases/download/v1.13.1/ninja-linux.zip -O download.zip
unzip download.zip
rm download.zip
chmod +x ninja