#-------------------------------------------------------------------------------
# Makefile
# ライブラリ用メイクファイル
#
# Gakimaru's researched and standard library for C++ - GASHA
#   Copyright (c) 2014 Itagaki Mamoru
#   Released under the MIT license.
#     https://github.com/gakimaru/gasha_src/blob/master/LICENSE
#-------------------------------------------------------------------------------

#-------------------------------------------------------------------------------

# ビルドプラットフォーム
ifndef BUILD_PLATFORM
	BUILD_PLATFORM := x86
	#BUILD_PLATFORM := x64
endif

# ビルド設定
ifndef BUILD_TYPE
	BUILD_TYPE := Release
	#BUILD_TYPE := Debug
endif

#-------------------------------------------------------------------------------

# ライブラリの基本ディレクトリ
LIB_SRC_BASE := ../..

# ソースファイル用基本ディレクトリ
SRC_BASE_DIR := ../src

# ヘッダーファイル用基本ディレクトリ
HEADER_BASE_DIR := .

# 出力ディレクトリ
OUT_BASE_DIR := _gcc/$(BUILD_PLATFORM)/$(BUILD_TYPE)
OUT_DIR := $(OUT_BASE_DIR)/sub/gasha_src

#-------------------------------------------------------------------------------

# ソースファイル
SRCS := \
	$(SRC_BASE_DIR)/build_settings/build_settings_diag.cpp \
	$(SRC_BASE_DIR)/utility/cpuid.cpp \
	$(SRC_BASE_DIR)/utility/type_traits.cpp \
	$(SRC_BASE_DIR)/utility/chrono.cpp \
	$(SRC_BASE_DIR)/math/basic_math.cpp \
	$(SRC_BASE_DIR)/math/crc32.cpp \
	$(SRC_BASE_DIR)/string/fast_string.cpp \
	$(SRC_BASE_DIR)/thread/thread_id.cpp \
	$(SRC_BASE_DIR)/thread/lock/lock_common.cpp \
	$(SRC_BASE_DIR)/thread/lock/spin_lock.cpp \
	$(SRC_BASE_DIR)/thread/lock/lw_spin_lock.cpp \
	$(SRC_BASE_DIR)/thread/lock/shared_spin_lock.cpp \
	$(SRC_BASE_DIR)/thread/lock/simple_shared_spin_lock.cpp \
	$(SRC_BASE_DIR)/thread/lock/unshared_spin_lock.cpp \

# オブジェクトファイル
OBJS := $(SRCS:%.cpp=$(OUT_DIR)/%.o)

# 標準ヘッダーファイル（強制インクルードファイル）
STD_HEADER_NAME := standard.h
STD_HEADER := $(HEADER_BASE_DIR)/$(STD_HEADER_NAME)

# プリコンパイル済み標準ヘッダーファイル
PC_STD_HEADER := $(OUT_DIR)/$(STD_HEADER:%.h=%.h.gch)

# ソース／ヘッダーファイルの依存ファイル
DEPS := $(SRCS:%.cpp=$(OUT_DIR)/%.d) $(PC_STD_HEADER:$(OUT_DIR)/%.gch=%.d)

#-------------------------------------------------------------------------------
# 共通部分をインクルード

include ../../gasha/proj/Makefile_common

#-------------------------------------------------------------------------------

# ターゲット：アーカイブファイル
$(ARC) : $(OBJS)
	mkdir -p $(dir $@); \
	$(AR) $(ARFLAGS) $@ $^

# ターゲット：オブジェクトファイルとプリコンパイル済みヘッダーファイル
#$(OUT_DIR)/../src/crc/crc32.o: ../src/crc/crc32.cpp $(PC_STD_HEADER)

# ターゲット：オブジェクトファイル
# ※オブジェクトファイルの依存関係定義ファイル（自動生成）をインクルード
-include $(DEPS)

#-------------------------------------------------------------------------------

# ターゲット：all
all : $(ARC)

# ターゲット：clean
clean :
	rm -vf $(ARC) $(PC_STD_HEADER) $(OBJS) $(DEPS)

# ターゲット：distclean
distclean :
	rm -rf $(OUT_BASE_DIR)

# 擬似ターゲット設定
.PHONY: all clean distclean

#-------------------------------------------------------------------------------

# End of file
