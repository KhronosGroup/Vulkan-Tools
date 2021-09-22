#!/bin/bash
# Update source for vulkan-headers

# Copyright 2016 The Android Open Source Project
# Copyright (C) 2015 Valve Corporation
#
# Licensed under the Apache License, Version 2.0 (the "License");
# you may not use this file except in compliance with the License.
# You may obtain a copy of the License at
#
#      http://www.apache.org/licenses/LICENSE-2.0
#
# Unless required by applicable law or agreed to in writing, software
# distributed under the License is distributed on an "AS IS" BASIS,
# WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
# See the License for the specific language governing permissions and
# limitations under the License.

set -e

sync_vulkan_headers=1

ANDROIDBUILDDIR=$PWD
BUILDDIR=$ANDROIDBUILDDIR
BASEDIR=$BUILDDIR/third_party

VULKAN_HEADERS_REVISION=$(cat $ANDROIDBUILDDIR/vulkan-headers_revision_android)

echo "VULKAN_HEADERS_REVISION=$VULKAN_HEADERS_REVISION"

VULKAN_HEADERS_URL=$(cat $ANDROIDBUILDDIR/vulkan-headers_url_android)

if [[ $(uname) == "Linux" ]]; then
    cores="$(nproc || echo 4)"
elif [[ $(uname) == "Darwin" ]]; then
    cores=$(sysctl -n hw.ncpu)
fi

#
# Parse parameters
#

function printUsage {
   echo "Supported parameters are:"
   echo "    --abi <abi> (optional)"
   echo "    --no-build (optional)"
   echo
   echo "i.e. ${0##*/} --abi arm64-v8a \\"
   exit 1
}

while [[ $# -gt 0 ]]
do
    case $1 in
        --abi)
            abi="$2"
            shift 2
            ;;
        --no-build)
            nobuild=1
            shift 1
            ;;
        *)
            # unknown option
            echo Unknown option: $1
            echo
            printUsage
            exit 1
            ;;
    esac
done

echo abi=$abi
if [[ -z $abi ]]
then
    echo No abi provided, so building for all supported abis.
fi

echo no-build=$nobuild
if [[ $nobuild ]]
then
    echo Skipping build.
fi

function create_vulkan-headers () {
   rm -rf $BASEDIR/Vulkan-Headers
   echo "Creating local Vulkan-Headers repository ($BASEDIR/Vulkan-Headers)."
   mkdir -p $BASEDIR/Vulkan-Headers
   cd $BASEDIR/Vulkan-Headers
   git clone $VULKAN_HEADERS_URL .
   git checkout $VULKAN_HEADERS_REVISION
}

function update_vulkan-headers () {
   echo "Updating $BASEDIR/Vulkan-Headers"
   cd $BASEDIR/Vulkan-Headers
   if [[ $(git config --get remote.origin.url) != $VULKAN_HEADERS_URL ]]; then
      echo "Vulkan-Headers URL mismatch, recreating local repo"
      create_vulkan-headers
      return
   fi
   git fetch --all
   git checkout $VULKAN_HEADERS_REVISION
}

if [ $sync_vulkan_headers -eq 1 ]; then
    if [ ! -d "$BASEDIR/Vulkan-Headers" -o ! -d "$BASEDIR/Vulkan-Headers/.git" ]; then
    create_vulkan-headers
    fi
    update_vulkan-headers
fi

echo ""
echo "${0##*/} finished."

