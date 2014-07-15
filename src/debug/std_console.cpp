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

#ifdef GASHA_HAS_DEBUG_LOG//デバッグログ無効時はまるごと無効化

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
//標準ダミーコンソール
stdDummyConsole stdDummyConsole::s_instance;

#endif//GASHA_HAS_DEBUG_LOG//デバッグログ無効時はまるごと無効化

GASHA_NAMESPACE_END;//ネームスペース：終了

// End of file
