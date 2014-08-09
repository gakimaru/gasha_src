//--------------------------------------------------------------------------------
// vs_console.cpp
// Visual Studio出力ウインドウ【関数／実体定義部】
//
// Gakimaru's standard library for C++ - GASHA
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

#ifdef GASHA_LOG_IS_ENABLED//デバッグログ無効時はまるごと無効化

//----------------------------------------
//Visual Studio出力ウインドウクラス
#ifdef GASHA_USE_VS_CONSOLE
//Visual Studio出力ウインドウクラス有効時

//出力開始
void vsConsole::begin()
{
	//何もしない
}

//出力終了
void vsConsole::end()
{
	//何もしない
}

//出力
void vsConsole::put(const char* str)
{
	OutputDebugStringA(str);
}

//改行出力
void vsConsole::putCr()
{
	//OutputDebugStringA("\r\n");
	OutputDebugStringA("\n");
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

//出力先が同じか判定
bool vsConsole::isSame(const iConsole* rhs) const
{
	const vsConsole* _rhs = dynamic_cast<const vsConsole*>(rhs);
	if (!_rhs)
		return false;
	return true;
}

//デストラクタ
vsConsole::~vsConsole()
{}

#else//GASHA_USE_VS_CONSOLE

//【VC++】LNK4221回避用のダミー関数
namespace _private{
	void vs_console_dummy(){}
}//namespace _private

#endif//GASHA_USE_VS_CONSOLE

#else//GASHA_LOG_IS_ENABLED

//【VC++】LNK4221回避用のダミー関数
namespace _private{
	void vs_console_dummy(){}
}//namespace _private

#endif//GASHA_LOG_IS_ENABLED//デバッグログ無効時はまるごと無効化

GASHA_NAMESPACE_END;//ネームスペース：終了

// End of file
