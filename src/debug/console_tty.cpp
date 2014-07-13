//--------------------------------------------------------------------------------
// console_tty.cpp
// TTY端末【関数／実体定義部】
//
// Gakimaru's researched and standard library for C++ - GASHA
//   Copyright (c) 2014 Itagaki Mamoru
//   Released under the MIT license.
//     https://github.com/gakimaru/gasha/blob/master/LICENSE
//--------------------------------------------------------------------------------

#include <gasha/console_tty.inl>//TTY端末【インライン関数／テンプレート関数定義部】

#include <stdio.h>//fprintf(), fflush()

GASHA_NAMESPACE_BEGIN;//ネームスペース：開始

//--------------------------------------------------------------------------------
//TTY端末
//--------------------------------------------------------------------------------

#ifdef GASHA_HAS_DEBUG_LOG//デバッグログ無効時はまるごと無効化

//----------------------------------------
//TTY端末クラス

//出力開始
void consoleTTY::beginOutput()
{
	//何もしない
}

//出力終了
void consoleTTY::endOutput()
{
	fflush(m_handle);
}

//出力
void consoleTTY::output(const char* str)
{
	::fprintf(m_handle, str);
}

//TTY用のカラー変更
void consoleTTY::changeColor(const GASHA_ consoleColor& color)
{
#ifdef GASHA_USE_ESCAPE_SEQUENCE
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
	output(msg);
#endif//GASHA_USE_ESCAPE_SEQUENCE
}

//カラーリセット
void consoleTTY::resetColor()
{
#ifdef GASHA_USE_ESCAPE_SEQUENCE
	//リセット
	output("\x1b[0m");
#endif//GASHA_USE_ESCAPE_SEQUENCE
}

//デストラクタ
consoleTTY::~consoleTTY()
{}

#endif//GASHA_HAS_DEBUG_LOG//デバッグログ無効時はまるごと無効化

GASHA_NAMESPACE_END;//ネームスペース：終了

// End of file
