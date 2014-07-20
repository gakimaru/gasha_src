//--------------------------------------------------------------------------------
// dummy_debug_pause.cpp
// ダミーデバッグポーズ【関数／実体定義部】
//
// Gakimaru's researched and standard library for C++ - GASHA
//   Copyright (c) 2014 Itagaki Mamoru
//   Released under the MIT license.
//     https://github.com/gakimaru/gasha/blob/master/LICENSE
//--------------------------------------------------------------------------------

#include <gasha/dummy_debug_pause.inl>//ダミーデバッグポーズ【インライン関数／テンプレート関数定義部】

GASHA_NAMESPACE_BEGIN;//ネームスペース：開始

//--------------------------------------------------------------------------------
//ダミーデバッグポーズ
//--------------------------------------------------------------------------------

#ifdef GASHA_ASSERTION_IS_ENABLED//アサーション無効時はまるごと無効化

//----------------------------------------
//ダミーデバッグポーズ

//デバッガ用ブレークポイント割り込み
void dummyDebugPause::breakPoint()
{
	//何もしない
}

//ポーズ処理呼び出し
void dummyDebugPause::pause()
{
	//何もしない
}

#endif//GASHA_ASSERTION_IS_ENABLED//アサーション無効時はまるごと無効化

GASHA_NAMESPACE_END;//ネームスペース：終了

// End of file
