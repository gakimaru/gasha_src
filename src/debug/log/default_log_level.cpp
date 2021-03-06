﻿//--------------------------------------------------------------------------------
// default_log_level.cpp
// 既定のログレベル【関数／実体定義部】
//
// Gakimaru's standard library for C++ - GASHA
//   Copyright (c) 2014 Itagaki Mamoru
//   Released under the MIT license.
//     https://github.com/gakimaru/gasha/blob/master/LICENSE
//--------------------------------------------------------------------------------

#include <gasha/default_log_level.inl>//既定のログレベル【インライン関数／テンプレート関数定義部】

#include <gasha/std_console.h>//標準コンソール

GASHA_NAMESPACE_BEGIN;//ネームスペース：開始

//--------------------------------------------------------------------------------
//既定のログレベル
//--------------------------------------------------------------------------------

#ifdef GASHA_LOG_IS_ENABLED//デバッグログ無効時はまるごと無効化

//既定のログレベルを登録
void registDefaultLogLevel()
{
	//通常ログレベル登録補助マクロ
	#define GASHA_REGIST_LOG_LEVEL(VALUE, CONSOLE, CONSOLE_N, FORE, BACK, FORE_N, BACK_N) \
		{ \
			GASHA_ iConsole* console = CONSOLE; \
			GASHA_ iConsole* console_n = CONSOLE_N; \
			GASHA_ consoleColor color(GASHA_ consoleColor::FORE, GASHA_ consoleColor::BACK); \
			GASHA_ consoleColor color_n(GASHA_ consoleColor::FORE_N, GASHA_ consoleColor::BACK_N); \
			GASHA_ iConsole* consoles[logLevel::PURPOSE_NUM] = { console, console_n }; \
			GASHA_ consoleColor* colors[logLevel::PURPOSE_NUM] = { &color, &color_n }; \
			_private::regLogLevel<VALUE>()(\
				#VALUE, \
				consoles, \
				colors \
			); \
		}

	//特殊ログレベル登録補助マクロ
	#define GASHA_REGIST_SPECIAL_LOG_LEVEL(VALUE) \
		{ \
			_private::regSpecialLogLevel<VALUE>()( \
				#VALUE \
			); \
		}

	//コンソール
	iConsole& stdout_console = GASHA_ stdOutConsole::instance();//標準出力コンソール
	iConsole& stderr_console = GASHA_ stdErrConsole::instance();//標準エラーコンソール
	iConsole& notice_console = GASHA_ stdNoticeConsole::instance();//画面通知用標準コンソール

	//通常ログレベル登録
	GASHA_REGIST_LOG_LEVEL(asNormal, &stdout_console, nullptr, STANDARD, STANDARD, BLACK, iWHITE);//通常メッセージ
	GASHA_REGIST_LOG_LEVEL(asVerbose, &stdout_console, nullptr, iBLACK, STANDARD, iBLACK, iWHITE);//冗長メッセージ
	GASHA_REGIST_LOG_LEVEL(asDetail, &stdout_console, nullptr, iBLACK, STANDARD, iBLACK, iWHITE);//詳細メッセージ
	GASHA_REGIST_LOG_LEVEL(asImportant, &stdout_console, nullptr, iBLUE, BLACK, iBLUE, iWHITE);//重要メッセージ
	GASHA_REGIST_LOG_LEVEL(asWarning, &stderr_console, &notice_console, iMAGENTA, STANDARD, BLACK, iMAGENTA);//警告メッセージ
	GASHA_REGIST_LOG_LEVEL(asCritical, &stderr_console, &notice_console, iRED, STANDARD, iYELLOW, iRED);//重大メッセージ
	GASHA_REGIST_LOG_LEVEL(asAbsolute, &stdout_console, nullptr, STANDARD, STANDARD, STANDARD, STANDARD);//絶対メッセージ（ログレベルに関係なく出力したいメッセージ）

	//特殊ログレベル登録
	GASHA_REGIST_SPECIAL_LOG_LEVEL(asSilent);//静寂（絶対メッセージ以外出力しない）
	GASHA_REGIST_SPECIAL_LOG_LEVEL(asSilentAbsolutely);//絶対静寂（全てのメッセージを出力しない）
}

#else//GASHA_LOG_IS_ENABLED

//【VC++】LNK4221回避用のダミー関数
namespace _private{
	void default_log_level_dummy(){}
}//namespace _private

#endif//GASHA_LOG_IS_ENABLED//デバッグログ無効時はまるごと無効化

GASHA_NAMESPACE_END;//ネームスペース：終了

// End of file
