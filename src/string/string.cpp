//--------------------------------------------------------------------------------
// string.cpp
// 文字列処理【関数／実体定義部】
//
// Gakimaru's researched and standard library for C++ - GASHA
//   Copyright (c) 2014 Itagaki Mamoru
//   Released under the MIT license.
//     https://github.com/gakimaru/gasha/blob/master/LICENSE
//--------------------------------------------------------------------------------

#include <gasha/string.inl>//文字列処理【インライン関数／テンプレート関数定義部】

#include <gasha/fast_math.h>//高速算術

#include <cstdint>//std::intptr_t, std::uint32_t
#include <cstring>//std::memcpy(), std::memset()

//【VC++】str*** を使用すると、error C4996 が発生する
//  error C4996: 'str***': This function or variable may be unsafe. Consider using strncpy_s instead. To disable deprecation, use _CRT_SECURE_NO_WARNINGS. See online help for details.
#pragma warning(disable: 4996)//C4996を抑える

GASHA_NAMESPACE_BEGIN;//ネームスペース：開始

//----------------------------------------
//strstr(BM法)
namespace _private
{
	//strstr(BM法)用補助関数:パターンが1文字専用
	const char* strstrbm1(const char* str, const char pattern, std::function<bool(const char*, const char*)> found_it)
	{
		//検索開始
		const std::size_t str_len = GASHA_ strlen(str);
		const char* str_end_p = str + str_len;
		const char* str_p = str;
		while (str_p < str_end_p)
		{
			const char str_c = *str_p;
			if (str_c == pattern)
			{
				if (found_it(str_p, str))
					return str_p;
			}
			//パターンの途中の文字が不一致
			++str_p;
		}
		return nullptr;
	}
}
//strstr(BM法)
const char* strstrbm(const char* str, const char* pattern, std::function<bool(const char*, const char*)> found_it)
{
//nullチェックしない
//	if (!str || !pattern)
//		return 0;
	//patternの長さに基づいて、処理を振り分ける
	if (*pattern == '\0')//パターンが0文字の時
		return str;
	if (*(pattern + 1) == '\0')//パターンが1文字の時
	{
		if (*str == '\0')
			return nullptr;
		return _private::strstrbm1(str, *pattern, found_it);
	}
	const std::size_t pattern_len = GASHA_ strlen(pattern);
	const std::size_t str_len = GASHA_ strlen(str);
	if (str_len < pattern_len)
		return nullptr;
	//パターン内の文字ごとに、照合失敗時のスキップ文字数を記録
	int skip[256];
	for (std::size_t i = 0; i < 256; ++i)
		skip[i] = static_cast<int>(pattern_len);
	for (std::size_t i = 0; i < pattern_len; ++i)
		skip[static_cast<unsigned char>(pattern[i])] = static_cast<int>(pattern_len) - static_cast<int>(i) - 1;
	//検索開始
	const std::size_t pattern_term = pattern_len - 1;
	const char* pattern_term_p = pattern + pattern_term;
	const char* pattern_term_1_p = pattern_term_p - 1;
	const char pattern_term_c = *pattern_term_p;
	const char* str_end_p = str + str_len;
	const char* str_p = str + pattern_len - 1;
	while (str_p < str_end_p)
	{
		const char str_c = *str_p;
		if (str_c == pattern_term_c)
		{
			//パターンの末尾の文字が一致 ... パターンを照合する
			const char* _str_p = str_p - 1;
			const char* _pattern_p = pattern_term_1_p;
			char _str_c = *_str_p;
			while (true)
			{
				if (_str_c != *_pattern_p)//パターン不一致
					break;
				if (_pattern_p == pattern)//パターン検出
				{
					if (found_it(_str_p, str))
						return _str_p;
					break;
				}
				--_pattern_p;
				--_str_p;
				_str_c = *_str_p;
			}
			//パターンの途中の文字が不一致 ... パターンの中に次の文字が見つかる位置まで移動
			_str_c = *(++str_p);
			const int _skip = skip[static_cast<unsigned char>(_str_c)];
			str_p += _skip;
		}
		else
		{
			//パターンの末尾の文字が不一致 ... パターンの中に str_c が見つかる位置まで移動
			const int _skip = skip[static_cast<unsigned char>(str_c)];
			str_p += _skip;
		}
	}
	return nullptr;
}

GASHA_NAMESPACE_END;//ネームスペース：終了

// End of file
