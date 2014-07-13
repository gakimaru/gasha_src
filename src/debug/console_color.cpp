//--------------------------------------------------------------------------------
// console_color.cpp
// コンソールカラー【関数／実体定義部】
//
// Gakimaru's researched and standard library for C++ - GASHA
//   Copyright (c) 2014 Itagaki Mamoru
//   Released under the MIT license.
//     https://github.com/gakimaru/gasha/blob/master/LICENSE
//--------------------------------------------------------------------------------

#include <gasha/console_color.inl>//コンソールカラー【インライン関数／テンプレート関数定義部】

#include <cstdio>//sprintf()

//【VC++】ワーニング設定を退避
#pragma warning(push)

//【VC++】sprintf を使用すると、error C4996 が発生する
//  error C4996: 'sprintf': This function or variable may be unsafe. Consider using strncpy_s instead. To disable deprecation, use _CRT_SECURE_NO_WARNINGS. See online help for details.
#pragma warning(disable: 4996)//C4996を抑える

GASHA_NAMESPACE_BEGIN;//ネームスペース：開始

//--------------------------------------------------------------------------------
//コンソールカラー
//--------------------------------------------------------------------------------

#ifdef GASHA_HAS_DEBUG_LOG//デバッグログ無効時はまるごと無効化

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
	int pos = std::sprintf(str, "%s+%s", toStr(m_fore), toStr(m_back));
	if (m_attr & BOLD)
		pos += std::sprintf(str + pos, "+BOLD");
	if (m_attr & UNDERLINE)
		pos += std::sprintf(str + pos, "+UNDERLINE");
	if (m_attr & REVERSE)
		pos += std::sprintf(str + pos, "+REVERSE");
	return str;
}

//----------------------------------------
//コンソールカラーリセット用構造体の実体定義
const stdConsoleColor_t stdConsoleColor;

#endif//GASHA_HAS_DEBUG_LOG//デバッグログ無効時はまるごと無効化

GASHA_NAMESPACE_END;//ネームスペース：終了

// End of file
