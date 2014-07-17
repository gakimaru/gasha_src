//--------------------------------------------------------------------------------
// strconv.cpp
// 文字列変換【関数／実体定義部】
//
// Gakimaru's researched and standard library for C++ - GASHA
//   Copyright (c) 2014 Itagaki Mamoru
//   Released under the MIT license.
//     https://github.com/gakimaru/gasha/blob/master/LICENSE
//--------------------------------------------------------------------------------

#include <gasha/strconv.inl>//文字列変換【インライン関数／テンプレート関数定義部】

#include <gasha/utility.h>//汎用ユーティリティ：min()

GASHA_NAMESPACE_BEGIN;//ネームスペース：開始

//----------------------------------------
//大文字変換関数
std::size_t upperCaseConv(char* dst, const std::size_t max_dst_size, const char* src, const std::size_t src_len)
{
	if (!dst || max_dst_size == 0)
		return 0;
	const std::size_t minimum_len = GASHA_ min(max_dst_size, src_len + 1) - 1;
	
	char* dst_p = dst;
	const char* src_p = src;
	for (std::size_t remain = minimum_len; remain > 0; --remain, ++dst_p, ++src_p)
	{
		char c = *(src_p);
		if (c >= 'a' && c <= 'z')
			c = c - 'a' + 'A';
		*(dst_p) = c;
	}
	*(dst_p) = '\0';
	return minimum_len;
}

//----------------------------------------
//小文字変換関数
std::size_t lowerCaseConv(char* dst, const std::size_t max_dst_size, const char* src, const std::size_t src_len)
{
	if (!dst || max_dst_size == 0)
		return 0;
	const std::size_t minimum_len = GASHA_ min(max_dst_size, src_len + 1) - 1;

	char* dst_p = dst;
	const char* src_p = src;
	for (std::size_t remain = minimum_len; remain > 0; --remain, ++dst_p, ++src_p)
	{
		char c = *(src_p);
		if (c >= 'A' && c <= 'Z')
			c = c - 'A' + 'a';
		*(dst_p) = c;
	}
	*(dst_p) = '\0';
	return minimum_len;
}


GASHA_NAMESPACE_END;//ネームスペース：終了

// End of file
