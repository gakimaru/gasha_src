//--------------------------------------------------------------------------------
// std_debug_pause.cpp
// 標準デバッグポーズ【関数／実体定義部】
//
// Gakimaru's researched and standard library for C++ - GASHA
//   Copyright (c) 2014 Itagaki Mamoru
//   Released under the MIT license.
//     https://github.com/gakimaru/gasha/blob/master/LICENSE
//--------------------------------------------------------------------------------

#include <gasha/std_debug_pause.inl>//標準デバッグポーズ【インライン関数／テンプレート関数定義部】

#include <gasha/simple_assert.inl>//シンプルアサーション

GASHA_NAMESPACE_BEGIN;//ネームスペース：開始

//--------------------------------------------------------------------------------
//標準デバッグポーズ
//--------------------------------------------------------------------------------

#ifdef GASHA_ASSERTION_IS_ENABLED//アサーション無効時はまるごと無効化

//----------------------------------------
//標準デバッグポーズ

//デバッガのブレークポイント発動
void stdDebugPause::breakPoint()
{
	debuggerBreak();
}

//ポーズ処理呼び出し
void stdDebugPause::pause()
{
	//何もしない
}

#endif//GASHA_ASSERTION_IS_ENABLED//アサーション無効時はまるごと無効化

//----------------------------------------
//標準デバッグポーズ
stdDebugPause stdDebugPause::m_instance;

GASHA_NAMESPACE_END;//ネームスペース：終了

// End of file
