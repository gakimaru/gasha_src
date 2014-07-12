//--------------------------------------------------------------------------------
// console_color.cpp
// コンソールカラー【関数／実体定義部】
//
// Gakimaru's researched and standard library for C++ - GASHA
//   Copyright (c) 2014 Itagaki Mamoru
//   Released under the MIT license.
//     https://github.com/gakimaru/gasha/blob/master/LICENSE
//--------------------------------------------------------------------------------

#include <gasha/console_color.inl>//コンソールカラー【インライン関数／テンプレート関す定義部】

#ifdef GASHA_USE_WINDOWS_CONSOLE
#include <Windows.h>//SetConsoleTextAttribute()
#endif//GASHA_USE_WINDOWS_CONSOLE

GASHA_NAMESPACE_BEGIN;//ネームスペース：開始

//--------------------------------------------------------------------------------
//コンソールカラー
//--------------------------------------------------------------------------------

//----------------------------------------
//コンソールカラークラス

//TTY用のカラー変更
void consoleColor::changeColorTTY(const consoleColor::color_t fore, const consoleColor::color_t back, const bool bold, const bool underline)
{
#ifdef GASHA_USE_ESCAPE_SEQUENCE
	//リセット
	if (fore == standard && back == standard && !bold && !underline)
	{
		m_console->printf("\x1b[0m");
		return;
	}

	char msg[32] = "\x1b[";
	static const std::size_t INIT_POS = 2;
	std::size_t pos = INIT_POS;
	auto addMsg = [&msg, &pos](const int no)
	{
		if (pos > INIT_POS)
			msg[pos++] = ';';
		if (no >= 100) msg[pos++] = '0' + (no / 100);
		if (no >= 10) msg[pos++] = '0' + (no / 10 % 10);
		msg[pos++] = '0' + no % 10;
	};

	//前景色
	if (fore == standard)
		addMsg(39);
	else
		addMsg(((fore & I) == I ? 90 : 30) + (fore & RGB));

	//背景色
	if (back == standard)
		addMsg(49);
	else
		addMsg(((back & I) == I ? 100 : 40) + (back & RGB));
	
	//強調
	if (bold)
		addMsg(1);

	//アンダーライン
	if (underline)
		addMsg(4);
	
	//出力
	if (pos > INIT_POS)
	{
		msg[pos] = 'm';
		m_console->printf(msg);
	}
#endif//GASHA_USE_ESCAPE_SEQUENCE
}

//Windowsコマンドプロンプト用のカラー変更
void consoleColor::changeColorWIN(const consoleColor::color_t fore, const consoleColor::color_t back, const bool bold, const bool underline)
{
#ifdef GASHA_USE_WINDOWS_CONSOLE_COLOR
	//デフォルトカラー取得
	const WORD default_color = m_console->screenBuffer().wAttributes;
	
	//リセット
	if (fore == standard && back == standard && !bold && !underline)
	{
		SetConsoleTextAttribute(m_console->handleWin(), default_color);
		return;
	}
	
	//カラー値を変更（0ビット目と2ビット目を入れ替える）
	auto changeColor = [](const consoleColor::color_t color) -> const consoleColor::color_t
	{
		unsigned int _color = static_cast<unsigned int>(color);
		return static_cast<consoleColor::color_t>((_color & 0xa) | ((_color & 0x1) != 0x0 ? 0x4 : 0x0) | ((_color & 0x4) != 0x0 ? 0x1 : 0x0));
	};
	const consoleColor::color_t _fore = changeColor(fore);
	const consoleColor::color_t _back = changeColor(back);
	static const int BG_SHIFT = 4;//背景色指定時のビットシフト数
	WORD _color = default_color;//新しいカラー

	//前景色
	if (_fore != standard)
		_color = ((_fore & RGBI) | (_color & (RGBI << BG_SHIFT)));

	//背景色
	if (_back != standard)
		_color = ((_color & RGBI) | ((_back & RGBI) << BG_SHIFT));

	//反映
	SetConsoleTextAttribute(m_console->handleWin(), _color);
#endif//GASHA_USE_WINDOWS_CONSOLE_COLOR
}

//Visual Studio 出力ウインドウ用のカラー変更
void consoleColor::changeColorVS(const consoleColor::color_t fore, const consoleColor::color_t back, const bool bold, const bool underline)
{
	//何もしない
}

GASHA_NAMESPACE_END;//ネームスペース：終了

// End of file
