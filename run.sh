# ******************************************************
# Author       : zhxx
# Last modified: 2017-09-04 10:51
# Filename     : build.sh
# Description  : http://myzhxx.cn
# ******************************************************
#!/bin/bash

set -x

SOURCE_DIR=`pwd`
BUILD_DIR=${BUILD_DIR:-./build}

mkdir -p $BUILD_DIR\
	&& cd $BUILD_DIR\
	&& cmake $SOURCE_DIR\
	&& make $*

