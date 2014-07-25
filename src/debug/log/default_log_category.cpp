//--------------------------------------------------------------------------------
// default_log_category.cpp
// 既定のログカテゴリ【関数／実体定義部】
//
// Gakimaru's standard library for C++ - GASHA
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

#ifdef GASHA_LOG_IS_ENABLED//デバッグログ無効時はまるごと無効化

//既定のログカテゴリを登録
void registDefaultLogCategory()
{
	//通常ログカテゴリ登録補助マクロ
	#define GASHA_REGIST_LOG_CATEGORY(VALUE, CONSOLE, CONSOLE_N) \
		{ \
			GASHA_ iConsole* console = CONSOLE; \
			GASHA_ iConsole* console_n = CONSOLE_N; \
			GASHA_ iConsole* consoles[logCategory::PURPOSE_NUM] = { console, console_n }; \
			_private::regLogCategory<VALUE>()(\
				#VALUE, \
				consoles \
			); \
		}

	//特殊ログカテゴリ登録補助マクロ
	#define GASHA_REGIST_SPECIAL_LOG_CATEGORY(VALUE) \
		{ \
			_private::regSpecialLogCategory<VALUE>()( \
				#VALUE \
			); \
		}

	//コンソール
	iConsole& dummy_console = GASHA_ stdDummyConsole::instance();//標準ダミーコンソール
	iConsole& notice_console = GASHA_ stdNoticeConsole::instance();//画面通知用標準コンソール

	//通常ログカテゴリ登録
	GASHA_REGIST_LOG_CATEGORY(forAny, nullptr, nullptr);//なんでも（カテゴリなし）
	GASHA_REGIST_LOG_CATEGORY(forDummy, &dummy_console, &dummy_console);//ダミー
	GASHA_REGIST_LOG_CATEGORY(forNotice, &dummy_console, &notice_console);//画面通知専用
	GASHA_REGIST_LOG_CATEGORY(forFileSystem, nullptr, nullptr);//ファイルシステム関係
	GASHA_REGIST_LOG_CATEGORY(forResource, nullptr, nullptr);//リソース関係
	GASHA_REGIST_LOG_CATEGORY(for3D, nullptr, nullptr);//3Dグラフィックス関係
	GASHA_REGIST_LOG_CATEGORY(for2D, nullptr, nullptr);//2Dグラフィックス関係
	GASHA_REGIST_LOG_CATEGORY(forSound, nullptr, nullptr);//サウンド関係
	
	//特殊ログカテゴリ登録
	GASHA_REGIST_SPECIAL_LOG_CATEGORY(forEvery);//全部まとめて変更
	GASHA_REGIST_SPECIAL_LOG_CATEGORY(forCallPoint);//直近のコールポイントのカテゴリに合わせる（なければforAny扱い）
	GASHA_REGIST_SPECIAL_LOG_CATEGORY(forCriticalCallPoint);//直近の重大コールポイントのカテゴリに合わせる（なければforAny扱い）
}

#endif//GASHA_LOG_IS_ENABLED//デバッグログ無効時はまるごと無効化

GASHA_NAMESPACE_END;//ネームスペース：終了

// End of file
