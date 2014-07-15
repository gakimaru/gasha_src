//--------------------------------------------------------------------------------
// default_log_category.cpp
// 既定のログカテゴリ【関数／実体定義部】
//
// Gakimaru's researched and standard library for C++ - GASHA
//   Copyright (c) 2014 Itagaki Mamoru
//   Released under the MIT license.
//     https://github.com/gakimaru/gasha/blob/master/LICENSE
//--------------------------------------------------------------------------------

#include <gasha/default_log_category.inl>//既定のログカテゴリ【インライン関数／テンプレート関数定義部】

#include <gasha/std_console.h>//標準コンソール

GASHA_NAMESPACE_BEGIN;//ネームスペース：開始

//--------------------------------------------------------------------------------
//既定のログカテゴリ
//--------------------------------------------------------------------------------

#ifdef GASHA_HAS_DEBUG_LOG//デバッグログ無効時はまるごと無効化

//既定のログカテゴリを登録
void registDefaultLogCategory()
{
	//通常ログカテゴリ登録補助マクロ
	#define REG_LOG_CATEGORY(VALUE, CONSOLE, CONSOLE_N) \
		{ \
			GASHA_ IConsole* console = CONSOLE; \
			GASHA_ IConsole* console_n = CONSOLE_N; \
			GASHA_ IConsole* consoles[logCategory::PURPOSE_NUM] = { console, console_n }; \
			_private::regLogCategory<VALUE>()(\
				#VALUE, \
				consoles \
			); \
		}

	//特殊ログカテゴリ登録補助マクロ
	#define REG_SPECIAL_LOG_CATEGORY(VALUE) \
		{ \
			_private::regSpecialLogCategory<VALUE>()( \
				#VALUE \
			); \
		}

	//コンソール
	IConsole& dummy_console = GASHA_ stdDummyConsole::instance();//標準ダミーコンソール
	IConsole& notice_console = GASHA_ stdConsoleOfNotice::instance();//画面通知用標準コンソール

	//通常ログカテゴリ登録
	REG_LOG_CATEGORY(forAny, nullptr, nullptr);//なんでも（カテゴリなし）
	REG_LOG_CATEGORY(forNotice, &dummy_console, &notice_console);//画面通知専用
	REG_LOG_CATEGORY(forFileSystem, nullptr, nullptr);//ファイルシステム関係
	REG_LOG_CATEGORY(forResource, nullptr, nullptr);//リソース関係
	REG_LOG_CATEGORY(for3D, nullptr, nullptr);//3Dグラフィックス関係
	REG_LOG_CATEGORY(for2D, nullptr, nullptr);//2Dグラフィックス関係
	REG_LOG_CATEGORY(forSound, nullptr, nullptr);//サウンド関係
	
	//特殊ログカテゴリ登録
	REG_SPECIAL_LOG_CATEGORY(forEvery);//全部まとめて変更
	REG_SPECIAL_LOG_CATEGORY(forCallPoint);//直近のコールポイントのカテゴリに合わせる（なければforAny扱い）
	REG_SPECIAL_LOG_CATEGORY(forCriticalCallPoint);//直近の重大コールポイントのカテゴリに合わせる（なければforAny扱い）
}

#endif//GASHA_HAS_DEBUG_LOG//デバッグログ無効時はまるごと無効化

GASHA_NAMESPACE_END;//ネームスペース：終了

// End of file
