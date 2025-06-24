#!/usr/bin/env bash

set -e

cd $(dirname "$0")

echo "Publishing CGE..."

# 总共有两个参数, 交叉起来四个版本
# 1. Has video module or not
# 2. 16kb page size or not

bash -e ./tasks.sh --setup-project --enable-cmake --release

# Step1 no video, no 16kb

bash -e ./tasks.sh --disable-video-module --disable-16kb-page-size --no-install --build --publish

# Step2 no video, 16kb

bash -e ./tasks.sh --disable-video-module --enable-16kb-page-size --no-install --build --publish

# Step3 video, no 16kb

bash -e ./tasks.sh --enable-video-module --disable-16kb-page-size --no-install --build --publish

# Step4 video, 16kb

bash -e ./tasks.sh --enable-video-module --enable-16kb-page-size --no-install --build --publish

# Done
echo "Publishing CGE done. See: $(cat local.properties | grep 'localRepoDir')"
