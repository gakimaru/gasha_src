#!/bin/sh

#-------------------------------------------------------------------------------
# mk.sh
# 全プラットフォーム＆ビルド設定でメイクを実行
#
# Gakimaru's researched and standard library for C++ - GASHA
#   Copyright (c) 2014 Itagaki Mamoru
#   Released under the MIT license
#     https://github.com/gakimaru/gasha_proj/blob/master/LICENSE
#-------------------------------------------------------------------------------

# x86 & Relase
export BUILD_PLATFORM=x86
export BUILD_TYPE=Release
make $1

# x86 & Debug
export BUILD_PLATFORM=x86
export BUILD_TYPE=Debug
make $1

# x64 & Relase
export BUILD_PLATFORM=x64
export BUILD_TYPE=Release
#make $1

# x64 & Debug
export BUILD_PLATFORM=x64
export BUILD_TYPE=Debug
#make $1

# End of file
