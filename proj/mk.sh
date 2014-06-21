#!/bin/sh

#-------------------------------------------------------------------------------
# mk.sh
# 全プラットフォーム＆ビルド設定でライブラリをメイク
#
# Gakimaru's researched and standard library for C++ - GASHA
#   Copyright (c) 2014 Itagaki Mamoru
#   Released under the MIT license
#     https://github.com/gakimaru/gasha_src/blob/master/LICENSE
#-------------------------------------------------------------------------------

# メイクファイル実行関数
function run_makefile_core() {
	echo '--------------------------------------------------------------------------------'
	pushd $1
	make $2
	popd
}

# メイクファイル呼び出し
function run_makefile() {
	echo ''
	echo '================================================================================'
	echo $1, $2, $3
	export BUILD_PLATFORM=$1
	export BUILD_TYPE=$2
	run_makefile_core . $3
}

# x86 & Relase
run_makefile x86 Release $1

# x86 & Debug
run_makefile x86 Debug $1

# x64 & Relase
#run_makefile x64 Release $1

# x64 & Debug
#run_makefile x64 Debug $1

# End of file
