#!/bin/bash
BASEDIR=`dirname $0`

if [ -d /System/Applications/Utilities/Terminal.app ]
then
    open /System/Applications/Utilities/Terminal.app $BASEDIR/vulkaninfo
else
    open /Applications/Utilities/Terminal.app $BASEDIR/vulkaninfo
fi

