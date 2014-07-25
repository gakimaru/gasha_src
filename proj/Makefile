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
	$(SRC_BASE_DIR)/string/string.cpp \
	$(SRC_BASE_DIR)/string/fast_string.cpp \
	$(SRC_BASE_DIR)/string/strconv.cpp \
	$(SRC_BASE_DIR)/thread/thread_id.cpp \
	$(SRC_BASE_DIR)/thread/lock/lock_common.cpp \
	$(SRC_BASE_DIR)/thread/lock/spin_lock.cpp \
	$(SRC_BASE_DIR)/thread/lock/lw_spin_lock.cpp \
	$(SRC_BASE_DIR)/thread/lock/shared_spin_lock.cpp \
	$(SRC_BASE_DIR)/thread/lock/simple_shared_spin_lock.cpp \
	$(SRC_BASE_DIR)/thread/lock/unshared_spin_lock.cpp \
	$(SRC_BASE_DIR)/memory/lf_mono_allocator.cpp \
	$(SRC_BASE_DIR)/memory/poly_allocator.cpp \
	$(SRC_BASE_DIR)/memory/new.cpp \
	$(SRC_BASE_DIR)/debug/console/console_color.cpp \
	$(SRC_BASE_DIR)/debug/console/tty_console.cpp \
	$(SRC_BASE_DIR)/debug/console/win_console.cpp \
	$(SRC_BASE_DIR)/debug/console/vs_console.cpp \
	$(SRC_BASE_DIR)/debug/console/dummy_console.cpp \
	$(SRC_BASE_DIR)/debug/console/std_console.cpp \
	$(SRC_BASE_DIR)/debug/log/log_level.cpp \
	$(SRC_BASE_DIR)/debug/log/default_log_level.cpp \
	$(SRC_BASE_DIR)/debug/log/log_category.cpp \
	$(SRC_BASE_DIR)/debug/log/default_log_category.cpp \
	$(SRC_BASE_DIR)/debug/log/log_mask.cpp \
	$(SRC_BASE_DIR)/debug/log/log_attr.cpp \
	$(SRC_BASE_DIR)/debug/log/log_work_buff.cpp \
	$(SRC_BASE_DIR)/debug/log/log_queue.cpp \
	$(SRC_BASE_DIR)/debug/log/log_queue_monitor.cpp \
	$(SRC_BASE_DIR)/debug/log/debug_log.cpp \
	$(SRC_BASE_DIR)/debug/call_point/call_point.cpp \
	$(SRC_BASE_DIR)/debug/assert/assert.cpp \
	$(SRC_BASE_DIR)/debug/assert/stdin_debug_pause.cpp \
	$(SRC_BASE_DIR)/debug/assert/dummy_debug_pause.cpp \
	$(SRC_BASE_DIR)/debug/assert/std_debug_pause.cpp \
	$(SRC_BASE_DIR)/debug/profiler/profiler.cpp \
	$(SRC_BASE_DIR)/serialization/type_ctrl.cpp \
	$(SRC_BASE_DIR)/serialization/type_ctrl_base.cpp \
	$(SRC_BASE_DIR)/serialization/item_info_base.cpp \
	$(SRC_BASE_DIR)/archive/results.cpp \
	$(SRC_BASE_DIR)/archive/archive_base.cpp \
	$(SRC_BASE_DIR)/archive/archive_adapter_base.cpp \
	$(SRC_BASE_DIR)/archive/output_archive_adapter.cpp \
	$(SRC_BASE_DIR)/archive/input_archive_adapter.cpp \
	$(SRC_BASE_DIR)/archive/binary_archive_format_base.cpp \
	$(SRC_BASE_DIR)/archive/output_binary_archive_format.cpp \
	$(SRC_BASE_DIR)/archive/input_binary_archive_format.cpp \
	$(SRC_BASE_DIR)/archive/output_text_archive_format.cpp \
	$(SRC_BASE_DIR)/archive/input_text_archive_format.cpp \

# オブジェクトファイル
OBJS := $(SRCS:%.cpp=$(OUT_DIR)/%.o)

# 標準ヘッダーファイル（強制インクルードファイル）
STD_HEADER_NAME := standard.h
STD_HEADER := $(HEADER_BASE_DIR)/$(STD_HEADER_NAME)

# プリコンパイル済み標準ヘッダーファイル
PC_STD_HEADER := $(OUT_DIR)/$(STD_HEADER:%.h=%.h.gch)

# ソース／ヘッダーファイルの依存ファイル
DEPS := $(SRCS:%.cpp=$(OUT_DIR)/%.d) $(PC_STD_HEADER:$(OUT_DIR)/%.gch=%.d)

# ライブラリビルド時専用のプリプロセッサ
CXXFLAGS += -DGASHA_BUILD_FOR_LIB

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
