﻿//--------------------------------------------------------------------------------
// tty_console.cpp
// TTY端末【関数／実体定義部】
//
// Gakimaru's standard library for C++ - GASHA
//   Copyright (c) 2014 Itagaki Mamoru
//   Released under the MIT license.
//     https://github.com/gakimaru/gasha/blob/master/LICENSE
//--------------------------------------------------------------------------------

#include <gasha/tty_console.inl>//TTY端末【インライン関数／テンプレート関数定義部】

#include <cstdio>//std::fprintf(), std::fflush()

GASHA_NAMESPACE_BEGIN;//ネームスペース：開始

//--------------------------------------------------------------------------------
//TTY端末
//--------------------------------------------------------------------------------

#ifdef GASHA_LOG_IS_ENABLED//デバッグログ無効時はまるごと無効化

//----------------------------------------
//TTY端末クラス

//出力開始
void ttyConsole::begin()
{
	resetColor();
}

//出力終了
void ttyConsole::end()
{
	resetColor();
	std::fflush(m_handle);
}

//出力
void ttyConsole::put(const char* str)
{
	std::fprintf(m_handle, str);
}

//改行出力
void ttyConsole::putCr()
{
	resetColor();
	std::fprintf(m_handle, "\n");
}

//TTY用のカラー変更
void ttyConsole::changeColor(GASHA_ consoleColor&& color)
{
#ifdef GASHA_USE_ESCAPE_SEQUENCE
	if (m_currColor == color)
		return;
	m_currColor = color;

	const GASHA_ consoleColor::color_t fore = color.fore();
	const GASHA_ consoleColor::color_t back = color.back();
	const GASHA_ consoleColor::attr_t attr = color.attr();

	//リセット判定
	if (fore == GASHA_ consoleColor::STANDARD && back == GASHA_ consoleColor::STANDARD && attr == GASHA_ consoleColor::NOATTR)
	{
		resetColor();
		return;
	}

	//エスケープシーケンス文字列用バッファと作成用関数
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
	if (fore == GASHA_ consoleColor::STANDARD)
		addMsg(39);
	else
		addMsg(((fore & GASHA_ consoleColor::I) == GASHA_ consoleColor::I ? 90 : 30) + (fore & GASHA_ consoleColor::RGB));

	//背景色
	if (back == GASHA_ consoleColor::STANDARD)
		addMsg(49);
	else
		addMsg(((back & GASHA_ consoleColor::I) == GASHA_ consoleColor::I ? 100 : 40) + (back & GASHA_ consoleColor::RGB));
	
	//強調
	if (attr & GASHA_ consoleColor::BOLD)
		addMsg(1);

	//下線
	if (attr & GASHA_ consoleColor::UNDERLINE)
		addMsg(4);

	//反転
	if (attr & GASHA_ consoleColor::REVERSE)
		addMsg(7);
	
	//調整
	if (pos == INIT_POS)
		msg[pos++] = '0';
	
	//最終文字
	msg[pos] = 'm';
	
	//出力
	std::fprintf(m_handle, msg);
#endif//GASHA_USE_ESCAPE_SEQUENCE
}

//カラーリセット
void ttyConsole::resetColor()
{
#ifdef GASHA_USE_ESCAPE_SEQUENCE
	if (m_currColor.isStandard())
		return;
	m_currColor.reset();

	//リセット
	std::fprintf(m_handle, "\x1b[0m");
#endif//GASHA_USE_ESCAPE_SEQUENCE
}

//出力先が同じか判定
bool ttyConsole::isSame(const iConsole* rhs) const
{
	const ttyConsole* _rhs = dynamic_cast<const ttyConsole*>(rhs);
	if (!_rhs)
		return false;
	return (m_handle == _rhs->m_handle);
}

//デストラクタ
ttyConsole::~ttyConsole()
{
	resetColor();
}

//----------------------------------------
//カラーなしTTY端末クラス

//カラー変更
void monoTtyConsole::changeColor(GASHA_ consoleColor&& color)
{
	//何もしない
}

//デストラクタ
monoTtyConsole::~monoTtyConsole()
{}

#else//GASHA_LOG_IS_ENABLED

//【VC++】LNK4221回避用のダミー関数
namespace _private{
	void tty_console_dummy(){}
}//namespace _private

#endif//GASHA_LOG_IS_ENABLED//デバッグログ無効時はまるごと無効化

GASHA_NAMESPACE_END;//ネームスペース：終了

// End of file
