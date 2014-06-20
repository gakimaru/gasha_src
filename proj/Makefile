#-------------------------------------------------------------------------------
# Makefile
# ライブラリ用メイクファイル
#
# Gakimaru's researched and standard library for C++ - GASHA
#   Copyright (c) 2014 Itagaki Mamoru
#   Released under the MIT license
#     https://github.com/gakimaru/gasha/blob/master/LICENSE
#-------------------------------------------------------------------------------

#-------------------------------------------------------------------------------

#-------------------------------------------------------------------------------

# アーカイブファイル用ディレクトリ
ARC_DIR := ../../gasha/lib/gcc

# ソースファイル用基本ディレクトリ
SRC_BASE_DIR := ../src

# ヘッダーファイル用基本ディレクトリ
HEADER_BASE_DIR := ./

# アーカイブファイル
ARC := $(ARC_DIR)/gasha_x86.a

# ソースファイル
SRCS := \
	$(SRC_BASE_DIR)/crc/crc32.cpp \

# オブジェクトファイル
OBJS := $(SRCS:%.cpp=%.o)

# 標準ヘッダーファイル（強制インクルードファイル）
STD_HEADER := $(HEADER_BASE_DIR)/standard.h

# プリコンパイル済み標準ヘッダーファイル
PC_HEADER := $(STD_HEADER:%.h=%.h.gch)

# ソース／ヘッダーファイルの依存ファイル
DEPS := $(SRCS:%.cpp=%.d) $(PC_HEADER:%.gch=%.d)

#-------------------------------------------------------------------------------

# アーカイブコマンド
AR := ar

# アーカイブオプション
ARFLAGS = rcsv

# C++コマンド
CXX := g++ -std=c++11

CPPFLAGS += \
# C++プリプロセッサオプション
CPPFLAGS += \
	-I. \
	-I../src/crc \
	-I../../gasha/include \
	-I../../gasha_settings/include \

# C++コンパイルオプション
CXXFLAGS += \
	-g \
	-Wall \
	-Wextra \
	-O3 \
	-msse4.2 \
	-fopenmp \

# C++コンパイルオプション（C++ファイル用）
CXXFLAGS_CPP := \
	-include $(STD_HEADER) \

# C++コンパイルオプション（プリコンパイル済みヘッダー用）
CXXFLAGS_GCH := \
	-x c++-header

#-------------------------------------------------------------------------------

# 型ルール：オブジェクトファイル生成
# ※依存関係定義ファイルも自動生成
%.o : %.cpp
	$(CXX) $(CFLAGS) $(CPPFLAGS) $(CXXFLAGS_COMMON) $(CXXFLAGS) $(CXXFLAGS_CPP) -MMD -c $< -o $@

# 型ルール：プリコンパイル済みヘッダーファイル生成
# ※依存関係定義ファイルも自動生成
%.h.gch : %.h
	$(CXX) $(CFLAGS) $(CPPFLAGS) $(CXXFLAGS_COMMON) $(CXXFLAGS) $(CXXFLAGS_GCH) -MMD -c $< -o $@

#-------------------------------------------------------------------------------

# ターゲット：アーカイブファイル
$(ARC) : $(OBJS)
	mkdir -p $(dir $@); \
	$(AR) $(ARFLAGS) $@ $<

# ターゲット：オブジェクトファイル
# ※オブジェクトファイルの依存関係定義ファイル（自動生成）をインクルード
-include $(DEPS)

# ターゲット：プリコンパイル済みヘッダーファイル（の依存ファイル定義ファイル）
$(PC_HEADER:%.gch=%.d) : $(PC_HEADER)

#-------------------------------------------------------------------------------

# ターゲット：all
all : $(ARC)

# ターゲット：clean
clean :
	rm -vf $(ARC) $(OBJS) $(DEPS) $(PC_HEADER)

# ターゲット：distclean
distclean :
	rm -vf $(ARC) $(OBJS) $(DEPS) $(PC_HEADER)

# 擬似ターゲット設定
.PHONY: all clean

#-------------------------------------------------------------------------------

# End of file
