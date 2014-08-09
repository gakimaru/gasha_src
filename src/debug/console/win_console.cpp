//--------------------------------------------------------------------------------
// win_console.cpp
//  Windowsコマンドプロントプト【関数／実体定義部】
//
// Gakimaru's standard library for C++ - GASHA
//   Copyright (c) 2014 Itagaki Mamoru
//   Released under the MIT license.
//     https://github.com/gakimaru/gasha/blob/master/LICENSE
//--------------------------------------------------------------------------------

#include <gasha/win_console.inl>// Windowsコマンドプロントプト【インライン関数／テンプレート関数定義部】

#include <cstring>//std::memset()
#include <cstdio>//std::fprintf(), std::fflush()

//#include <Windows.h>//SetConsoleTextAttribute(), GetConsoleScreenBufferInfo()

GASHA_NAMESPACE_BEGIN;//ネームスペース：開始

//--------------------------------------------------------------------------------
//Windowsコマンドプロントプト
//--------------------------------------------------------------------------------

#ifdef GASHA_LOG_IS_ENABLED//デバッグログ無効時はまるごと無効化

#ifdef GASHA_USE_WINDOWS_CONSOLE
//Windowsコマンドプロンプトクラス有効時

//----------------------------------------
//Windowsコマンドプロントプトクラス

//出力開始
void winConsole::begin()
{
	resetColor();
}

//出力終了
void winConsole::end()
{
	resetColor();
	std::fflush(m_handle);
}

//出力
void winConsole::put(const char* str)
{
	std::fprintf(m_handle, str);
}

//改行出力
void winConsole::putCr()
{
	resetColor();
	//std::fprintf(m_handle, "\r\n");
	std::fprintf(m_handle, "\n");
}

//カラー変更
void winConsole::changeColor(GASHA_ consoleColor&& color)
{
#ifdef GASHA_USE_WINDOWS_CONSOLE_COLOR
	if (m_currColor == color)
		return;
	m_currColor = color;

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

	//カラー変更前にフラッシュ
	std::fflush(m_handle);

	//反映
	SetConsoleTextAttribute(m_hWin, _color);
#endif//GASHA_USE_WINDOWS_CONSOLE_COLOR
}

//カラーリセット
void winConsole::resetColor()
{
#ifdef GASHA_USE_WINDOWS_CONSOLE_COLOR
	if (m_currColor.isStandard())
		return;
	m_currColor.reset();

	//デフォルトカラー取得
	const WORD default_color = m_screenBuffer.wAttributes;

	//カラー変更前にフラッシュ
	std::fflush(m_handle);

	//リセット
	SetConsoleTextAttribute(m_hWin, default_color);
#endif//GASHA_USE_WINDOWS_CONSOLE_COLOR
}

//出力先が同じか判定
bool winConsole::isSame(const iConsole* rhs) const
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
	m_hWin(INVALID_HANDLE_VALUE),
	m_currColor(consoleColor::stdColor)
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
{
	resetColor();
}

//----------------------------------------
//カラーなしWindowsコマンドプロントプトクラス

//カラー変更
void monoWinConsole::changeColor(GASHA_ consoleColor&& color)
{
	//何もしない
}

//デストラクタ
monoWinConsole::~monoWinConsole()
{}

#else//GASHA_USE_WINDOWS_CONSOLE

//【VC++】LNK4221回避用のダミー関数
namespace _private{
	void win_console_dummy(){}
}//namespace _private

#endif//GASHA_USE_WINDOWS_CONSOLE

#else//GASHA_LOG_IS_ENABLED

//【VC++】LNK4221回避用のダミー関数
namespace _private{
	void win_console_dummy(){}
}//namespace _private

#endif//GASHA_LOG_IS_ENABLED//デバッグログ無効時はまるごと無効化

GASHA_NAMESPACE_END;//ネームスペース：終了

// End of file
