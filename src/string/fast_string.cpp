﻿//--------------------------------------------------------------------------------
// fast_string.cpp
// 高速文字列処理
//
// Gakimaru's researched and standard library for C++ - GASHA
//   Copyright (c) 2014 Itagaki Mamoru
//   Released under the MIT license.
//     https://github.com/gakimaru/gasha/blob/master/LICENSE
//--------------------------------------------------------------------------------

#include <gasha/fast_string.h>//高速文字列処理【宣言部】
#include <gasha/fast_string.inl>//高速文字列処理【インライン関数／テンプレート関数定義部】

#include <gasha/fast_math.h>//高速算術

#include <cstdint>//intptr_t, std::uint32_t
#include <memory.h>//memcpy()

#ifdef GASHA_USE_SSE4_2
#include <nmmintrin.h>//SSE4.2
#endif//GASHA_USE_SSE4_2

//【VC++】str*** を使用すると、error C4996 が発生する
//  error C4996: 'str***': This function or variable may be unsafe. Consider using strncpy_s instead. To disable deprecation, use _CRT_SECURE_NO_WARNINGS. See online help for details.
#pragma warning(disable: 4996)//C4996を抑える

GASHA_NAMESPACE_BEGIN;//ネームスペース：開始

#ifdef GASHA_IS_GCC
//----------------------------------------
//通常版strnlen
std::size_t strnlen(const char* str, const std::size_t max_len)
{
//nullチェックしない
//	if (!str)
//		return 0;
	const char* p_end = str + max_len;
	const char* p = str;
	while (p <= p_end)
	{
		if (!*p)
			return static_cast<std::size_t>(p - str);
		++p;
	}
	return max_len;
}
#endif//GASHA_IS_GCC

//----------------------------------------
//通常版strstr(BM法)
namespace _private
{
	//通常版strstr(BM法)用補助関数:パターンが1文字専用
	const char* strstrbm1(const char* str, const char pattern, std::function<bool(const char*, const char*)> found_it)
	{
		//検索開始
		const std::size_t str_len = strlen(str);
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
//通常版strstr(BM法)
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
	const std::size_t pattern_len = strlen_sse(pattern);
	const std::size_t str_len = strlen_sse(str);
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

//----------------------------------------
//strstr0
const char* strstr0(const char* str, const char* pattern)
{
	const std::size_t pattern_len = strlen(pattern);
	const int ret = strncmp(str, pattern, pattern_len);
	if (ret == 0)
		return str;
	return nullptr;
}

#ifdef GASHA_USE_SSE4_2

//----------------------------------------
//SSE版strlen
std::size_t strlen_sse(const char* str)
{
//nullチェックしない
//	if (!str)
//		return 0;
	static const int flags = _SIDD_SBYTE_OPS | _SIDD_CMP_EQUAL_EACH | _SIDD_POSITIVE_POLARITY | _SIDD_LEAST_SIGNIFICANT;
	const __m128i null = _mm_setzero_si128();
	const char* p = str;
	const std::size_t str_over = reinterpret_cast<intptr_t>(str)& 0xf;
	if (str_over != 0)
	{
		//非16バイトアランイメント時
		const __m128i str16 = _mm_loadu_si128(reinterpret_cast<const __m128i*>(p));
		const int         zf  = _mm_cmpistrz(null, str16, flags);//※この二行は、コンパイル後は1回の cmpistri になる（はず）
		const std::size_t pos = _mm_cmpistri(null, str16, flags);
		if (zf)
			return pos;
		p += (16 - str_over);
	}
	//16バイトアランイメント時
	const __m128i* p128 = reinterpret_cast<const __m128i*>(p);
	while (true)
	{
		const __m128i str16 = _mm_load_si128(p128);
		const int         zf  = _mm_cmpistrz(null, str16, flags);//※この二行は、コンパイル後は1回の cmpistri になる（はず）
		const std::size_t pos = _mm_cmpistri(null, str16, flags);
		if (zf)
			return (reinterpret_cast<const char*>(p128)-str) + pos;
		++p128;
	}
	return 0;//dummy
}

//----------------------------------------
//SSE版strnlen
std::size_t strnlen_sse(const char* str, const std::size_t max_len)
{
//nullチェックしない
//	if (!str)
//		return 0;
	static const int flags = _SIDD_SBYTE_OPS | _SIDD_CMP_EQUAL_EACH | _SIDD_POSITIVE_POLARITY | _SIDD_LEAST_SIGNIFICANT;
	const __m128i null = _mm_setzero_si128();
	const char* p_end = str + max_len;
	const char* p_end_16 = p_end - 16;
	const char* p = str;
	const std::size_t str_over = reinterpret_cast<intptr_t>(str)& 0xf;
	if (str_over != 0)
	{
		//非16バイトアランイメント時
		const __m128i str16 = _mm_loadu_si128(reinterpret_cast<const __m128i*>(p));
		const int         zf  = _mm_cmpistrz(null, str16, flags);//※この二行は、コンパイル後は1回の cmpistri になる（はず）
		const std::size_t pos = _mm_cmpistri(null, str16, flags);
		if (zf)
		{
			if (pos < max_len)
				return pos;
			return max_len;
		}
		p += (16 - str_over);
	}
	//16バイトアランイメント時
	const __m128i* p128 = reinterpret_cast<const __m128i*>(p);
	while (reinterpret_cast<const char*>(p128) <= p_end_16)
	{
		const __m128i str16 = _mm_load_si128(p128);
		const int         zf  = _mm_cmpistrz(null, str16, flags);//※この二行は、コンパイル後は1回の cmpistri になる（はず）
		const std::size_t pos = _mm_cmpistri(null, str16, flags);
		if (zf)
			return (reinterpret_cast<const char*>(p128)-str) + pos;
		++p128;
	}
	if (reinterpret_cast<const char*>(p128) < p_end)
	{
		const __m128i str16 = _mm_load_si128(p128);
		const int         zf  = _mm_cmpistrz(null, str16, flags);//※この二行は、コンパイル後は1回の cmpistri になる（はず）
		const std::size_t pos = _mm_cmpistri(null, str16, flags);
		if (zf)
		{
			const std::size_t len = (reinterpret_cast<const char*>(p128)-str) + pos;
			if (len <= max_len)
				return len;
		}
	}
	return max_len;
}

//----------------------------------------
//SSE版strcmp
namespace _private
{
	//SSE版strcmp補助関数
	//アラインメント判定
	//戻り値:
	//  0 ... 16バイトアラインメント＋16バイトアラインメント
	//  1 ... 16バイトアラインメント＋非16バイトアラインメント
	//  2 ... 非16バイトアラインメント＋16バイトアラインメント
	//  3 ... 非16バイトアラインメント＋非16バイトアラインメント
	inline static int sse_str_pattern(const char* str1, const char* str2)
	{
		return (((reinterpret_cast<intptr_t>(str1)& 0xf) != 0) << 0) |
			(((reinterpret_cast<intptr_t>(str2)& 0xf) != 0) << 1);
	}
	//文字の大小判定
	//戻り値: 0 ... 一致、1以上 ... val1の方が大きい、-1以下 ... val2の方が大きい
	//       ※VC++の場合は、0, 1, -1 のいずれかの値を返す
	inline static int sse_chr_compare(const int val1, const int val2)
	{
	#ifdef GASHA_IS_VC
		const int val = val1 - val2;
		return (val >> 31) | (val != 0);
	#else//GASHA_IS_VC
		return val1 - val2;
	#endif//GASHA_IS_VC
	}
	//SSE版strcmp:16バイトアランイメント＋16バイトアランイメント時
	static int strcmp_sse_case0(const char* str1, const char* str2)
	{
		static const int flags = _SIDD_SBYTE_OPS | _SIDD_CMP_EQUAL_EACH | _SIDD_NEGATIVE_POLARITY | _SIDD_LEAST_SIGNIFICANT;
		const __m128i* p1 = reinterpret_cast<const __m128i*>(str1);
		const __m128i* p2 = reinterpret_cast<const __m128i*>(str2);
		while (true)
		{
			const __m128i str16_1 = _mm_load_si128(p1);
			const __m128i str16_2 = _mm_load_si128(p2);
			const int         cf  = _mm_cmpistrc(str16_1, str16_2, flags);//※この三行は、コンパイル後は1回の cmpistri になる（はず）
			const int         zf  = _mm_cmpistrz(str16_1, str16_2, flags);
			const std::size_t pos = _mm_cmpistri(str16_1, str16_2, flags);
			if (cf)
			{
				const char c1 = *(reinterpret_cast<const char*>(p1)+pos);
				const char c2 = *(reinterpret_cast<const char*>(p2)+pos);
				return sse_chr_compare(c1, c2);
			}
			if (zf)
				return 0;
			++p1;
			++p2;
		}
		return 0;//dummy
	}
	//SSE版strcmp:非16バイトアランイメント＋16バイトアランイメント時
	static int strcmp_sse_case1(const char* str1, const char* str2)
	{
		static const int flags = _SIDD_SBYTE_OPS | _SIDD_CMP_EQUAL_EACH | _SIDD_NEGATIVE_POLARITY | _SIDD_LEAST_SIGNIFICANT;
		const char* p1 = str1;
		const __m128i* p2 = reinterpret_cast<const __m128i*>(str2);
		while (true)
		{
			const __m128i str16_1 = _mm_loadu_si128(reinterpret_cast<const __m128i*>(p1));
			const __m128i str16_2 = _mm_load_si128(p2);
			const int         cf  = _mm_cmpistrc(str16_1, str16_2, flags);//※この三行は、コンパイル後は1回の cmpistri になる（はず）
			const int         zf  = _mm_cmpistrz(str16_1, str16_2, flags);
			const std::size_t pos = _mm_cmpistri(str16_1, str16_2, flags);
			if (cf)
			{
				const char c1 = *(p1 + pos);
				const char c2 = *(reinterpret_cast<const char*>(p2)+pos);
				return sse_chr_compare(c1, c2);
			}
			if (zf)
				return 0;
			p1 += 16;
			++p2;
		}
		return 0;//dummy
	}
	//SSE版strcmp:16バイトアランイメント＋非16バイトアランイメント時
	static int strcmp_sse_case2(const char* str1, const char* str2)
	{
		static const int flags = _SIDD_SBYTE_OPS | _SIDD_CMP_EQUAL_EACH | _SIDD_NEGATIVE_POLARITY | _SIDD_LEAST_SIGNIFICANT;
		const __m128i* p1 = reinterpret_cast<const __m128i*>(str1);
		const char* p2 = str2;
		while (true)
		{
			const __m128i str16_1 = _mm_load_si128(p1);
			const __m128i str16_2 = _mm_loadu_si128(reinterpret_cast<const __m128i*>(p2));
			const int         cf  = _mm_cmpistrc(str16_1, str16_2, flags);//※この三行は、コンパイル後は1回の cmpistri になる（はず）
			const int         zf  = _mm_cmpistrz(str16_1, str16_2, flags);
			const std::size_t pos = _mm_cmpistri(str16_1, str16_2, flags);
			if (cf)
			{
				const char c1 = *(reinterpret_cast<const char*>(p1)+pos);
				const char c2 = *(p2 + pos);
				return sse_chr_compare(c1, c2);
			}
			if (zf)
				return 0;
			++p1;
			p2 += 16;
		}
		return 0;//dummy
	}
	//SSE版strcmp:非16バイトアランイメント＋非16バイトアランイメント時
	static int strcmp_sse_case3(const char* str1, const char* str2)
	{
		static const int flags = _SIDD_SBYTE_OPS | _SIDD_CMP_EQUAL_EACH | _SIDD_NEGATIVE_POLARITY | _SIDD_LEAST_SIGNIFICANT;
		const char* p1 = str1;
		const char* p2 = str2;
		while (true)
		{
			const __m128i str16_1 = _mm_loadu_si128(reinterpret_cast<const __m128i*>(p1));
			const __m128i str16_2 = _mm_loadu_si128(reinterpret_cast<const __m128i*>(p2));
			const int         cf  = _mm_cmpistrc(str16_1, str16_2, flags);//※この三行は、コンパイル後は1回の cmpistri になる（はず）
			const int         zf  = _mm_cmpistrz(str16_1, str16_2, flags);
			const std::size_t pos = _mm_cmpistri(str16_1, str16_2, flags);
			if (cf)
			{
				const char c1 = *(reinterpret_cast<const char*>(p1)+pos);
				const char c2 = *(reinterpret_cast<const char*>(p2)+pos);
				return sse_chr_compare(c1, c2);
			}
			if (zf)
				return 0;
			p1 += 16;
			p2 += 16;
		}
		return 0;//dummy
	}
	//SSE版strcmp:関数テーブル
	typedef int(*strcmp_sse_t)(const char* str1, const char* str2);
	static const strcmp_sse_t strcmp_sse_table[] =
	{
		strcmp_sse_case0,
		strcmp_sse_case1,
		strcmp_sse_case2,
		strcmp_sse_case3
	};
}//namespace _private
//SSE版strcmp
int strcmp_sse(const char* str1, const char* str2)
{
//nullチェックしない
//	if (!str1) if (!str2) return 0;
//	           else       return -1;
//	else       if (!str2) return 1;
//	//         else       continue...
	return _private::strcmp_sse_table[_private::sse_str_pattern(str1, str2)](str1, str2);
}

//----------------------------------------
//SSE版strncmp
namespace _private
{
	//SSE版strncmp:16バイトアランイメント＋16バイトアランイメント時
	static int strncmp_sse_case0(const char* str1, const char* str2, const std::size_t max_len)
	{
		static const int flags = _SIDD_SBYTE_OPS | _SIDD_CMP_EQUAL_EACH | _SIDD_NEGATIVE_POLARITY | _SIDD_LEAST_SIGNIFICANT;
		//const __m128i null = _mm_setzero_si128();
		const __m128i* p1 = reinterpret_cast<const __m128i*>(str1);
		const __m128i* p2 = reinterpret_cast<const __m128i*>(str2);
		const char* p1_end = str1 + max_len;
		const char* p1_end_16 = p1_end - 16;
		while (reinterpret_cast<const char*>(p1) <= p1_end_16)
		{
			const __m128i str16_1 = _mm_load_si128(p1);
			const __m128i str16_2 = _mm_load_si128(p2);
			const int         cf  = _mm_cmpistrc(str16_1, str16_2, flags);//※この三行は、コンパイル後は1回の cmpistri になる（はず）
			const int         zf  = _mm_cmpistrz(str16_1, str16_2, flags);
			const std::size_t pos = _mm_cmpistri(str16_1, str16_2, flags);
			if (cf)
			{
				const char c1 = *(reinterpret_cast<const char*>(p1)+pos);
				const char c2 = *(reinterpret_cast<const char*>(p2)+pos);
				return sse_chr_compare(c1, c2);
			}
			if (zf)
				return 0;
			++p1;
			++p2;
		}
		{
			const std::size_t remain = p1_end - reinterpret_cast<const char*>(p1);
			if (remain > 0)
			{
				const __m128i str16_1 = _mm_load_si128(p1);
				const __m128i str16_2 = _mm_load_si128(p2);
				const int         cf  = _mm_cmpistrc(str16_1, str16_2, flags);//※この三行は、コンパイル後は1回の cmpistri になる（はず）
				//const int       zf  = _mm_cmpistrz(str16_1, str16_2, flags);
				const std::size_t pos = _mm_cmpistri(str16_1, str16_2, flags);
				if (cf)
				{
					if (pos >= remain)
						return 0;
					const char c1 = *(reinterpret_cast<const char*>(p1)+pos);
					const char c2 = *(reinterpret_cast<const char*>(p2)+pos);
					return sse_chr_compare(c1, c2);
				}
				//if (zf)
				//	return 0;
			}
		}
		return 0;
	}
	//SSE版strncmp:非16バイトアランイメント＋16バイトアランイメント時
	static int strncmp_sse_case1(const char* str1, const char* str2, const std::size_t max_len)
	{
		static const int flags = _SIDD_SBYTE_OPS | _SIDD_CMP_EQUAL_EACH | _SIDD_NEGATIVE_POLARITY | _SIDD_LEAST_SIGNIFICANT;
		//const __m128i null = _mm_setzero_si128();
		const char* p1 = str1;
		const __m128i* p2 = reinterpret_cast<const __m128i*>(str2);
		const char* p1_end = str1 + max_len;
		const char* p1_end_16 = p1_end - 16;
		while (p1 <= p1_end_16)
		{
			const __m128i str16_1 = _mm_loadu_si128(reinterpret_cast<const __m128i*>(p1));
			const __m128i str16_2 = _mm_load_si128(p2);
			const int         cf  = _mm_cmpistrc(str16_1, str16_2, flags);//※この三行は、コンパイル後は1回の cmpistri になる（はず）
			const int         zf  = _mm_cmpistrz(str16_1, str16_2, flags);
			const std::size_t pos = _mm_cmpistri(str16_1, str16_2, flags);
			if (cf)
			{
				const char c1 = *(p1 + pos);
				const char c2 = *(reinterpret_cast<const char*>(p2)+pos);
				return sse_chr_compare(c1, c2);
			}
			if (zf)
				return 0;
			p1 += 16;
			++p2;
		}
		{
			const std::size_t remain = p1_end - p1;
			if (remain > 0)
			{
				const __m128i str16_1 = _mm_loadu_si128(reinterpret_cast<const __m128i*>(p1));
				const __m128i str16_2 = _mm_load_si128(p2);
				const int         cf  = _mm_cmpistrc(str16_1, str16_2, flags);//※この三行は、コンパイル後は1回の cmpistri になる（はず）
				//const int       zf  = _mm_cmpistrz(str16_1, str16_2, flags);
				const std::size_t pos = _mm_cmpistri(str16_1, str16_2, flags);
				if (cf)
				{
					if (pos >= remain)
						return 0;
					const char c1 = *(p1 + pos);
					const char c2 = *(reinterpret_cast<const char*>(p2)+pos);
					return sse_chr_compare(c1, c2);
				}
				//if (zf)
				//	return 0;
			}
		}
		return 0;
	}
	//SSE版strncmp:16バイトアランイメント＋非16バイトアランイメント時
	static int strncmp_sse_case2(const char* str1, const char* str2, const std::size_t max_len)
	{
		static const int flags = _SIDD_SBYTE_OPS | _SIDD_CMP_EQUAL_EACH | _SIDD_NEGATIVE_POLARITY | _SIDD_LEAST_SIGNIFICANT;
		//const __m128i null = _mm_setzero_si128();
		const __m128i* p1 = reinterpret_cast<const __m128i*>(str1);
		const char* p2 = str2;
		const char* p1_end = str1 + max_len;
		const char* p1_end_16 = p1_end - 16;
		while (reinterpret_cast<const char*>(p1) <= p1_end_16)
		{
			const __m128i str16_1 = _mm_load_si128(p1);
			const __m128i str16_2 = _mm_loadu_si128(reinterpret_cast<const __m128i*>(p2));
			const int         cf  = _mm_cmpistrc(str16_1, str16_2, flags);//※この三行は、コンパイル後は1回の cmpistri になる（はず）
			const int         zf  = _mm_cmpistrz(str16_1, str16_2, flags);
			const std::size_t pos = _mm_cmpistri(str16_1, str16_2, flags);
			if (cf)
			{
				const char c1 = *(reinterpret_cast<const char*>(p1)+pos);
				const char c2 = *(p2 + pos);
				return sse_chr_compare(c1, c2);
			}
			if (zf)
				return 0;
			++p1;
			p2 += 16;
		}
		{
			const std::size_t remain = p1_end - reinterpret_cast<const char*>(p1);
			if (remain > 0)
			{
				const __m128i str16_1 = _mm_load_si128(p1);
				const __m128i str16_2 = _mm_loadu_si128(reinterpret_cast<const __m128i*>(p2));
				const int         cf  = _mm_cmpistrc(str16_1, str16_2, flags);//※この三行は、コンパイル後は1回の cmpistri になる（はず）
				//const int       zf  = _mm_cmpistrz(str16_1, str16_2, flags);
				const std::size_t pos = _mm_cmpistri(str16_1, str16_2, flags);
				if (cf)
				{
					if (pos >= remain)
						return 0;
					const char c1 = *(reinterpret_cast<const char*>(p1)+pos);
					const char c2 = *(p2 + pos);
					return sse_chr_compare(c1, c2);
				}
				//if (zf)
				//	return 0;
			}
		}
		return 0;
	}
	//SSE版strncmp:非16バイトアランイメント＋非16バイトアランイメント時
	static int strncmp_sse_case3(const char* str1, const char* str2, const std::size_t max_len)
	{
		static const int flags = _SIDD_SBYTE_OPS | _SIDD_CMP_EQUAL_EACH | _SIDD_NEGATIVE_POLARITY | _SIDD_LEAST_SIGNIFICANT;
		//const __m128i null = _mm_setzero_si128();
		const char* p1 = str1;
		const char* p2 = str2;
		const char* p1_end = str1 + max_len;
		const char* p1_end_16 = p1_end - 16;
		while (p1 <= p1_end_16)
		{
			const __m128i str16_1 = _mm_loadu_si128(reinterpret_cast<const __m128i*>(p1));
			const __m128i str16_2 = _mm_loadu_si128(reinterpret_cast<const __m128i*>(p2));
			const int         cf  = _mm_cmpistrc(str16_1, str16_2, flags);//※この三行は、コンパイル後は1回の cmpistri になる（はず）
			const int         zf  = _mm_cmpistrz(str16_1, str16_2, flags);
			const std::size_t pos = _mm_cmpistri(str16_1, str16_2, flags);
			if (cf)
			{
				const char c1 = *(p1 + pos);
				const char c2 = *(p2 + pos);
				return sse_chr_compare(c1, c2);
			}
			if (zf)
				return 0;
			p1 += 16;
			p2 += 16;
		}
		{
			const std::size_t remain = p1_end - p1;
			if (remain > 0)
			{
				const __m128i str16_1 = _mm_loadu_si128(reinterpret_cast<const __m128i*>(p1));
				const __m128i str16_2 = _mm_loadu_si128(reinterpret_cast<const __m128i*>(p2));
				const int         cf  = _mm_cmpistrc(str16_1, str16_2, flags);//※この三行は、コンパイル後は1回の cmpistri になる（はず）
				//const int       zf  = _mm_cmpistrz(str16_1, str16_2, flags);
				const std::size_t pos = _mm_cmpistri(str16_1, str16_2, flags);
				if (cf)
				{
					if (pos >= remain)
						return 0;
					const char c1 = *(p1 + pos);
					const char c2 = *(p2 + pos);
					return sse_chr_compare(c1, c2);
				}
				//if (zf)
				//	return 0;
			}
		}
		return 0;
	}
	//SSE版strncmp:関数テーブル
	typedef int(*strncmp_sse_t)(const char* str1, const char* str2, const std::size_t max_len);
	static const strncmp_sse_t strncmp_sse_table[] =
	{
		strncmp_sse_case0,
		strncmp_sse_case1,
		strncmp_sse_case2,
		strncmp_sse_case3
	};
}//namespace _private
//SSE版strncmp
int strncmp_sse(const char* str1, const char* str2, const std::size_t max_len)
{
//nullチェックしない
//	if (!str1) if (!str2) return 0;
//	           else       return -1;
//	else       if (!str2) return 1;
//	//         else       continue...
	return _private::strncmp_sse_table[_private::sse_str_pattern(str1, str2)](str1, str2, max_len);
}

//----------------------------------------
//SSE版strchr
const char* strchr_sse(const char* str, const char c)
{
//nullチェックしない
//	if (!str)
//		return 0;
	static const int flags = _SIDD_SBYTE_OPS | _SIDD_CMP_EQUAL_EACH | _SIDD_POSITIVE_POLARITY | _SIDD_LEAST_SIGNIFICANT;
	const __m128i c16 = _mm_set1_epi8(c);
	const char* p = str;
	const std::size_t str_over = reinterpret_cast<intptr_t>(str)& 0xf;
	if (str_over != 0)
	{
		//非16バイトアランイメント時
		const __m128i str16 = _mm_loadu_si128(reinterpret_cast<const __m128i*>(p));
		const int         cf  = _mm_cmpistrc(c16, str16, flags);//※この三行は、コンパイル後は1回の cmpistri になる（はず）
		const int         zf  = _mm_cmpistrz(c16, str16, flags);
		const std::size_t pos = _mm_cmpistri(c16, str16, flags);
		if (cf)
			return reinterpret_cast<const char*>(p)+pos;
		if (zf)
			return nullptr;
		p += (16 - str_over);
	}
	//16バイトアランイメント時
	const __m128i* p128 = reinterpret_cast<const __m128i*>(p);
	while (true)
	{
		const __m128i str16 = _mm_load_si128(p128);
		const int         cf  = _mm_cmpistrc(c16, str16, flags);//※この三行は、コンパイル後は1回の cmpistri になる（はず）
		const int         zf  = _mm_cmpistrz(c16, str16, flags);
		const std::size_t pos = _mm_cmpistri(c16, str16, flags);
		if (cf)
			return reinterpret_cast<const char*>(p128)+pos;
		if (zf)
			return nullptr;
		++p128;
	}
	return nullptr;//dummy
}

//----------------------------------------
//SSE版strrchr
const char* strrchr_sse(const char* str, const char c)
{
//nullチェックしない
//	if (!str)
//		return 0;
	const char* found_p = nullptr;
	static const int flags = _SIDD_SBYTE_OPS | _SIDD_CMP_EQUAL_EACH | _SIDD_POSITIVE_POLARITY | _SIDD_MOST_SIGNIFICANT;
	const __m128i c16 = _mm_set1_epi8(c);
	const char* p = str;
	const std::size_t str_over = reinterpret_cast<intptr_t>(str)& 0xf;
	if (str_over != 0)
	{
		//非16バイトアランイメント時
		const __m128i str16 = _mm_loadu_si128(reinterpret_cast<const __m128i*>(p));
		const int         cf  = _mm_cmpistrc(c16, str16, flags);//※この三行は、コンパイル後は1回の cmpistri になる（はず）
		const int         zf  = _mm_cmpistrz(c16, str16, flags);
		const std::size_t pos = _mm_cmpistri(c16, str16, flags);
		if (cf)
			found_p = p + pos;
		if (zf)
			return found_p;
		p += (16 - str_over);
	}
	//16バイトアランイメント時
	const __m128i* p128 = reinterpret_cast<const __m128i*>(p);
	while (true)
	{
		const __m128i str16 = _mm_load_si128(p128);
		const int         cf  = _mm_cmpistrc(c16, str16, flags);//※この三行は、コンパイル後は1回の cmpistri になる（はず）
		const int         zf  = _mm_cmpistrz(c16, str16, flags);
		const std::size_t pos = _mm_cmpistri(c16, str16, flags);
		if (cf)
			found_p = reinterpret_cast<const char*>(p128)+pos;
		if (zf)
			return found_p;
		++p128;
	}
	return nullptr;//dummy
}

//----------------------------------------
//SSE版strstr
namespace _private
{
	//SSE版strstr用補助関数:パターンが2文字専用
	static const char* strstr_sse_2chr(const char* str, const char* pattern)
	{
		//	if (*str == '\0' || *(str + 1) == '\0')//←この判定は呼び出し元で行っている
		//		return nullptr;
		const char pattern_top_c = *pattern;//先頭の文字
		const char pattern_2nd_c = *(pattern + 1);//2文字目の文字
		const __m128i pattern_top_c16 = _mm_set1_epi8(pattern_top_c);
		static const int flags = _SIDD_SBYTE_OPS | _SIDD_CMP_EQUAL_EACH | _SIDD_POSITIVE_POLARITY | _SIDD_LEAST_SIGNIFICANT;
		const char* p = str;
		const std::size_t str_over = reinterpret_cast<intptr_t>(p)& 0xf;
		if (str_over != 0)
		{
			//非16バイトアランイメント時
			const __m128i str16 = _mm_loadu_si128(reinterpret_cast<const __m128i*>(p));
			const int       zf   = _mm_cmpistrz(pattern_top_c16, str16, flags);//※この二行は、コンパイル後は1回の cmpistri になる（はず）
			const int       cf   = _mm_cmpistrc(pattern_top_c16, str16, flags);
			//const __m128i mask = _mm_cmpistrm(pattern_top_c16, str16, flags);//※この命令もCF,ZFを更新するので一まとめになることを期待したが、VC++2013の確認でそうならなかったので、次のifブロック内に移す
			if (cf)
			{
				const m128i_cast mask(_mm_cmpistrm(pattern_top_c16, str16, flags));
				std::uint32_t found_bits = mask.m_u32[0];
				const char* _p = p;
				while (found_bits)
				{
					if ((found_bits & 1) && *_p == pattern_top_c && *(_p + 1) == pattern_2nd_c)
						return _p;
					found_bits >>= 1;
					++_p;
				}
			}
			if (zf)
				return nullptr;
			p += (16 - str_over);
		}
		//16バイトアランイメント時
		const __m128i* p128 = reinterpret_cast<const __m128i*>(p);
		while (true)
		{
			const __m128i str16 = _mm_load_si128(p128);
			const int       zf   = _mm_cmpistrz(pattern_top_c16, str16, flags);//※この二行は、コンパイル後は1回の cmpistri になる（はず）
			const int       cf   = _mm_cmpistrc(pattern_top_c16, str16, flags);
			//const __m128i mask = _mm_cmpistrm(pattern_top_c16, str16, flags);//※この命令もCF,ZFを更新するので一まとめになることを期待したが、VC++2013の確認でそうならなかったので、次のifブロック内に移す
			if (cf)
			{
				const m128i_cast mask(_mm_cmpistrm(pattern_top_c16, str16, flags));
				std::uint32_t found_bits = mask.m_u32[0];
				const char* _p = reinterpret_cast<const char*>(p128);
				while (found_bits)
				{
					if ((found_bits & 1) && *_p == pattern_top_c && *(_p + 1) == pattern_2nd_c)
						return _p;
					found_bits >>= 1;
					++_p;
				}
			}
			if (zf)
				return nullptr;
			++p128;
		}
		return nullptr;//dummy
	}
	//SSE版strstr補助関数:16バイトアランイメント＋16バイトアランイメント時
	//戻り値: 0 ... 不一致, 1 ... 一致, 2 ... strの方が短い
	static int strstr_sse_strcmp_case0(const char* str, const char* pattern)
	{
		static const int flags = _SIDD_SBYTE_OPS | _SIDD_CMP_EQUAL_EACH | _SIDD_NEGATIVE_POLARITY | _SIDD_LEAST_SIGNIFICANT;
		const __m128i* p1 = reinterpret_cast<const __m128i*>(str);
		const __m128i* p2 = reinterpret_cast<const __m128i*>(pattern);
		while (true)
		{
			const __m128i str16_1 = _mm_load_si128(p1);
			const __m128i str16_2 = _mm_load_si128(p2);
			const int         cf  = _mm_cmpistrc(str16_1, str16_2, flags);//※この三行は、コンパイル後は1回の cmpistri になる（はず）
			const int         zf  = _mm_cmpistrz(str16_1, str16_2, flags);
			const std::size_t pos = _mm_cmpistri(str16_1, str16_2, flags);
			if (cf)
			{
				const char c1 = *(reinterpret_cast<const char*>(p1)+pos);
				const char c2 = *(reinterpret_cast<const char*>(p2)+pos);
				return (c2 == '\0') | ((c1 == '\0') << 1);
			}
			if (zf)
				return 1;
			++p1;
			++p2;
		}
		return 0;//dummy
	}
	//SSE版strstr補助関数:非16バイトアランイメント＋16バイトアランイメント時
	//戻り値: 0 ... 不一致, 1 ... 一致, 2 ... strの方が短い
	static int strstr_sse_strcmp_case1(const char* str, const char* pattern)
	{
		static const int flags = _SIDD_SBYTE_OPS | _SIDD_CMP_EQUAL_EACH | _SIDD_NEGATIVE_POLARITY | _SIDD_LEAST_SIGNIFICANT;
		const char* p1 = str;
		const __m128i* p2 = reinterpret_cast<const __m128i*>(pattern);
		while (true)
		{
			const __m128i str16_1 = _mm_loadu_si128(reinterpret_cast<const __m128i*>(p1));
			const __m128i str16_2 = _mm_load_si128(p2);
			const int         cf  = _mm_cmpistrc(str16_1, str16_2, flags);//※この三行は、コンパイル後は1回の cmpistri になる（はず）
			const int         zf  = _mm_cmpistrz(str16_1, str16_2, flags);
			const std::size_t pos = _mm_cmpistri(str16_1, str16_2, flags);
			if (cf)
			{
				const char c1 = *(p1 + pos);
				const char c2 = *(reinterpret_cast<const char*>(p2)+pos);
				return (c2 == '\0') | ((c1 == '\0') << 1);
			}
			if (zf)
				return 1;
			p1 += 16;
			++p2;
		}
		return 0;//dummy
	}
	//SSE版strstr補助関数:16バイトアランイメント＋非16バイトアランイメント時
	//戻り値: 0 ... 不一致, 1 ... 一致, 2 ... strの方が短い
	static int strstr_sse_strcmp_case2(const char* str, const char* pattern)
	{
		static const int flags = _SIDD_SBYTE_OPS | _SIDD_CMP_EQUAL_EACH | _SIDD_NEGATIVE_POLARITY | _SIDD_LEAST_SIGNIFICANT;
		const __m128i* p1 = reinterpret_cast<const __m128i*>(str);
		const char* p2 = pattern;
		while (true)
		{
			const __m128i str16_1 = _mm_load_si128(p1);
			const __m128i str16_2 = _mm_loadu_si128(reinterpret_cast<const __m128i*>(p2));
			const int         cf  = _mm_cmpistrc(str16_1, str16_2, flags);//※この三行は、コンパイル後は1回の cmpistri になる（はず）
			const int         zf  = _mm_cmpistrz(str16_1, str16_2, flags);
			const std::size_t pos = _mm_cmpistri(str16_1, str16_2, flags);
			if (cf)
			{
				const char c1 = *(reinterpret_cast<const char*>(p1)+pos);
				const char c2 = *(p2 + pos);
				return (c2 == '\0') | ((c1 == '\0') << 1);
			}
			if (zf)
				return 1;
			++p1;
			p2 += 16;
		}
		return 0;//dummy
	}
	//SSE版strstr補助関数:非16バイトアランイメント＋非16バイトアランイメント時
	//戻り値: 0 ... 不一致, 1 ... 一致, 2 ... strの方が短い
	static int strstr_sse_strcmp_case3(const char* str, const char* pattern)
	{
		static const int flags = _SIDD_SBYTE_OPS | _SIDD_CMP_EQUAL_EACH | _SIDD_NEGATIVE_POLARITY | _SIDD_LEAST_SIGNIFICANT;
		const char* p1 = str;
		const char* p2 = pattern;
		while (true)
		{
			const __m128i str16_1 = _mm_loadu_si128(reinterpret_cast<const __m128i*>(p1));
			const __m128i str16_2 = _mm_loadu_si128(reinterpret_cast<const __m128i*>(p2));
			const int         cf  = _mm_cmpistrc(str16_1, str16_2, flags);//※この三行は、コンパイル後は1回の cmpistri になる（はず）
			const int         zf  = _mm_cmpistrz(str16_1, str16_2, flags);
			const std::size_t pos = _mm_cmpistri(str16_1, str16_2, flags);
			if (cf)
			{
				const char c1 = *(reinterpret_cast<const char*>(p1)+pos);
				const char c2 = *(reinterpret_cast<const char*>(p2)+pos);
				return (c2 == '\0') | ((c1 == '\0') << 1);
			}
			if (zf)
				return 1;
			p1 += 16;
			p2 += 16;
		}
		return 0;//dummy
	}
	//SSE版strstr:補助関数テーブル
	typedef int(*strstr_sse_strcmp_t)(const char* str, const char* pattern);
	static const strstr_sse_strcmp_t strstr_sse_strcmp_table[] =
	{
		strstr_sse_strcmp_case0,
		strstr_sse_strcmp_case1,
		strstr_sse_strcmp_case2,
		strstr_sse_strcmp_case3
	};
	//SSE版strstr補助関数
	//戻り値: 0 ... 不一致, 1 ... 一致, 2 ... strの方が短い
	inline static int strstr_sse_strcmp(const char* str, const char* pattern)
	{
		return strstr_sse_strcmp_table[sse_str_pattern(str, pattern)](str, pattern);
	}
}//namespace _private
//SSE版strstr
const char* strstr_sse(const char* str, const char* pattern)
{
//nullチェックしない
//	if (!str || !pattern)
//		return 0;
#if 0//strchr_sse()とstrncmp_sse()を活用した単純な処理（遅いのでNG）
	const std::size_t pattern_len = strlen_sse(pattern);
	const char pattern_c = *pattern;
	const char* p = str;
	while (true)
	{
		const char* found = strchr_sse(p, pattern_c);
		if (!found)
			return nullptr;
		if (strncmp_sse(found, pattern, pattern_len) == 0)
			return found;
		p = found + 1;
	}
	return nullptr;//dummy
#else//SSE命令使用 ※このやり方がベストだが、VC++のstrstrよりもはるかに遅い（GCCのstrstrよりは速い）
	//patternの長さに基づいて、処理を振り分ける
	if (*pattern == '\0')//パターンが0文字の時
		return str;
	if (*(pattern + 1) == '\0')//パターンが1文字の時
	{
		if (*str == '\0')
			return nullptr;
		return strchr(str, *pattern);
	}
	if (*(pattern + 2) == '\0')//パターンが2文字の時
	{
		if (*str == '\0' || *(str + 1) == '\0')
			return nullptr;
		return _private::strstr_sse_2chr(str, pattern);
	}
	const char pattern_top_c = *pattern;//先頭の文字
	const char pattern_2nd_c = *(pattern + 1);//2文字目の文字
	const __m128i pattern_top_c16 = _mm_set1_epi8(pattern_top_c);
	static const int flags = _SIDD_SBYTE_OPS | _SIDD_CMP_EQUAL_EACH | _SIDD_POSITIVE_POLARITY | _SIDD_LEAST_SIGNIFICANT;
	const char* p = str;
	const std::size_t str_over = reinterpret_cast<intptr_t>(p)& 0xf;
	if (str_over != 0)
	{
		//非16バイトアランイメント時
		const __m128i str16 = _mm_loadu_si128(reinterpret_cast<const __m128i*>(p));
		const int       zf   = _mm_cmpistrz(pattern_top_c16, str16, flags);//※この二行は、コンパイル後は1回の cmpistri になる（はず）
		const int       cf   = _mm_cmpistrc(pattern_top_c16, str16, flags);
		//const __m128i mask = _mm_cmpistrm(pattern_top_c16, str16, flags);//※この命令もCF,ZFを更新するので一まとめになることを期待したが、VC++2013の確認でそうならなかったので、次のifブロック内に移す
		if (cf)
		{
			const m128i_cast mask(_mm_cmpistrm(pattern_top_c16, str16, flags));
			std::uint32_t found_bits = mask.m_u32[0];
			const char* _p = p;
			while (found_bits)
			{
				if (found_bits & 1)
				{
					if (*_p == pattern_top_c && *(_p + 1) == pattern_2nd_c)
					{
						const int cmp = _private::strstr_sse_strcmp(_p, pattern);
						if (cmp != 0)
						{
							if (cmp == 1)
								return _p;
							else
								return nullptr;
						}
					}
				}
				found_bits >>= 1;
				++_p;
			}
		}
		if (zf)
			return nullptr;
		p += (16 - str_over);
	}
	//16バイトアランイメント時
	const __m128i* p128 = reinterpret_cast<const __m128i*>(p);
	while (true)
	{
		const __m128i str16 = _mm_load_si128(p128);
		const int       zf   = _mm_cmpistrz(pattern_top_c16, str16, flags);//※この二行は、コンパイル後は1回の cmpistri になる（はず）
		const int       cf   = _mm_cmpistrc(pattern_top_c16, str16, flags);
		//const __m128i mask = _mm_cmpistrm(pattern_top_c16, str16, flags);//※この命令もCF,ZFを更新するので一まとめになることを期待したが、VC++2013の確認でそうならなかったので、次のifブロック内に移す
		if (cf)
		{
			const m128i_cast mask(_mm_cmpistrm(pattern_top_c16, str16, flags));
			std::uint32_t found_bits = mask.m_u32[0];
			const char* _p = reinterpret_cast<const char*>(p128);
			while (found_bits)
			{
				if (found_bits & 1)
				{
					if (*_p == pattern_top_c && *(_p + 1) == pattern_2nd_c)
					{
						const int cmp = _private::strstr_sse_strcmp(_p, pattern);
						if (cmp != 0)
						{
							if (cmp == 1)
								return _p;
							else
								return nullptr;
						}
					}
				}
				found_bits >>= 1;
				++_p;
			}
		}
		if (zf)
			return nullptr;
		++p128;
	}
	return nullptr;//dummy
#endif
}

//----------------------------------------
//SSE版strstr(BM法)
namespace _private
{
	//SSE版strstr(BM法)用補助関数:パターンが1文字専用
	const char* strstrbm1_sse(const char* str, const char pattern, std::function<bool(const char*, const char*)> found_it)
	{
		//検索開始
		const __m128i pattern_c16 = _mm_set1_epi8(pattern);
		static const int flags = _SIDD_SBYTE_OPS | _SIDD_CMP_EQUAL_EACH | _SIDD_POSITIVE_POLARITY | _SIDD_LEAST_SIGNIFICANT;
		const char* str_p = str;
		while (true)
		{
			const __m128i str16 = _mm_loadu_si128(reinterpret_cast<const __m128i*>(str_p));
			const int       zf   = _mm_cmpistrz(pattern_c16, str16, flags);//※この二行は、コンパイル後は1回の cmpistri になる（はず）
			const int       cf   = _mm_cmpistrc(pattern_c16, str16, flags);
			//const __m128i mask = _mm_cmpistrm(pattern_c16, str16, flags);//※この命令もCF,ZFを更新するので一まとめになることを期待したが、VC++2013の確認でそうならなかったので、次のifブロック内に移す
			if (cf)
			{
				//パターンの末尾の文字が一致（16文字まとめて照合） ... パターンを照合する
				const m128i_cast mask(_mm_cmpistrm(pattern_c16, str16, flags));
				std::uint32_t found_bits = mask.m_u32[0];
				while (found_bits)
				{
					if (found_bits & 1)
					{
						if (found_it(str_p, str))
							return str_p;
					}
					found_bits >>= 1;
					++str_p;
				}
				if (zf)
					return nullptr;
			}
			else
			{
				if (zf)
					return nullptr;
				//パターンの文字が不一致 ... パターンの中に次の文字が見つかる位置まで移動
				++str_p;
			}
		}
		return nullptr;
	}
}
//SSE版strstr(BM法)
const char* strstrbm_sse(const char* str, const char* pattern, std::function<bool(const char*, const char*)> found_it)
{
//nullチェックしない
//	if (!str || !pattern)
//		return 0;
	if (*pattern == '\0')//パターンが0文字の時
		return str;
	if (*(pattern + 1) == '\0')//パターンが1文字の時
	{
		if (*str == '\0')
			return nullptr;
		return _private::strstrbm1_sse(str, *pattern, found_it);
	}
	const std::size_t pattern_len = strlen_sse(pattern);
	const std::size_t str_len = strnlen_sse(str, pattern_len);
	if (str_len < pattern_len)
		return nullptr;
	//パターン内の文字ごとに、照合失敗時のスキップ文字数を記録
	int skip[256];
	for (std::size_t i = 0; i < 256; ++i)
		skip[i] = static_cast<int>(pattern_len);
	for (std::size_t i = 0; i < pattern_len; ++i)
		skip[static_cast<int>(pattern[i])] = static_cast<int>(pattern_len) - static_cast<int>(i) - 1;
	//検索開始
	const std::size_t pattern_term = pattern_len - 1;
	const char* pattern_term_p = pattern + pattern_term;
	const char* pattern_term_1_p = pattern_term_p - 1;
	const char pattern_term_c = *pattern_term_p;
	const __m128i pattern_term_c16 = _mm_set1_epi8(pattern_term_c);
	static const int flags = _SIDD_SBYTE_OPS | _SIDD_CMP_EQUAL_EACH | _SIDD_POSITIVE_POLARITY | _SIDD_LEAST_SIGNIFICANT;
	const char* str_p = str + pattern_len - 1;
	while (true)
	{
		const __m128i str16 = _mm_loadu_si128(reinterpret_cast<const __m128i*>(str_p));
		const int       zf   = _mm_cmpistrz(pattern_term_c16, str16, flags);//※この二行は、コンパイル後は1回の cmpistri になる（はず）
		const int       cf   = _mm_cmpistrc(pattern_term_c16, str16, flags);
		//const __m128i mask = _mm_cmpistrm(pattern_term_c16, str16, flags);//※この命令もCF,ZFを更新するので一まとめになることを期待したが、VC++2013の確認でそうならなかったので、次のifブロック内に移す
		if (cf)
		{
			//パターンの末尾の文字が一致（16文字まとめて照合） ... パターンを照合する
			const m128i_cast mask(_mm_cmpistrm(pattern_term_c16, str16, flags));
			std::uint32_t found_bits = mask.m_u32[0];
			while (found_bits)
			{
				if (found_bits & 1)
				{
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
					found_bits >>= 1;
					const int _skip = skip[static_cast<unsigned char>(_str_c)];
					str_p += _skip;
					found_bits >>= _skip;
				}
				else
				{
					found_bits >>= 1;
					++str_p;
				}
			}
			if (zf)
				return nullptr;
		}
		else
		{
			if (zf)
				return nullptr;
			//パターンの末尾の文字が不一致 ... パターンの中に次の文字が見つかる位置まで移動
			const char str_c = *(str_p += 16);
			const int _skip = skip[static_cast<unsigned char>(str_c)];
			str_p += _skip;
		}
	}
	return nullptr;
}

//----------------------------------------
//SSE版strstr0
const char* strstr0_sse(const char* str, const char* pattern)
{
	const int ret = _private::strstr_sse_strcmp(str, pattern);
	if (ret == 1)
		return str;
	return nullptr;
}

//----------------------------------------
//SSE版strcpy
namespace _private
{
	//SSE版strcpy用補助関数
	#define GASHA_SSE_MEMCPY_TYPE 2// 0 ... memcpy() 使用
	                               // 1 ... コピーサイズとアラインメントごとの関数
	                               // 2 ... コピーサイズごとの関数（アラインメント無視／16バイトの時だけアランイメントで振り分け）
	                               // 3 ... _mm_maskmoveu_si128() 使用
	//SSE版strcpy用補助関数:_m128iからメモリへのコピー用関数
	//【OK】コピーサイズごとの関数を用意するやり方は、memcpy()を使うよりも速かった
	static void sse_memcpy_00_a(char* dst, const __m128i src)
	{
		//なにもしない
	}
#if GASHA_SSE_MEMCPY_TYPE == 1
	static void sse_memcpy_00_u(char* dst, const __m128i src)
	{
		//なにもしない
	}
#endif//GASHA_SSE_MEMCPY_TYPE == 1
	static void sse_memcpy_01_a(char* dst, const __m128i src)
	{
		m128i_cast _src(src);
		std::uint8_t* dst_p8 = reinterpret_cast<std::uint8_t*>(dst);
		*dst_p8 = _src.m_u8[0];
	}
#if GASHA_SSE_MEMCPY_TYPE == 1
	static void sse_memcpy_01_u(char* dst, const __m128i src)
	{
		m128i_cast _src(src);
		std::uint8_t* dst_p8 = reinterpret_cast<std::uint8_t*>(dst);
		*dst_p8 = _src.m_u8[0];
	}
#endif//GASHA_SSE_MEMCPY_TYPE == 1
	static void sse_memcpy_02_a(char* dst, const __m128i src)
	{
		m128i_cast _src(src);
		std::uint16_t* dst_p16 = reinterpret_cast<std::uint16_t*>(dst);
		*dst_p16 = _src.m_u16[0];
	}
#if GASHA_SSE_MEMCPY_TYPE == 1
	static void sse_memcpy_02_u(char* dst, const __m128i src)
	{
		m128i_cast _src(src);
		std::uint8_t* dst_p8 = reinterpret_cast<std::uint8_t*>(dst);
		for(int i =0; i < 2; ++i)
			dst_p8[i] = _src.m_u8[i];
	}
#endif//GASHA_SSE_MEMCPY_TYPE == 1
	static void sse_memcpy_03_a(char* dst, const __m128i src)
	{
		m128i_cast _src(src);
		std::uint16_t* dst_p16 = reinterpret_cast<std::uint16_t*>(dst);
		*dst_p16 = _src.m_u16[0];
		std::uint8_t* dst_p8 = reinterpret_cast<std::uint8_t*>(++dst_p16);
		*dst_p8 = _src.m_u8[2];
	}
#if GASHA_SSE_MEMCPY_TYPE == 1
	static void sse_memcpy_03_u(char* dst, const __m128i src)
	{
		m128i_cast _src(src);
		std::uint8_t* dst_p8 = reinterpret_cast<std::uint8_t*>(dst);
		for(int i =0; i < 3; ++i)
			dst_p8[i] = _src.m_u8[i];
	}
#endif//GASHA_SSE_MEMCPY_TYPE == 1
	static void sse_memcpy_04_a(char* dst, const __m128i src)
	{
		m128i_cast _src(src);
		std::uint32_t* dst_p32 = reinterpret_cast<std::uint32_t*>(dst);
		*dst_p32 = _src.m_u32[0];
	}
#if GASHA_SSE_MEMCPY_TYPE == 1
	static void sse_memcpy_04_u(char* dst, const __m128i src)
	{
		m128i_cast _src(src);
		std::uint8_t* dst_p8 = reinterpret_cast<std::uint8_t*>(dst);
		for(int i =0; i < 4; ++i)
			dst_p8[i] = _src.m_u8[i];
	}
#endif//GASHA_SSE_MEMCPY_TYPE == 1
	static void sse_memcpy_05_a(char* dst, const __m128i src)
	{
		m128i_cast _src(src);
		std::uint32_t* dst_p32 = reinterpret_cast<std::uint32_t*>(dst);
		*dst_p32 = _src.m_u32[0];
		std::uint8_t* dst_p8 = reinterpret_cast<std::uint8_t*>(++dst_p32);
		*dst_p8 = _src.m_u8[4];
	}
#if GASHA_SSE_MEMCPY_TYPE == 1
	static void sse_memcpy_05_u(char* dst, const __m128i src)
	{
		m128i_cast _src(src);
		std::uint8_t* dst_p8 = reinterpret_cast<std::uint8_t*>(dst);
		for(int i =0; i < 5; ++i)
			dst_p8[i] = _src.m_u8[i];
	}
#endif//GASHA_SSE_MEMCPY_TYPE == 1
	static void sse_memcpy_06_a(char* dst, const __m128i src)
	{
		m128i_cast _src(src);
		std::uint32_t* dst_p32 = reinterpret_cast<std::uint32_t*>(dst);
		*dst_p32 = _src.m_u32[0];
		std::uint16_t* dst_p16 = reinterpret_cast<std::uint16_t*>(++dst_p32);
		*dst_p16 = _src.m_u16[2];
	}
#if GASHA_SSE_MEMCPY_TYPE == 1
	static void sse_memcpy_06_u(char* dst, const __m128i src)
	{
		m128i_cast _src(src);
		std::uint8_t* dst_p8 = reinterpret_cast<std::uint8_t*>(dst);
		for(int i =0; i < 6; ++i)
			dst_p8[i] = _src.m_u8[i];
	}
#endif//GASHA_SSE_MEMCPY_TYPE == 1
	static void sse_memcpy_07_a(char* dst, const __m128i src)
	{
		m128i_cast _src(src);
		std::uint32_t* dst_p32 = reinterpret_cast<std::uint32_t*>(dst);
		*dst_p32 = _src.m_u32[0];
		std::uint16_t* dst_p16 = reinterpret_cast<std::uint16_t*>(++dst_p32);
		*dst_p16 = _src.m_u16[2];
		std::uint8_t* dst_p8 = reinterpret_cast<std::uint8_t*>(++dst_p16);
		*dst_p8 = _src.m_u8[6];
	}
#if GASHA_SSE_MEMCPY_TYPE == 1
	static void sse_memcpy_07_u(char* dst, const __m128i src)
	{
		m128i_cast _src(src);
		std::uint8_t* dst_p8 = reinterpret_cast<std::uint8_t*>(dst);
		for (int i = 0; i < 7; ++i)
			dst_p8[i] = _src.m_u8[i];
	}
#endif//GASHA_SSE_MEMCPY_TYPE == 1
	static void sse_memcpy_08_a(char* dst, const __m128i src)
	{
		m128i_cast _src(src);
		std::uint64_t* dst_p64 = reinterpret_cast<std::uint64_t*>(dst);
		*dst_p64 = _src.m_u64[0];
	}
#if GASHA_SSE_MEMCPY_TYPE == 1
	static void sse_memcpy_08_u(char* dst, const __m128i src)
	{
		m128i_cast _src(src);
		std::uint8_t* dst_p8 = reinterpret_cast<std::uint8_t*>(dst);
		for (int i = 0; i < 8; ++i)
			dst_p8[i] = _src.m_u8[i];
	}
#endif//GASHA_SSE_MEMCPY_TYPE == 1
	static void sse_memcpy_09_a(char* dst, const __m128i src)
	{
		m128i_cast _src(src);
		std::uint64_t* dst_p64 = reinterpret_cast<std::uint64_t*>(dst);
		*dst_p64 = _src.m_u64[0];
		std::uint8_t* dst_p8 = reinterpret_cast<std::uint8_t*>(++dst_p64);
		*dst_p8 = _src.m_u8[8];
	}
#if GASHA_SSE_MEMCPY_TYPE == 1
	static void sse_memcpy_09_u(char* dst, const __m128i src)
	{
		m128i_cast _src(src);
		std::uint8_t* dst_p8 = reinterpret_cast<std::uint8_t*>(dst);
		for (int i = 0; i < 9; ++i)
			dst_p8[i] = _src.m_u8[i];
	}
#endif//GASHA_SSE_MEMCPY_TYPE == 1
	static void sse_memcpy_10_a(char* dst, const __m128i src)
	{
		m128i_cast _src(src);
		std::uint64_t* dst_p64 = reinterpret_cast<std::uint64_t*>(dst);
		*dst_p64 = _src.m_u64[0];
		std::uint16_t* dst_p16 = reinterpret_cast<std::uint16_t*>(++dst_p64);
		*dst_p16 = _src.m_u16[4];
	}
#if GASHA_SSE_MEMCPY_TYPE == 1
	static void sse_memcpy_10_u(char* dst, const __m128i src)
	{
		m128i_cast _src(src);
		std::uint8_t* dst_p8 = reinterpret_cast<std::uint8_t*>(dst);
		for (int i = 0; i < 10; ++i)
			dst_p8[i] = _src.m_u8[i];
	}
#endif//GASHA_SSE_MEMCPY_TYPE == 1
	static void sse_memcpy_11_a(char* dst, const __m128i src)
	{
		m128i_cast _src(src);
		std::uint64_t* dst_p64 = reinterpret_cast<std::uint64_t*>(dst);
		*dst_p64 = _src.m_u64[0];
		std::uint16_t* dst_p16 = reinterpret_cast<std::uint16_t*>(++dst_p64);
		*dst_p16 = _src.m_u16[4];
		std::uint8_t* dst_p8 = reinterpret_cast<std::uint8_t*>(++dst_p16);
		*dst_p8 = _src.m_u8[10];
	}
#if GASHA_SSE_MEMCPY_TYPE == 1
	static void sse_memcpy_11_u(char* dst, const __m128i src)
	{
		m128i_cast _src(src);
		std::uint8_t* dst_p8 = reinterpret_cast<std::uint8_t*>(dst);
		for (int i = 0; i < 11; ++i)
			dst_p8[i] = _src.m_u8[i];
	}
#endif//GASHA_SSE_MEMCPY_TYPE == 1
	static void sse_memcpy_12_a(char* dst, const __m128i src)
	{
		m128i_cast _src(src);
		std::uint64_t* dst_p64 = reinterpret_cast<std::uint64_t*>(dst);
		*dst_p64 = _src.m_u64[0];
		std::uint32_t* dst_p32 = reinterpret_cast<std::uint32_t*>(++dst_p64);
		*dst_p32 = _src.m_u32[2];
	}
#if GASHA_SSE_MEMCPY_TYPE == 1
	static void sse_memcpy_12_u(char* dst, const __m128i src)
	{
		m128i_cast _src(src);
		std::uint8_t* dst_p8 = reinterpret_cast<std::uint8_t*>(dst);
		for(int i =0; i < 12; ++i)
			dst_p8[i] = _src.m_u8[i];
	}
#endif//GASHA_SSE_MEMCPY_TYPE == 1
	static void sse_memcpy_13_a(char* dst, const __m128i src)
	{
		m128i_cast _src(src);
		std::uint64_t* dst_p64 = reinterpret_cast<std::uint64_t*>(dst);
		*dst_p64 = _src.m_u64[0];
		std::uint32_t* dst_p32 = reinterpret_cast<std::uint32_t*>(++dst_p64);
		*dst_p32 = _src.m_u32[2];
		std::uint8_t* dst_p8 = reinterpret_cast<std::uint8_t*>(++dst_p32);
		*dst_p8 = _src.m_u8[12];
	}
#if GASHA_SSE_MEMCPY_TYPE == 1
	static void sse_memcpy_13_u(char* dst, const __m128i src)
	{
		m128i_cast _src(src);
		std::uint8_t* dst_p8 = reinterpret_cast<std::uint8_t*>(dst);
		for (int i = 0; i < 13; ++i)
			dst_p8[i] = _src.m_u8[i];
	}
#endif//GASHA_SSE_MEMCPY_TYPE == 1
	static void sse_memcpy_14_a(char* dst, const __m128i src)
	{
		m128i_cast _src(src);
		std::uint64_t* dst_p64 = reinterpret_cast<std::uint64_t*>(dst);
		*dst_p64 = _src.m_u64[0];
		std::uint32_t* dst_p32 = reinterpret_cast<std::uint32_t*>(++dst_p64);
		*dst_p32 = _src.m_u32[2];
		std::uint16_t* dst_p16 = reinterpret_cast<std::uint16_t*>(++dst_p32);
		*dst_p16 = _src.m_u16[6];
	}
#if GASHA_SSE_MEMCPY_TYPE == 1
	static void sse_memcpy_14_u(char* dst, const __m128i src)
	{
		m128i_cast _src(src);
		std::uint8_t* dst_p8 = reinterpret_cast<std::uint8_t*>(dst);
		for(int i =0; i < 14; ++i)
			dst_p8[i] = _src.m_u8[i];
	}
#endif//GASHA_SSE_MEMCPY_TYPE == 1
	static void sse_memcpy_15_a(char* dst, const __m128i src)
	{
		m128i_cast _src(src);
		std::uint64_t* dst_p64 = reinterpret_cast<std::uint64_t*>(dst);
		*dst_p64 = _src.m_u64[0];
		std::uint32_t* dst_p32 = reinterpret_cast<std::uint32_t*>(++dst_p64);
		*dst_p32 = _src.m_u32[2];
		std::uint16_t* dst_p16 = reinterpret_cast<std::uint16_t*>(++dst_p32);
		*dst_p16 = _src.m_u16[6];
		std::uint8_t* dst_p8 = reinterpret_cast<std::uint8_t*>(++dst_p16);
		*dst_p8 = _src.m_u8[14];
	}
#if GASHA_SSE_MEMCPY_TYPE == 1
	static void sse_memcpy_15_u(char* dst, const __m128i src)
	{
		m128i_cast _src(src);
		std::uint8_t* dst_p8 = reinterpret_cast<std::uint8_t*>(dst);
		for(int i =0; i < 15; ++i)
			dst_p8[i] = _src.m_u8[i];
	}
#endif//GASHA_SSE_MEMCPY_TYPE == 1
	static void sse_memcpy_16_a(char* dst, const __m128i src)
	{
		_mm_store_si128(reinterpret_cast<__m128i*>(dst), src);
	}
	static void sse_memcpy_16_u(char* dst, const __m128i src)
	{
		_mm_storeu_si128(reinterpret_cast<__m128i*>(dst), src);
	}
	//SSE版strcpy用補助関数:_m128iからメモリへのコピー用関数テーブル
	typedef void(*sse_memcpy_t)(char* dst, const __m128i src);
	static const sse_memcpy_t sse_memcpy_a_table[17] =
	{
		sse_memcpy_00_a,
		sse_memcpy_01_a,
		sse_memcpy_02_a,
		sse_memcpy_03_a,
		sse_memcpy_04_a,
		sse_memcpy_05_a,
		sse_memcpy_06_a,
		sse_memcpy_07_a,
		sse_memcpy_08_a,
		sse_memcpy_09_a,
		sse_memcpy_10_a,
		sse_memcpy_11_a,
		sse_memcpy_12_a,
		sse_memcpy_13_a,
		sse_memcpy_14_a,
		sse_memcpy_15_a,
		sse_memcpy_16_a,
	};
#if GASHA_SSE_MEMCPY_TYPE == 1
	static const sse_memcpy_t sse_memcpy_u_table[17] =
	{
		sse_memcpy_00_u,
		sse_memcpy_01_u,
		sse_memcpy_02_u,
		sse_memcpy_03_u,
		sse_memcpy_04_u,
		sse_memcpy_05_u,
		sse_memcpy_06_u,
		sse_memcpy_07_u,
		sse_memcpy_08_u,
		sse_memcpy_09_u,
		sse_memcpy_10_u,
		sse_memcpy_11_u,
		sse_memcpy_12_u,
		sse_memcpy_13_u,
		sse_memcpy_14_u,
		sse_memcpy_15_u,
		sse_memcpy_16_u,
	};
#elif GASHA_SSE_MEMCPY_TYPE == 2
	static const sse_memcpy_t sse_memcpy_u_table[17] =
	{
		sse_memcpy_00_a,
		sse_memcpy_01_a,
		sse_memcpy_02_a,
		sse_memcpy_03_a,
		sse_memcpy_04_a,
		sse_memcpy_05_a,
		sse_memcpy_06_a,
		sse_memcpy_07_a,
		sse_memcpy_08_a,
		sse_memcpy_09_a,
		sse_memcpy_10_a,
		sse_memcpy_11_a,
		sse_memcpy_12_a,
		sse_memcpy_13_a,
		sse_memcpy_14_a,
		sse_memcpy_15_a,
		sse_memcpy_16_u,
	};
#elif GASHA_SSE_MEMCPY_TYPE == 3
	//SSE版strcpy用補助関数:_m128iからメモリへのコピー用定数：_mm_maskmoveu_si128()用
	//【NG】_mm_maskmoveu_si128()を使用したやり方は、memcpy()を使うよりも遅かった
	static const __m128i sse_memcpy_flags[17] =
	{
		_mm_set_epi8(0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u),
		_mm_set_epi8(0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x80u),
		_mm_set_epi8(0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x80u, 0x80u),
		_mm_set_epi8(0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x80u, 0x80u, 0x80u),
		_mm_set_epi8(0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x80u, 0x80u, 0x80u, 0x80u),
		_mm_set_epi8(0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x80u, 0x80u, 0x80u, 0x80u, 0x80u),
		_mm_set_epi8(0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x80u, 0x80u, 0x80u, 0x80u, 0x80u, 0x80u),
		_mm_set_epi8(0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x80u, 0x80u, 0x80u, 0x80u, 0x80u, 0x80u, 0x80u),
		_mm_set_epi8(0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x80u, 0x80u, 0x80u, 0x80u, 0x80u, 0x80u, 0x80u, 0x80u),
		_mm_set_epi8(0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x80u, 0x80u, 0x80u, 0x80u, 0x80u, 0x80u, 0x80u, 0x80u, 0x80u),
		_mm_set_epi8(0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x80u, 0x80u, 0x80u, 0x80u, 0x80u, 0x80u, 0x80u, 0x80u, 0x80u, 0x80u),
		_mm_set_epi8(0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x80u, 0x80u, 0x80u, 0x80u, 0x80u, 0x80u, 0x80u, 0x80u, 0x80u, 0x80u, 0x80u),
		_mm_set_epi8(0x00u, 0x00u, 0x00u, 0x00u, 0x80u, 0x80u, 0x80u, 0x80u, 0x80u, 0x80u, 0x80u, 0x80u, 0x80u, 0x80u, 0x80u, 0x80u),
		_mm_set_epi8(0x00u, 0x00u, 0x00u, 0x80u, 0x80u, 0x80u, 0x80u, 0x80u, 0x80u, 0x80u, 0x80u, 0x80u, 0x80u, 0x80u, 0x80u, 0x80u),
		_mm_set_epi8(0x00u, 0x00u, 0x80u, 0x80u, 0x80u, 0x80u, 0x80u, 0x80u, 0x80u, 0x80u, 0x80u, 0x80u, 0x80u, 0x80u, 0x80u, 0x80u),
		_mm_set_epi8(0x00u, 0x80u, 0x80u, 0x80u, 0x80u, 0x80u, 0x80u, 0x80u, 0x80u, 0x80u, 0x80u, 0x80u, 0x80u, 0x80u, 0x80u, 0x80u),
		_mm_set_epi8(0x80u, 0x80u, 0x80u, 0x80u, 0x80u, 0x80u, 0x80u, 0x80u, 0x80u, 0x80u, 0x80u, 0x80u, 0x80u, 0x80u, 0x80u, 0x80u),
	};
#endif
	//SSE版strcpy用補助関数:_m128iからメモリへのコピー用関数
	inline static void sse_memcpy_a(__m128i* dst, const __m128i src, const std::size_t len)
	{
	#if GASHA_SSE_MEMCPY_TYPE == 1//↓速い
		sse_memcpy_a_table[len](reinterpret_cast<char*>(dst), src);
	#elif GASHA_SSE_MEMCPY_TYPE == 2//↓最速
		sse_memcpy_a_table[len](reinterpret_cast<char*>(dst), src);
	#elif GASHA_SSE_MEMCPY_TYPE == 3//↓かなり遅い
		_mm_maskmoveu_si128(src, sse_memcpy_flags[len], reinterpret_cast<const char*>(dst));
	#else//GASHA_SSE_MEMCPY_TYPE == 0//↓割と速い
		memcpy(reinterpret_cast<const char*>(dst), reinterpret_cast<const char*>(&src), len);
	#endif
	}
	inline static void sse_memcpy_u(char* dst, const __m128i src, const std::size_t len)
	{
	#if GASHA_SSE_MEMCPY_TYPE == 1//↓速い
		sse_memcpy_u_table[len](dst, src);
	#elif GASHA_SSE_MEMCPY_TYPE == 2//↓最速
		sse_memcpy_u_table[len](dst, src);
	#elif GASHA_SSE_MEMCPY_TYPE == 4//↓かなり遅い
		_mm_maskmoveu_si128(src, sse_memcpy_flags[len], dst);
	#else//GASHA_SSE_MEMCPY_TYPE == 0//↓割と速い
		memcpy(dst, reinterpret_cast<const char*>(&src), len);
	#endif
	}
	//SSE版strcpy:16バイトアランイメント＋16バイトアランイメント時
	static const char* strcpy_sse_case0(char* dst, const char* src)
	{
		static const int flags = _SIDD_SBYTE_OPS | _SIDD_CMP_EQUAL_EACH | _SIDD_POSITIVE_POLARITY | _SIDD_LEAST_SIGNIFICANT;
		const __m128i null = _mm_setzero_si128();
		__m128i* dst_p = reinterpret_cast<__m128i*>(dst);
		const __m128i* src_p = reinterpret_cast<const __m128i*>(src);
		while (true)
		{
			const __m128i str16 = _mm_load_si128(src_p);
			const int         zf  = _mm_cmpistrz(null, str16, flags);//※この二行は、コンパイル後は1回の cmpistri になる（はず）
			const std::size_t pos = _mm_cmpistri(null, str16, flags);
			if (zf)
			{
				sse_memcpy_a(dst_p, str16, pos + 1);
				return dst;
			}
			_mm_store_si128(dst_p, str16);
			++dst_p;
			++src_p;
		}
		return nullptr;//dummy
	}
	//SSE版strcpy:非16バイトアランイメント＋16バイトアランイメント時
	static const char* strcpy_sse_case1(char* dst, const char* src)
	{
		static const int flags = _SIDD_SBYTE_OPS | _SIDD_CMP_EQUAL_EACH | _SIDD_POSITIVE_POLARITY | _SIDD_LEAST_SIGNIFICANT;
		const __m128i null = _mm_setzero_si128();
		char* dst_p = dst;
		const __m128i* src_p = reinterpret_cast<const __m128i*>(src);
		while (true)
		{
			const __m128i str16 = _mm_load_si128(src_p);
			const int         zf  = _mm_cmpistrz(null, str16, flags);//※この二行は、コンパイル後は1回の cmpistri になる（はず）
			const std::size_t pos = _mm_cmpistri(null, str16, flags);
			if (zf)
			{
				sse_memcpy_u(dst_p, str16, pos + 1);
				return dst;
			}
			_mm_storeu_si128(reinterpret_cast<__m128i*>(dst_p), str16);
			dst_p += 16;
			++src_p;
		}
		return nullptr;//dummy
	}
	//SSE版strcpy:16バイトアランイメント＋非16バイトアランイメント時
	static const char* strcpy_sse_case2(char* dst, const char* src)
	{
		static const int flags = _SIDD_SBYTE_OPS | _SIDD_CMP_EQUAL_EACH | _SIDD_POSITIVE_POLARITY | _SIDD_LEAST_SIGNIFICANT;
		const __m128i null = _mm_setzero_si128();
		__m128i* dst_p = reinterpret_cast<__m128i*>(dst);
		const char* src_p = src;
		while (true)
		{
			const __m128i str16 = _mm_loadu_si128(reinterpret_cast<const __m128i*>(src_p));
			const int         zf  = _mm_cmpistrz(null, str16, flags);//※この二行は、コンパイル後は1回の cmpistri になる（はず）
			const std::size_t pos = _mm_cmpistri(null, str16, flags);
			if (zf)
			{
				sse_memcpy_a(dst_p, str16, pos + 1);
				return dst;
			}
			_mm_store_si128(dst_p, str16);
			++dst_p;
			src_p += 16;
		}
		return nullptr;//dummy
	}
	//SSE版strcpy:非16バイトアランイメント＋非16バイトアランイメント時
	static const char* strcpy_sse_case3(char* dst, const char* src)
	{
		static const int flags = _SIDD_SBYTE_OPS | _SIDD_CMP_EQUAL_EACH | _SIDD_POSITIVE_POLARITY | _SIDD_LEAST_SIGNIFICANT;
		const __m128i null = _mm_setzero_si128();
		char* dst_p = dst;
		const char* src_p = src;
		while (true)
		{
			const __m128i str16 = _mm_loadu_si128(reinterpret_cast<const __m128i*>(src_p));
			const int         zf  = _mm_cmpistrz(null, str16, flags);//※この二行は、コンパイル後は1回の cmpistri になる（はず）
			const std::size_t pos = _mm_cmpistri(null, str16, flags);
			if (zf)
			{
				sse_memcpy_u(dst_p, str16, pos + 1);
				return dst;
			}
			_mm_storeu_si128(reinterpret_cast<__m128i*>(dst_p), str16);
			dst_p += 16;
			src_p += 16;
		}
		return nullptr;//dummy
	}
	//SSE版strcpy:関数テーブル
	typedef const char* (*strcpy_sse_t)(char* dst, const char* src);
	static const strcpy_sse_t strcpy_sse_table[] =
	{
		strcpy_sse_case0,
		strcpy_sse_case1,
		strcpy_sse_case2,
		strcpy_sse_case3
	};
}//namespace _private
//SSE版strcpy
const char* strcpy_sse(char* dst, const char* src)
{
#if 0//strlen_sseを活用して単純化した処理（遅いのでNG）
	//※もし、memcpyがsseのストアよりも高速であるならこの方が速い（かもしれない）
	memcpy(dst, src, strlen_sse(src));
	return dst;
#else
//nullチェックしない
//	if (!dst || !src)
//		return nullptr;
	return _private::strcpy_sse_table[_private::sse_str_pattern(dst, src)](dst, src);
#endif
}

//----------------------------------------
//SSE版strncpy
namespace _private
{
	//SSE版strncpy用補助関数
	//※前提：この処理は、max_len > len の時にしか呼び出されないものとする。
	//　　　　max_len >= len の時は、_memcpy_m128i() を使用する。
#ifdef GASHA_SSE_STRNCPY_PADDING_ZERO
	inline static void sse_memncpy_a(__m128i* dst, const __m128i src, const std::size_t len, const std::size_t max_len)
	{
		sse_memcpy_a(dst, src, len);
		memset(reinterpret_cast<char*>(dst)+len, '\0', max_len - len);
	}
	inline static void sse_memncpy_u(char* dst, const __m128i src, const std::size_t len, const std::size_t max_len)
	{
		sse_memcpy_u(dst, src, len);
		memset(dst + len, '\0', max_len - len);
	}
#else//GASHA_SSE_STRNCPY_PADDING_ZERO
	inline static void sse_memncpy_a(__m128i* dst, const __m128i src, const std::size_t len)
	{
		sse_memcpy_a(dst, src, len);
		*(reinterpret_cast<char*>(dst)+len) = '\0';
	}
	inline static void sse_memncpy_u(char* dst, const __m128i src, const std::size_t len)
	{
		sse_memcpy_u(dst, src, len);
		dst[len] = '\0';
	}
#endif//GASHA_SSE_STRNCPY_PADDING_ZERO
	//SSE版strncpy:16バイトアランイメント＋16バイトアランイメント時
	static const char* strncpy_sse_case0(char* dst, const char* src, const std::size_t max_len)
	{
		static const int flags = _SIDD_SBYTE_OPS | _SIDD_CMP_EQUAL_EACH | _SIDD_POSITIVE_POLARITY | _SIDD_LEAST_SIGNIFICANT;
		const __m128i null = _mm_setzero_si128();
		__m128i* dst_p = reinterpret_cast<__m128i*>(dst);
		const __m128i* src_p = reinterpret_cast<const __m128i*>(src);
		const char* src_end = src + max_len;
		const char* src_end_16 = src_end - 16;
		while (reinterpret_cast<const char*>(src_p) <= src_end_16)
		{
			const __m128i str16 = _mm_load_si128(src_p);
			const int         zf  = _mm_cmpistrz(null, str16, flags);//※この二行は、コンパイル後は1回の cmpistri になる（はず）
			const std::size_t pos = _mm_cmpistri(null, str16, flags);
			if (zf)
			{
			#ifdef GASHA_SSE_STRNCPY_PADDING_ZERO
				const std::size_t remain = src_end - reinterpret_cast<const char*>(src_p);
				sse_memncpy_a(dst_p, str16, pos, remain);
			#else//GASHA_SSE_STRNCPY_PADDING_ZERO
				sse_memncpy_a(dst_p, str16, pos);
			#endif//GASHA_SSE_STRNCPY_PADDING_ZERO
				return dst;
			}
			_mm_store_si128(dst_p, str16);
			++dst_p;
			++src_p;
		}
		{
			const std::size_t remain = src_end - reinterpret_cast<const char*>(src_p);
			if (remain > 0)
			{
				const __m128i str16 = _mm_load_si128(src_p);
				sse_memcpy_a(dst_p, str16, remain);
			}
		}
		return dst;
	}
	//SSE版strncpy:非16バイトアランイメント＋16バイトアランイメント時
	static const char* strncpy_sse_case1(char* dst, const char* src, const std::size_t max_len)
	{
		static const int flags = _SIDD_SBYTE_OPS | _SIDD_CMP_EQUAL_EACH | _SIDD_POSITIVE_POLARITY | _SIDD_LEAST_SIGNIFICANT;
		const __m128i null = _mm_setzero_si128();
		char* dst_p = dst;
		const __m128i* src_p = reinterpret_cast<const __m128i*>(src);
		const char* src_end = src + max_len;
		const char* src_end_16 = src_end - 16;
		while (reinterpret_cast<const char*>(src_p) <= src_end_16)
		{
			const __m128i str16 = _mm_load_si128(src_p);
			const int         zf  = _mm_cmpistrz(null, str16, flags);//※この二行は、コンパイル後は1回の cmpistri になる（はず）
			const std::size_t pos = _mm_cmpistri(null, str16, flags);
			if (zf)
			{
			#ifdef GASHA_SSE_STRNCPY_PADDING_ZERO
				const std::size_t remain = src_end - reinterpret_cast<const char*>(src_p);
				sse_memncpy_u(dst_p, str16, pos, remain);
			#else//GASHA_SSE_STRNCPY_PADDING_ZERO
				sse_memncpy_u(dst_p, str16, pos);
			#endif//GASHA_SSE_STRNCPY_PADDING_ZERO
				return dst;
			}
			_mm_storeu_si128(reinterpret_cast<__m128i*>(dst_p), str16);
			dst_p += 16;
			++src_p;
		}
		{
			const std::size_t remain = src_end - reinterpret_cast<const char*>(src_p);
			if (remain > 0)
			{
				const __m128i str16 = _mm_load_si128(src_p);
				sse_memcpy_u(dst_p, str16, remain);
			}
		}
		return dst;
	}
	//SSE版strncpy:16バイトアランイメント＋非16バイトアランイメント時
	static const char* strncpy_sse_case2(char* dst, const char* src, const std::size_t max_len)
	{
		static const int flags = _SIDD_SBYTE_OPS | _SIDD_CMP_EQUAL_EACH | _SIDD_POSITIVE_POLARITY | _SIDD_LEAST_SIGNIFICANT;
		const __m128i null = _mm_setzero_si128();
		__m128i* dst_p = reinterpret_cast<__m128i*>(dst);
		const char* src_p = src;
		const char* src_end = src + max_len;
		const char* src_end_16 = src_end - 16;
		while (src_p <= src_end_16)
		{
			const __m128i str16 = _mm_loadu_si128(reinterpret_cast<const __m128i*>(src_p));
			const int         zf  = _mm_cmpistrz(null, str16, flags);//※この二行は、コンパイル後は1回の cmpistri になる（はず）
			const std::size_t pos = _mm_cmpistri(null, str16, flags);
			if (zf)
			{
			#ifdef GASHA_SSE_STRNCPY_PADDING_ZERO
				const std::size_t remain = src_end - src_p;
				sse_memncpy_a(dst_p, str16, pos, remain);
			#else//GASHA_SSE_STRNCPY_PADDING_ZERO
				sse_memncpy_a(dst_p, str16, pos);
			#endif//GASHA_SSE_STRNCPY_PADDING_ZERO
				return dst;
			}
			_mm_store_si128(dst_p, str16);
			++dst_p;
			src_p += 16;
		}
		{
			const std::size_t remain = src_end - src_p;
			if (remain > 0)
			{
				const __m128i str16 = _mm_loadu_si128(reinterpret_cast<const __m128i*>(src_p));
				sse_memcpy_a(dst_p, str16, remain);
			}
		}
		return dst;
	}
	//SSE版strncpy:非16バイトアランイメント＋非16バイトアランイメント時
	static const char* strncpy_sse_case3(char* dst, const char* src, const std::size_t max_len)
	{
		static const int flags = _SIDD_SBYTE_OPS | _SIDD_CMP_EQUAL_EACH | _SIDD_POSITIVE_POLARITY | _SIDD_LEAST_SIGNIFICANT;
		const __m128i null = _mm_setzero_si128();
		char* dst_p = dst;
		const char* src_p = src;
		const char* src_end = src + max_len;
		const char* src_end_16 = src_end - 16;
		while (src_p <= src_end_16)
		{
			const __m128i str16 = _mm_loadu_si128(reinterpret_cast<const __m128i*>(src_p));
			const int         zf  = _mm_cmpistrz(null, str16, flags);//※この二行は、コンパイル後は1回の cmpistri になる（はず）
			const std::size_t pos = _mm_cmpistri(null, str16, flags);
			if (zf)
			{
			#ifdef GASHA_SSE_STRNCPY_PADDING_ZERO
				const std::size_t remain = src_end - src_p;
				sse_memncpy_u(dst_p, str16, pos, remain);
			#else//GASHA_SSE_STRNCPY_PADDING_ZERO
				sse_memncpy_u(dst_p, str16, pos);
			#endif//GASHA_SSE_STRNCPY_PADDING_ZERO
				return dst;
			}
			_mm_storeu_si128(reinterpret_cast<__m128i*>(dst_p), str16);
			dst_p += 16;
			src_p += 16;
		}
		{
			const std::size_t remain = src_end - src_p;
			if (remain > 0)
			{
				const __m128i str16 = _mm_loadu_si128(reinterpret_cast<const __m128i*>(src_p));
				sse_memcpy_u(dst_p, str16, remain);
			}
		}
		return dst;
	}
	//SSE版strncpy:関数テーブル
	typedef const char* (*strncpy_sse_t)(char* dst, const char* src, const std::size_t max_len);
	static const strncpy_sse_t strncpy_sse_table[] =
	{
		strncpy_sse_case0,
		strncpy_sse_case1,
		strncpy_sse_case2,
		strncpy_sse_case3
	};
}//namespace _private
//SSE版strncpy
const char* strncpy_sse(char* dst, const char* src, const std::size_t max_len)
{
#if 0//strlen_sseを活用して単純化した処理（遅いのでNG）
	//※もし、memcpyがsseのストアよりも高速であるならこの方が速い（かもしれない）
	const std::size_t len = strlen_sse(src);
	if (len >= max_len)
	{
		memcpy(dst, src, max_len);
		return dst;
	}
	else
	{
		memcpy(dst, src, len);
		//memset(dst + len, '\0', max_len - len);//strncpy本来の仕様
		dst[len] = '\0';//これでも十分
		return dst;
	}
	return nullptr;//dummy
#else
//nullチェックしない
//	if (!dst || !src)
//		return nullptr;
	return _private::strncpy_sse_table[_private::sse_str_pattern(dst, src)](dst, src, max_len);
#endif
}

#else//GASHA_USE_SSE4_2

//----------------------------------------
//SSE版strlen
std::size_t strlen_sse(const char* str)
{
	return strlen(str);
}

//----------------------------------------
//SSE版strnlen
std::size_t strnlen_sse(const char* str, const std::size_t max_len)
{
	return strnlen(str, max_len);
}

//----------------------------------------
//SSE版strcmp
int strcmp_sse(const char* str1, const char* str2)
{
	return strcmp(str1, str2);
}

//----------------------------------------
//SSE版strncmp
int strncmp_sse(const char* str1, const char* str2, const std::size_t max_len)
{
	return strncmp(str1, str2, max_len);
}

//----------------------------------------
//SSE版strchr
const char* strchr_sse(const char* str, const char c)
{
	return strchr(str, c);
}

//----------------------------------------
//SSE版strrchr
const char* strrchr_sse(const char* str, const char c)
{
	return strrchr(str, c);
}

//----------------------------------------
//SSE版strstr
const char* strstr_sse(const char* str, const char* pattern)
{
	return strstr(str, pattern);
}

//----------------------------------------
//SSE版strstr(BM法)
const char* strstrbm_sse(const char* str, const char* pattern, std::function<bool(const char*, const char*)> found_it)
{
	return strstrbm(str, pattern, found_it);
}

//----------------------------------------
//SSE版strstr0
const char* strstr0_sse(const char* str, const char* pattern)
{
	return strstr0(str, pattern);
}

//----------------------------------------
//SSE版strcpy
const char* strcpy_sse(char* dst, const char* src)
{
	return strcpy(dst, src);
}

//----------------------------------------
//SSE版strncpy
const char* strncpy_sse(char* dst, const char* src, const std::size_t max_len)
{
	return strncpy(dst, src, max_len);
}

#endif//GASHA_USE_SSE4_2

GASHA_NAMESPACE_END;//ネームスペース：終了

// End of file