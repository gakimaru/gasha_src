//--------------------------------------------------------------------------------
// build_settings.cpp
// ビルド設定
//
// Gakimaru's researched and standard library for C++ - GASHA
//   Copyright (c) 2014 Itagaki Mamoru
//   Released under the MIT license.
//     https://github.com/gakimaru/gasha/blob/master/LICENSE
//--------------------------------------------------------------------------------

#include <gasha/build_settings/build_settings.h>//ビルド設定

#include <gasha/cpuid.h>//CPU情報（x86系CPU用）

#ifdef GASHA_USE_AVX
#include <immintrin.h>//AVX
#endif//GASHA_USE_AVX

#include <stdio.h>//printf(), fprintf()
#include <string.h>//strncpy()
#include <assert.h>//assert()
#include <stdlib.h>//abort()

//【VC++】strncpyを使用すると、error C4996 が発生する
//  error C4996: 'strncpy': This function or variable may be unsafe. Consider using strncpy_s instead. To disable deprecation, use _CRT_SECURE_NO_WARNINGS. See online help for details.
#pragma warning(disable: 4996)//C4996を抑える

GASHA_NAMESPACE_BEGIN;//ネームスペース：開始

//--------------------
//ビルド設定が実行環境に適合するかチェック
//※ダメな要素があった場合、即abort
void checkBuildSettings()
{
	printf("------------------------------------------------------------------------------\n");
	printf("Checking build-settings suitable for runtime environment.\n");
	printf("\n");
	
	bool is_error = false;
	
	//CPU機能判定
#ifdef GASHA_IS_X86
	//x86系CPU情報取得
	char cpu_info_str[12] = { 0 };
	int cpu_info[4] = { 0, 0, 0, 0 };
	__cpuid(cpu_info, 0);//CPU情報取得：Type0
	strncpy(cpu_info_str, reinterpret_cast<const char*>(&cpu_info[1]), sizeof(cpu_info_str) - 1);
	printf("cpu_string=\"%s\"\n", cpu_info_str);
	__cpuid(cpu_info, 1);//CPU情報取得：Type1

#ifdef GASHA_USE_SSE
	const bool sse_is_supported = (cpu_info[3] & (1 << 25)) || false;//SSE対応
	if (!sse_is_supported)
	{
		is_error = true;
		fprintf(stderr, "[NG] SSE is NOT supported!\n");
	}
	printf("[OK] SSE is supported.\n");
#else//GASHA_USE_SSE
	printf("[--] SSE is not used.\n");
#endif//GASHA_USE_SSE

#ifdef GASHA_USE_SSE2
	const bool sse2_is_supported = (cpu_info[3] & (1 << 26)) || false;//SSE2対応
	if (!sse2_is_supported)
	{
		is_error = true;
		fprintf(stderr, "[NG] SSE2 is NOT supported!\n");
	}
	printf("[OK] SSE2 is supported.\n");
#else//GASHA_USE_SSE2
	printf("[--] SSE2 is not used.\n");
#endif//GASHA_USE_SSE2

#ifdef GASHA_USE_SSE3
	const bool sse3_is_supported = (cpu_info[2] & (1 << 9)) || false;//SSE3対応
	if (!sse3_is_supported)
	{
		is_error = true;
		fprintf(stderr, "[NG] SSE3 is NOT supported!\n");
	}
	printf("[OK] SSE3 is supported.\n");
#else//GASHA_USE_SSE3
	printf("[--] SSE3 is not used.\n");
#endif//GASHA_USE_SSE3

#ifdef GASHA_USE_SSE4A
	const bool sse4a_is_supported = (cpu_info[2] & (1 << 6)) || false;//SSE4a対応
	if (!sse4a_is_supported)
	{
		is_error = true;
		fprintf(stderr, "[NG] SSE4a is NOT supported!\n");
	}
	printf("[OK] SSE4a is NOT supported.\n");
#else//GASHA_USE_SSE4A
	printf("[--] SSE 4a is not used.\n");
#endif//GASHA_USE_SSE4A

#ifdef GASHA_USE_SSE4_1
	const bool sse4_1_is_supported = (cpu_info[2] & (1 << 19)) || false;//SSE4.1対応
	if (!sse4_1_is_supported)
	{
		is_error = true;
		fprintf(stderr, "[NG] SSE4.1 is NOT supported!\n");
	}
	printf("[OK] SSE4.1 is NOT supported.\n");
#else//GASHA_USE_SSE4_1
	printf("[--] SSE4.1 is not used.\n");
#endif//GASHA_USE_SSE4_1

#ifdef GASHA_USE_SSE4_2
	const bool sse4_2_is_supported = (cpu_info[2] & (1 << 20)) || false;//SSE4.2対応
	if (!sse4_2_is_supported)
	{
		is_error = true;
		fprintf(stderr, "[NG] SSE4.2 is NOT supported!\n");
	}
	printf("[OK] SSE4.2 is supported.\n");
#else//GASHA_USE_SSE4_2
	printf("[--] SSE4.2 is not used.\n");
#endif//GASHA_USE_SSE4_2

#ifdef GASHA_USE_POPCNT
	const bool poopcnt_is_supported = (cpu_info[2] & (1 << 23)) || false;//POPCNT対応
	if (!poopcnt_is_supported)
	{
		is_error = true;
		fprintf(stderr, "[NG] POPCNT is NOT supported!\n");
	}
	printf("[OK] POPCNT is supported.\n");
#else//GASHA_USE_POPCNT
	printf("[--] POPCNT is not used.\n");
#endif//GASHA_USE_POPCNT

#ifdef GASHA_USE_AES
	const bool aes_is_supported = (cpu_info[2] & (1 << 25)) || false;//AES対応
	if (!aes_is_supported)
	{
		is_error = true;
		fprintf(stderr, "[NG] AES is NOT supported!\n");
	}
	printf("[OK] AES is supported.\n");
#else//GASHA_USE_AES
	printf("[--] AES is not used.\n");
#endif//GASHA_USE_AES

#ifdef GASHA_USE_AVX
	const bool osxsave_is_supported = (cpu_info[2] & (1 << 27)) || false;//OSXSAVE対応
	const bool _avx_is_supported = (cpu_info[2] & (1 << 28)) || false;//AVX対応（基本）
#endif//GASHA_USE_AVX

#ifdef GASHA_USE_AVX
	bool avx_is_supported = false;//AVX対応
	if (osxsave_is_supported && _avx_is_supported)
	{
		unsigned long long xcr_feature_enabled = _xgetbv(_XCR_XFEATURE_ENABLED_MASK);
        avx_is_supported = (xcr_feature_enabled & 0x6) || false;
	}
	if(!avx_is_supported)
	{
		is_error = true;
		fprintf(stderr, "[NG] AVX is NOT supported!\n");
	}
	printf("[OK] AVX is supported.\n");
#else//GASHA_USE_AVX

	printf("[--] AVX is not used.\n");
#endif//GASHA_USE_AVX
#ifdef GASHA_USE_AVX2//※判定未対応
	printf("[OK?] AVX2 is supported?\n");
#else//GASHA_USE_AVX2
	printf("[--] AVX2 is not used.\n");
#endif//GASHA_USE_AVX2

#endif//GASHA_IS_X86

	printf("\n");
	printf("------------------------------------------------------------------------------\n");
	printf("Build-settings by compiled environment.\n");
	
	//CPUアーキテクチャ情報
	printf("\n");
	printf("Platform: %s (Target Versionn=%d.%d)\n", GASHA_PLATFORM_NAME, GASHA_PLATFORM_VER, GASHA_PLATFORM_MINOR);
	printf("CPU Architecture: %s (%d bit, %s endian)", GASHA_PLATFORM_ARCHITECTURE_NAME, GASHA_PLATFORM_ARCHITECTURE_BITS, GASHA_ENDIAN_NAME);
#ifdef GASHA_IS_X86
	printf(" [x86]");
#endif//GASHA_IS_X86
#ifdef GASHA_IS_X64
	printf(" [x64]");
#endif//GASHA_IS_X64
	printf("\n");

	//コンパイラ情報
	printf("\n");
	printf("Compiler: %s (Version=%d.%d)", GASHA_COMPILER_NAME, GASHA_COMPILER_VER, GASHA_COMPILER_MINOR);
#ifdef GASHA_IS_VC
	printf(" ... ");
	switch (GASHA_COMPILER_VER)
	{
	case 600: printf("MS-C Compiler 6.0"); break;
	case 700: printf("MS-C/C++ Compiler 7.0"); break;
	case 800: printf("Visual C++ 1.0"); break;
	case 900: printf("Visual C++ 2.0"); break;
	case 1000: printf("Visual C++ 4.0"); break;
	case 1010: printf("Visual C++ 4.1"); break;
	case 1020: printf("Visual C++ 4.2"); break;
	case 1100: printf("Visual C++ 5.0 (Visual Studio 97)"); break;
	case 1200: printf("Visual C++ 6.0 (Visual Studio 6.0)"); break;
	case 1300: printf("Visual C++ 7.0 (Visual Studio.NET 2002)"); break;
	case 1310: printf("Visual C++ 7.1 (Visual Studio.NET 2003)"); break;
	case 1400: printf("Visual C++ 8.0 (Visual Studio 2005)"); break;
	case 1500: printf("Visual C++ 9.0 (Visual Studio 2008)"); break;
	case 1600: printf("Visual C++ 10.0 (Visual Studio 2010)"); break;
	case 1700: printf("Visual C++ 11.0 (Visual Studio 2012)"); break;
	case 1800: printf("Visual C++ 12.0 (Visual Studio 2013)"); break;
	default: printf("Unknown"); break;
	}
#endif//GASHA_IS_VC
	
	//言語仕様判定
	printf("\n");
	printf("Language: %s", GASHA_PROGRAM_LANGUAGE_NAME);
#ifdef __cplusplus
	printf(" (__cplusplus=%ld)", __cplusplus);
#endif//__cplusplus
#ifdef GASHA_HAS_CPP98
	printf(" [C++98]");
#endif//GASHA_HAS_CPP98
#ifdef GASHA_HAS_CPP03
	printf(" [C++03]");
#endif//GASHA_HAS_CPP03
#ifdef GASHA_HAS_CPP11
	printf(" [C++11]");
#endif//GASHA_HAS_CPP11
#ifdef GASHA_HAS_CPP14
	printf(" [C++14]");
#endif//GASHA_HAS_CPP14
	printf("\n");

	printf("\n");

	//【C++11仕様】nullptr
	printf("- nullptr ... ");
#ifdef GASHA_HAS_NULLPTR
	printf("is available.\n");
#else//GASHA_HAS_NULLPTR
#ifdef GASHA_HAS_NULLPTR_PROXY
	printf("is PROXY available.\n");
#else//GASHA_HAS_NULLPTR_PROXY
	printf("is NOT available.\n");
#endif//GASHA_HAS_NULLPTR_PROXY
#endif//GASHA_HAS_NULLPTR

	//【C++11仕様】override
	printf("- override ... ");
#ifdef GASHA_HAS_OVERRIDE
	printf("is available.\n");
#else//GASHA_HAS_OVERRIDE
#ifdef GASHA_HAS_OVERRIDE_DUMMY
	printf("is DUMMY.\n");
#else//GASHA_HAS_OVERRIDE_DUMMY
	printf("is NOT available.\n");
#endif//GASHA_HAS_OVERRIDE_DUMMY
#endif//GASHA_HAS_OVERRIDE

	//【C++11仕様】final
	printf("- final ... ");
#ifdef GASHA_HAS_FINAL
	printf("is available.\n");
#else//GASHA_HAS_FINAL
#ifdef GASHA_HAS_FINAL_DUMMY
	printf("is DUMMY.\n");
#else//GASHA_HAS_FINAL_DUMMY
	printf("is NOT available.\n");
#endif//GASHA_HAS_FINAL_DUMMY
#endif//GASHA_HAS_FINAL

	//【C++11仕様】auto型推論
	printf("- auto ... ");
#ifdef GASHA_HAS_AUTO
	printf("is available.\n");
#else//GASHA_HAS_AUTO
	printf("is NOT available.\n");
#endif//GASHA_HAS_AUTO

	//【C++11仕様】decltype型指定子
	printf("- decltype ... ");
#ifdef GASHA_HAS_DECLTYPE
	printf("is available.\n");
#else//GASHA_HAS_DECLTYPE
	printf("is NOT available.\n");
#endif//GASHA_HAS_DECLTYPE
	
	//【C++11仕様】default/delete宣言
	printf("- default/delete ... ");
#ifdef GASHA_HAS_DEFAULT_AND_DELETE
	printf("is available.\n");
#else//GASHA_HAS_DEFAULT_AND_DELETE
	printf("is NOT available.\n");
#endif//GASHA_HAS_DEFAULT_AND_DELETE

	//【C++11仕様】constexpr
	printf("- constexpr ... ");
#ifdef GASHA_HAS_CONSTEXPR
	printf("is available.\n");
#else//GASHA_HAS_CONSTEXPR
#ifdef GASHA_HAS_CONSTEXPR_DUMMY
	printf("is DUMMY.\n");
#else//GASHA_HAS_CONSTEXPR_DUMMY
	printf("is NOT available.\n");
#endif//GASHA_HAS_CONSTEXPR_DUMMY
#endif//GASHA_HAS_CONSTEXPR

	//【C++11仕様】ユーザー定義リテラル
	printf("- user-defined-literal ... ");
#ifdef GASHA_HAS_USER_DEFINED_LITERAL
	printf("is available.\n");
#else//GASHA_HAS_USER_DEFINED_LITERAL
	printf("is NOT available.\n");
#endif//GASHA_HAS_USER_DEFINED_LITERAL

	//【C++11仕様】ラムダ式
	printf("- lambda expression ... ");
#ifdef GASHA_HAS_LAMBDA_EXPRESSION
	printf("is available.\n");
#else//GASHA_HAS_LAMBDA_EXPRESSION
	printf("is NOT available.\n");
#endif//GASHA_HAS_LAMBDA_EXPRESSION

	//【C++11仕様】範囲に基づくforループ
	printf("- range-based for ... ");
#ifdef GASHA_HAS_RANGE_BASED_FOR
	printf("is available.\n");
#else//GASHA_HAS_RANGE_BASED_FOR
	printf("is NOT available.\n");
#endif//GASHA_HAS_RANGE_BASED_FOR

	//【C++11仕様】強い型付けを持った列挙型（enumの型指定）
	printf("- enum class ... ");
#ifdef GASHA_HAS_ENUM_CLASS
	printf("is available.\n");
#else//GASHA_HAS_ENUM_CLASS
	printf("is NOT available.\n");
#endif//GASHA_HAS_ENUM_CLASS

	//【C++11仕様】可変長引数テンプレート
	printf("- variadic template ... ");
#ifdef GASHA_HAS_VARIADIC_TEMPLATE
	printf("is available.\n");
#else//GASHA_HAS_VARIADIC_TEMPLATE
	printf("is NOT available.\n");
#endif//GASHA_HAS_VARIADIC_TEMPLATE

	//【C++11仕様】テンプレートエイリアス（テンプレートの別名using）
	printf("- template aliases ... ");
#ifdef GASHA_HAS_TEMPLATE_ALIASES
	printf("is available.\n");
#else//GASHA_HAS_TEMPLATE_ALIASES
	printf("is NOT available.\n");
#endif//GASHA_HAS_TEMPLATE_ALIASES

	//【C++11仕様】右辺値参照とムーブセマンティクス
	printf("- rvalue reference & std::move ... ");
#ifdef GASHA_HAS_RVALUE_REFERENCE
	printf("is available.\n");
#else//GASHA_HAS_RVALUE_REFERENCE
	printf("is NOT available.\n");
#endif//GASHA_HAS_RVALUE_REFERENCE

	//【C++11仕様】静的アサーション
	printf("- static_assert ... ");
#ifdef GASHA_HAS_STATIC_ASSERT
	printf("is available.\n");
#else//GASHA_HAS_STATIC_ASSERT
#ifdef GASHA_HAS_STATIC_ASSERT_PROXY
	printf("is available.(substitution)\n");
#else//GASHA_HAS_STATIC_ASSERT_PROXY
	printf("is NOT available.\n");
#endif//GASHA_HAS_STATIC_ASSERT_PROXY
#endif//GASHA_HAS_STATIC_ASSERT

	//【C++11仕様】TLS指定子
	printf("- thread_local ... ");
#ifdef GASHA_HAS_THREAD_LOCAL
	printf("is available.\n");
#else//GASHA_HAS_THREAD_LOCAL
#ifdef GASHA_HAS_THREAD_LOCAL_PROXY
	printf("is PROXY available.\n");
#else//GASHA_HAS_THREAD_LOCAL_PROXY
	printf("is NOT available.\n");
#endif//GASHA_HAS_THREAD_LOCAL_PROXY
#endif//GASHA_HAS_THREAD_LOCAL

	//【C++11仕様】alignas
	printf("- alignas ... ");
#ifdef GASHA_HAS_ALIGNAS
	printf("is available.\n");
#else//GASHA_HAS_ALIGNAS
#ifdef GASHA_HAS_ALIGNAS_PROXY
	printf("is PROXY available.\n");
#else//GASHA_HAS_ALIGNAS_PROXY
	printf("is NOT available.\n");
#endif//GASHA_HAS_ALIGNAS_PROXY
#endif//GASHA_HAS_ALIGNAS

	//【C++11仕様】alignof
	printf("- alignof ... ");
#ifdef GASHA_HAS_ALIGNOF
	printf("is available.\n");
#else//GASHA_HAS_ALIGNOF
#ifdef GASHA_HAS_ALIGNOF_PROXY
	printf("is PROXY available.\n");
#else//GASHA_HAS_ALIGNOF_PROXY
	printf("is NOT available.\n");
#endif//GASHA_HAS_ALIGNOF_PROXY
#endif//GASHA_HAS_ALIGNOF

	printf("\n");
	
	//_aligned_malloc 
	//※C++11仕様ではなく、コンパイラ独自仕様の共通化
	printf("- _aligned_malloc ... ");
#ifdef GASHA_HAS_ALIGNED_MALLOC
	printf("is available.\n");
#else//GASHA_HAS_ALIGNED_MALLOC
#ifdef GASHA_HAS_ALIGNED_MALLOC_PROXY
	printf("is available with commonized.\n");
#else//GASHA_HAS_ALIGNED_MALLOC_PROXY
	printf("is NOT available.\n");
#endif//GASHA_HAS_ALIGNED_MALLOC_PROXY
#endif//GASHA_HAS_ALIGNED_MALLOC
	
	//_aligned_free
	//※C++11仕様ではなく、コンパイラ独自仕様の共通化
	printf("- _aligned_free ... ");
#ifdef GASHA_HAS_ALIGNED_FREE
	printf("is available.\n");
#else//GASHA_HAS_ALIGNED_FREE
#ifdef GASHA_HAS_ALIGNED_FREE_PROXY
	printf("is available with commonized.\n");
#else//GASHA_HAS_ALIGNED_FREE_PROXY
	printf("is NOT available.\n");
#endif//GASHA_HAS_ALIGNED_FREE_PROXY
#endif//GASHA_HAS_ALIGNED_FREE

	//no_inline
	//※C++11仕様ではなく、コンパイラ独自仕様の共通化
	printf("- no_inline ... ");
#ifdef GASHA_HAS_NO_INLINE
	printf("is available.\n");
#else//GASHA_HAS_NO_INLINE
#ifdef GASHA_HAS_NO_INLINE_PROXY
	printf("is available with commonized.\n");
#else//GASHA_HAS_NO_INLINE_PROXY
	printf("is NOT available.\n");
#endif//GASHA_HAS_NO_INLINE_PROXY
#endif//GASHA_HAS_NO_INLINE

	//always_inline
	//※C++11仕様ではなく、コンパイラ独自仕様の共通化
	printf("- always_inline ... ");
#ifdef GASHA_HAS_ALWAYS_INLINE
	printf("is available.\n");
#else//GASHA_HAS_ALWAYS_INLINE
#ifdef GASHA_HAS_ALWAYS_INLINE_PROXY
	printf("is available with commonized.\n");
#else//GASHA_HAS_ALWAYS_INLINE_PROXY
	printf("is NOT available.\n");
#endif//GASHA_HAS_ALWAYS_INLINE_PROXY
#endif//GASHA_HAS_ALWAYS_INLINE

	if (is_error)
	{
		printf("This program is NOT suitable to this machine !!\n");
		printf("Abort this program.\n");
		abort();
	}
}

GASHA_NAMESPACE_END;//ネームスペース：終了

// End of file
