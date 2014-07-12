//--------------------------------------------------------------------------------
// console.cpp
// コンソール【関数／実体定義部】
//
// Gakimaru's researched and standard library for C++ - GASHA
//   Copyright (c) 2014 Itagaki Mamoru
//   Released under the MIT license.
//     https://github.com/gakimaru/gasha/blob/master/LICENSE
//--------------------------------------------------------------------------------

#include <gasha/console.inl>//コンソール【インライン関数／テンプレート関数定義部】

#include <memory.h>//memset()

GASHA_NAMESPACE_BEGIN;//ネームスペース：開始

//--------------------------------------------------------------------------------
//コンソール
//--------------------------------------------------------------------------------

//----------------------------------------
//コンソールクラス

//コンストラクタ：Windowsコマンドプロンプト向け
console::console(const console::WIN_t&, FILE* handle) :
#ifdef GASHA_USE_WINDOWS_CONSOLE
	m_type(WIN),
	m_handleTTY(handle),
	m_handleWin(INVALID_HANDLE_VALUE)
{
	//Windowsハンドルを取得
	if (handle == stdout)
		m_handleWin = GetStdHandle(STD_OUTPUT_HANDLE);
	else if (handle == stderr)
		m_handleWin = GetStdHandle(STD_ERROR_HANDLE);
	
	//スクリーンバッファ取得
	if (m_handleWin != INVALID_HANDLE_VALUE)
		GetConsoleScreenBufferInfo(m_handleWin, &m_screenBuffer);
	else
		memset(&m_screenBuffer, 0, sizeof(m_screenBuffer));
}
#else//GASHA_USE_WINDOWS_CONSOLE
	console::console(console_TTY, stdout)//委譲コンストラクタ
{}
#endif//GASHA_USE_WINDOWS_CONSOLE

//定数
const console::TTY_t console_TTY;//TTY端末
const console::WIN_t console_WIN;//Windowsコマンドプロンプト
const console::VS_t console_VS;//Visual Studioの出力ウインドウ

GASHA_NAMESPACE_END;//ネームスペース：終了

// End of file
