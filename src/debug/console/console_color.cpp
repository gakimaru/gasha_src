//--------------------------------------------------------------------------------
// console_color.cpp
// コンソールカラー【関数／実体定義部】
//
// Gakimaru's standard library for C++ - GASHA
//   Copyright (c) 2014 Itagaki Mamoru
//   Released under the MIT license.
//     https://github.com/gakimaru/gasha/blob/master/LICENSE
//--------------------------------------------------------------------------------

#include <gasha/console_color.inl>//コンソールカラー【インライン関数／テンプレート関数定義部】

#include <gasha/string.h>//文字列処理：spprintf()

GASHA_NAMESPACE_BEGIN;//ネームスペース：開始

//--------------------------------------------------------------------------------
//コンソールカラー
//--------------------------------------------------------------------------------

#ifdef GASHA_LOG_IS_ENABLED//デバッグログ無効時はまるごと無効化

//----------------------------------------
//コンソールカラークラス

//文字列作成
const char* consoleColor::toStr(const consoleColor::color_t color) const
{
	if (color == STANDARD)
		return "STANDARD";
	static const char* table[] = 
	{
		"BLACK",
		"RED",
		"GREEN",
		"YELLOW",
		"BLUE",
		"MAGENTA",
		"CYAN",
		"WHITE",
		"iBLACK",
		"iRED",
		"iGREEN",
		"iYELLOW",
		"iBLUE",
		"iMAGENTA",
		"iCYAN",
		"iWHITE",
	};
	return table[color];
}
const char* consoleColor::toStr(char* str) const
{
	int pos = GASHA_ spprintf(str, "%s+%s", toStr(m_fore), toStr(m_back));
	if (m_attr & BOLD)
		pos += GASHA_ spprintf(str + pos, "+BOLD");
	if (m_attr & UNDERLINE)
		pos += GASHA_ spprintf(str + pos, "+UNDERLINE");
	if (m_attr & REVERSE)
		pos += GASHA_ spprintf(str + pos, "+REVERSE");
	return str;
}

#endif//GASHA_LOG_IS_ENABLED//デバッグログ無効時はまるごと無効化

//----------------------------------------
//コンソールカラーリセット用構造体の実体定義
const consoleColor::stdColor_type consoleColor::stdColor;

GASHA_NAMESPACE_END;//ネームスペース：終了

// End of file
