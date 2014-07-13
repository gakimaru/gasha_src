//--------------------------------------------------------------------------------
// vs_console.cpp
// Visual Studio出力ウインドウ【関数／実体定義部】
//
// Gakimaru's researched and standard library for C++ - GASHA
//   Copyright (c) 2014 Itagaki Mamoru
//   Released under the MIT license.
//     https://github.com/gakimaru/gasha/blob/master/LICENSE
//--------------------------------------------------------------------------------

#include <gasha/vs_console.inl>//Visual Studio出力ウインドウ【インライン関数／テンプレート関数定義部】

#ifdef GASHA_USE_VS_CONSOLE
#include <Windows.h>//OutputDebugString()
#endif//GASHA_USE_VS_CONSOLE

GASHA_NAMESPACE_BEGIN;//ネームスペース：開始

//--------------------------------------------------------------------------------
//Visual Studio出力ウインドウ
//--------------------------------------------------------------------------------

#ifdef GASHA_HAS_DEBUG_LOG//デバッグログ無効時はまるごと無効化

//----------------------------------------
//Visual Studio出力ウインドウクラス
#ifdef GASHA_USE_VS_CONSOLE
//Visual Studio出力ウインドウクラス有効時

//出力開始
void vsConsole::beginOutput()
{
	//何もしない
}

//出力終了
void vsConsole::endOutput()
{
	//何もしない
}

//出力
void vsConsole::output(const char* str)
{
	OutputDebugStringA(str);
}

//カラー変更
void vsConsole::changeColor(GASHA_ consoleColor&& color)
{
	//何もしない
}

//カラーリセット
void vsConsole::resetColor()
{
	//何もしない
}

//デストラクタ
vsConsole::~vsConsole()
{}

#endif//GASHA_USE_VS_CONSOLE

#endif//GASHA_HAS_DEBUG_LOG//デバッグログ無効時はまるごと無効化

GASHA_NAMESPACE_END;//ネームスペース：終了

// End of file
