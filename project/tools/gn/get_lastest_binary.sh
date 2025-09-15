#!/bin/bash
# This script downloads the latest GN binary for Linux AMD64 architecture.

wget https://chrome-infra-packages.appspot.com/dl/gn/gn/linux-amd64/+/latest -O download.zip
unzip download.zip
rm download.zip
chmod +x gn