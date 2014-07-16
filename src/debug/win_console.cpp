//--------------------------------------------------------------------------------
// win_console.cpp
//  Windowsコマンドプロントプト【関数／実体定義部】
//
// Gakimaru's researched and standard library for C++ - GASHA
//   Copyright (c) 2014 Itagaki Mamoru
//   Released under the MIT license.
//     https://github.com/gakimaru/gasha/blob/master/LICENSE
//--------------------------------------------------------------------------------

#include <gasha/win_console.inl>// Windowsコマンドプロントプト【インライン関数／テンプレート関数定義部】

#include <cstring>//memset()
#include <cstdio>//fprintf(), fflush()

#ifdef GASHA_USE_WINDOWS_CONSOLE
#include <Windows.h>//SetConsoleTextAttribute(), GetConsoleScreenBufferInfo()
#endif//GASHA_USE_WINDOWS_CONSOLE

GASHA_NAMESPACE_BEGIN;//ネームスペース：開始

//--------------------------------------------------------------------------------
//Windowsコマンドプロントプト
//--------------------------------------------------------------------------------

#ifdef GASHA_HAS_DEBUG_LOG//デバッグログ無効時はまるごと無効化

#ifdef GASHA_USE_WINDOWS_CONSOLE
//Windowsコマンドプロンプトクラス有効時

//----------------------------------------
//Windowsコマンドプロントプトクラス

//出力開始
void winConsole::beginOutput()
{
	//何もしない
}

//出力終了
void winConsole::endOutput()
{
	std::fflush(m_handle);
}

//出力
void winConsole::output(const char* str)
{
	std::fprintf(m_handle, str);
}

//カラー変更
void winConsole::changeColor(GASHA_ consoleColor&& color)
{
#ifdef GASHA_USE_WINDOWS_CONSOLE_COLOR
	const GASHA_ consoleColor::color_t fore = color.fore();
	const GASHA_ consoleColor::color_t back = color.back();
	
	//リセット判定
	if (fore == GASHA_ consoleColor::STANDARD && back == GASHA_ consoleColor::STANDARD)
	{
		resetColor();
		return;
	}

	//デフォルトカラー取得
	const WORD default_color = m_screenBuffer.wAttributes;
	
	//カラー値を変更（0ビット目と2ビット目を入れ替える）
	auto changeColor = [](GASHA_ consoleColor::color_t color) -> WORD
	{
		WORD _color = static_cast<WORD>(color);
		return (_color & 0xa) | ((_color & 0x1) != 0x0 ? 0x4 : 0x0) | ((_color & 0x4) != 0x0 ? 0x1 : 0x0);
	};
	const WORD _fore = changeColor(fore);
	const WORD _back = changeColor(back);
	static const int BG_SHIFT = 4;//背景色指定時のビットシフト数
	WORD _color = default_color;//新しいカラー

	//前景色
	if (_fore != GASHA_ consoleColor::STANDARD)
		_color = ((_fore & GASHA_ consoleColor::RGBI) | (_color & (GASHA_ consoleColor::RGBI << BG_SHIFT)));

	//背景色
	if (_back != GASHA_ consoleColor::STANDARD)
		_color = ((_color & GASHA_ consoleColor::RGBI) | ((_back & GASHA_ consoleColor::RGBI) << BG_SHIFT));

	//反映
	SetConsoleTextAttribute(m_hWin, _color);
#endif//GASHA_USE_WINDOWS_CONSOLE_COLOR
}

//カラーリセット
void winConsole::resetColor()
{
#ifdef GASHA_USE_WINDOWS_CONSOLE_COLOR
	//デフォルトカラー取得
	const WORD default_color = m_screenBuffer.wAttributes;
	
	//リセット
	SetConsoleTextAttribute(m_hWin, default_color);
#endif//GASHA_USE_WINDOWS_CONSOLE_COLOR
}

//出力先が同じか判定
bool winConsole::isSame(const IConsole* rhs) const
{
	const winConsole* _rhs = dynamic_cast<const winConsole*>(rhs);
	if (!_rhs)
		return false;
	return (m_handle == _rhs->m_handle);
}

//コンストラクタ
winConsole::winConsole(std::FILE* handle, const char* name) :
	m_name(name),
	m_handle(handle),
	m_hWin(INVALID_HANDLE_VALUE)
{
	//Windowsハンドルを取得
	if (m_handle == stdout)
		m_hWin = GetStdHandle(STD_OUTPUT_HANDLE);
	else if (m_handle == stderr)
		m_hWin = GetStdHandle(STD_ERROR_HANDLE);

	//スクリーンバッファ取得
	if (m_hWin != INVALID_HANDLE_VALUE)
		GetConsoleScreenBufferInfo(m_hWin, &m_screenBuffer);
	else
		std::memset(&m_screenBuffer, 0, sizeof(m_screenBuffer));
}

//デストラクタ
winConsole::~winConsole()
{}

#endif//GASHA_USE_WINDOWS_CONSOLE

#endif//GASHA_HAS_DEBUG_LOG//デバッグログ無効時はまるごと無効化

GASHA_NAMESPACE_END;//ネームスペース：終了

// End of file
