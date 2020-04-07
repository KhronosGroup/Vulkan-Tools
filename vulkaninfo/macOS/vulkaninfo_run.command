#!/bin/bash
BASEDIR=`dirname $0`
export DYLD_LIBRARY_PATH=$DYLD_LIBRARY_PATH:$BASEDIR
$BASEDIR/vulkaninfo
