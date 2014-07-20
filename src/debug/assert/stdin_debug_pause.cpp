//--------------------------------------------------------------------------------
// stdin_debug_pause.cpp
// 標準入力デバッグポーズ【関数／実体定義部】
//
// Gakimaru's researched and standard library for C++ - GASHA
//   Copyright (c) 2014 Itagaki Mamoru
//   Released under the MIT license.
//     https://github.com/gakimaru/gasha/blob/master/LICENSE
//--------------------------------------------------------------------------------

#include <gasha/stdin_debug_pause.inl>//標準入力デバッグポーズ【インライン関数／テンプレート関数定義部】

#include <gasha/simple_assert.inl>//シンプルアサーション

#include <cstdio>//std::printf(), std::getchar()

GASHA_NAMESPACE_BEGIN;//ネームスペース：開始

//--------------------------------------------------------------------------------
//標準入力デバッグポーズ
//--------------------------------------------------------------------------------

#ifdef GASHA_ASSERTION_IS_ENABLED//アサーション無効時はまるごと無効化

//----------------------------------------
//標準入力デバッグポーズ

//デバッガのブレークポイント発動
void stdinDebugPause::breakPoint()
{
	//debuggerBreak();//シンプルアサーション無効化
}

//ポーズ処理呼び出し
void stdinDebugPause::pause()
{
#ifdef GASHA_STDIN_IS_AVAILABLE
	std::printf("Hit enter key ...");
	while (true)
	{
		int c = std::getchar();
		if (c == '\r' || c == '\n')
			break;
	}
	//std::printf("\n");
#endif// GASHA_STDIN_IS_AVAILABLE
}

#endif//GASHA_ASSERTION_IS_ENABLED//アサーション無効時はまるごと無効化

GASHA_NAMESPACE_END;//ネームスペース：終了

// End of file
