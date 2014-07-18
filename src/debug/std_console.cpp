//--------------------------------------------------------------------------------
// std_console.cpp
// 標準コンソール【関数／実体定義部】
//
// Gakimaru's researched and standard library for C++ - GASHA
//   Copyright (c) 2014 Itagaki Mamoru
//   Released under the MIT license.
//     https://github.com/gakimaru/gasha/blob/master/LICENSE
//--------------------------------------------------------------------------------

#include <gasha/std_console.inl>//標準コンソール【インライン関数／テンプレート関数定義部】

GASHA_NAMESPACE_BEGIN;//ネームスペース：開始

//--------------------------------------------------------------------------------
//標準コンソール
//--------------------------------------------------------------------------------

#ifdef GASHA_LOG_IS_ENABLED//デバッグログ無効時はまるごと無効化

//----------------------------------------
//画面通知用標準コンソール

#ifdef GASHA_USE_WINDOWS_CONSOLE

//出力終了
void stdConsoleOfNotice::end()
{
	putCr();
	winConsole::end();
}

#else//GASHA_USE_WINDOWS_CONSOLE

//出力終了
void stdConsoleOfNotice::end()
{
	putCr();
	ttyConsole::end();
}

#endif//GASHA_USE_WINDOWS_CONSOLE

#endif//GASHA_LOG_IS_ENABLED//デバッグログ無効時はまるごと無効化

//----------------------------------------
//標準出力コンソール
stdOutConsole stdOutConsole::s_instance;

//----------------------------------------
//標準エラーコンソール
stdErrConsole stdErrConsole::s_instance;

//----------------------------------------
//画面通知用標準コンソール
stdConsoleOfNotice stdConsoleOfNotice::s_instance;

//----------------------------------------
//標準メモリコンソール
stdMemConsole stdMemConsole::s_instance;

//----------------------------------------
//標準ダミーコンソール
stdDummyConsole stdDummyConsole::s_instance;

GASHA_NAMESPACE_END;//ネームスペース：終了

#ifdef GASHA_LOG_IS_ENABLED//デバッグログ無効時はまるごと無効化

//----------------------------------------
//標準メモリコンソールの明示的なインスタンス化
#include <gasha/mem_console.cpp.h>//メモリコンソール【関数／実体定義部】

//明示的なインスタンス化
GASHA_INSTANCING_memConsole(GASHA_STD_MEM_CONSOLE_BUFF_SIZE);

#endif//GASHA_LOG_IS_ENABLED//デバッグログ無効時はまるごと無効化

// End of file
