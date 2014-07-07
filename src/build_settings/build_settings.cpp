//--------------------------------------------------------------------------------
// build_settings.cpp
// ビルド設定【関数／実体定義部】
//
// Gakimaru's researched and standard library for C++ - GASHA
//   Copyright (c) 2014 Itagaki Mamoru
//   Released under the MIT license.
//     https://github.com/gakimaru/gasha/blob/master/LICENSE
//--------------------------------------------------------------------------------

#include <gasha/build_settings/build_settings.h>//ビルド設定

#include <gasha/fast_string.h>//高速文字列：strncpy_fast()

#include <gasha/cpuid.h>//CPU情報（x86系CPU用）

#include <stdio.h>//sprintf

#ifdef GASHA_USE_AVX
#include <immintrin.h>//AVX
#endif//GASHA_USE_AVX

//【VC++】sprintf を使用すると、error C4996 が発生する
//  error C4996: 'sprintf': This function or variable may be unsafe. Consider using strncpy_fast_s instead. To disable deprecation, use _CRT_SECURE_NO_WARNINGS. See online help for details.
#pragma warning(disable: 4996)//C4996を抑える

GASHA_NAMESPACE_BEGIN;//ネームスペース：開始

//--------------------
//ビルド設定が実行環境に適合するか診断
bool diagnoseBuildSettings(char* message, std::size_t& size)
{
	size = 0;
	message[0] = '\0';

	size += sprintf(message + size, "------------------------------------------------------------------------------\n");
	size += sprintf(message + size, "[ Diagnose build-settings suitable for runtime environment ]\n");
	size += sprintf(message + size, "\n");
	
	bool has_error = false;
	
	//CPU機能判定
#ifdef GASHA_IS_X86
	//x86系CPU情報取得
	char cpu_info_str[12] = { 0 };
	int cpu_info[4] = { 0, 0, 0, 0 };
	__cpuid(cpu_info, 0);//CPU情報取得：Type0
	strncpy_fast(cpu_info_str, reinterpret_cast<const char*>(&cpu_info[1]), sizeof(cpu_info_str) - 1);
	size += sprintf(message + size, "cpu_string=\"%s\"\n", cpu_info_str);
	__cpuid(cpu_info, 1);//CPU情報取得：Type1

#ifdef GASHA_USE_SSE
	const bool sse_is_supported = (cpu_info[3] & (1 << 25)) || false;//SSE対応
	if (!sse_is_supported)
	{
		has_error = true;
		size += sprintf(message + size, "[NG] SSE is NOT supported!\n");
	}
	size += sprintf(message + size, "[OK] SSE is supported.\n");
#else//GASHA_USE_SSE
	size += sprintf(message + size, "[--] SSE is not used.\n");
#endif//GASHA_USE_SSE

#ifdef GASHA_USE_SSE2
	const bool sse2_is_supported = (cpu_info[3] & (1 << 26)) || false;//SSE2対応
	if (!sse2_is_supported)
	{
		has_error = true;
		size += sprintf(message + size, "[NG] SSE2 is NOT supported!\n");
	}
	size += sprintf(message + size, "[OK] SSE2 is supported.\n");
#else//GASHA_USE_SSE2
	size += sprintf(message + size, "[--] SSE2 is not used.\n");
#endif//GASHA_USE_SSE2

#ifdef GASHA_USE_SSE3
	const bool sse3_is_supported = (cpu_info[2] & (1 << 9)) || false;//SSE3対応
	if (!sse3_is_supported)
	{
		has_error = true;
		size += sprintf(message + size, "[NG] SSE3 is NOT supported!\n");
	}
	size += sprintf(message + size, "[OK] SSE3 is supported.\n");
#else//GASHA_USE_SSE3
	size += sprintf(message + size, "[--] SSE3 is not used.\n");
#endif//GASHA_USE_SSE3

#ifdef GASHA_USE_SSE4A
	const bool sse4a_is_supported = (cpu_info[2] & (1 << 6)) || false;//SSE4a対応
	if (!sse4a_is_supported)
	{
		has_error = true;
		size += sprintf(message + size, "[NG] SSE4a is NOT supported!\n");
	}
	size += sprintf(message + size, "[OK] SSE4a is NOT supported.\n");
#else//GASHA_USE_SSE4A
	size += sprintf(message + size, "[--] SSE 4a is not used.\n");
#endif//GASHA_USE_SSE4A

#ifdef GASHA_USE_SSE4_1
	const bool sse4_1_is_supported = (cpu_info[2] & (1 << 19)) || false;//SSE4.1対応
	if (!sse4_1_is_supported)
	{
		has_error = true;
		size += sprintf(message + size, "[NG] SSE4.1 is NOT supported!\n");
	}
	size += sprintf(message + size, "[OK] SSE4.1 is NOT supported.\n");
#else//GASHA_USE_SSE4_1
	size += sprintf(message + size, "[--] SSE4.1 is not used.\n");
#endif//GASHA_USE_SSE4_1

#ifdef GASHA_USE_SSE4_2
	const bool sse4_2_is_supported = (cpu_info[2] & (1 << 20)) || false;//SSE4.2対応
	if (!sse4_2_is_supported)
	{
		has_error = true;
		size += sprintf(message + size, "[NG] SSE4.2 is NOT supported!\n");
	}
	size += sprintf(message + size, "[OK] SSE4.2 is supported.\n");
#else//GASHA_USE_SSE4_2
	size += sprintf(message + size, "[--] SSE4.2 is not used.\n");
#endif//GASHA_USE_SSE4_2

#ifdef GASHA_USE_POPCNT
	const bool poopcnt_is_supported = (cpu_info[2] & (1 << 23)) || false;//POPCNT対応
	if (!poopcnt_is_supported)
	{
		has_error = true;
		size += sprintf(message + size, "[NG] POPCNT is NOT supported!\n");
	}
	size += sprintf(message + size, "[OK] POPCNT is supported.\n");
#else//GASHA_USE_POPCNT
	size += sprintf(message + size, "[--] POPCNT is not used.\n");
#endif//GASHA_USE_POPCNT

#ifdef GASHA_USE_AES
	const bool aes_is_supported = (cpu_info[2] & (1 << 25)) || false;//AES対応
	if (!aes_is_supported)
	{
		has_error = true;
		size += sprintf(message + size, "[NG] AES is NOT supported!\n");
	}
	size += sprintf(message + size, "[OK] AES is supported.\n");
#else//GASHA_USE_AES
	size += sprintf(message + size, "[--] AES is not used.\n");
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
		has_error = true;
		size += sprintf(message + size, "[NG] AVX is NOT supported!\n");
	}
	size += sprintf(message + size, "[OK] AVX is supported.\n");
#else//GASHA_USE_AVX

	size += sprintf(message + size, "[--] AVX is not used.\n");
#endif//GASHA_USE_AVX
#ifdef GASHA_USE_AVX2//※判定未対応
	size += sprintf(message + size, "[OK?] AVX2 is supported?\n");
#else//GASHA_USE_AVX2
	size += sprintf(message + size, "[--] AVX2 is not used.\n");
#endif//GASHA_USE_AVX2

#endif//GASHA_IS_X86

	size += sprintf(message + size, "\n");
	size += sprintf(message + size, "------------------------------------------------------------------------------\n");
	size += sprintf(message + size, "[ Build-settings by compiled environment ]\n");
	
	//CPUアーキテクチャ情報
	size += sprintf(message + size, "\n");
	size += sprintf(message + size, "Platform: %s (Target Versionn=%d.%d)\n", GASHA_PLATFORM_NAME, GASHA_PLATFORM_VER, GASHA_PLATFORM_MINOR);
	size += sprintf(message + size, "CPU Architecture: %s (%d bit, %s endian)", GASHA_PLATFORM_ARCHITECTURE_NAME, GASHA_PLATFORM_ARCHITECTURE_BITS, GASHA_ENDIAN_NAME);
#ifdef GASHA_IS_X86
	size += sprintf(message + size, " [x86]");
#endif//GASHA_IS_X86
#ifdef GASHA_IS_X64
	size += sprintf(message + size, " [x64]");
#endif//GASHA_IS_X64
	size += sprintf(message + size, "\n");

	//コンパイラ情報
	size += sprintf(message + size, "\n");
	size += sprintf(message + size, "Compiler: %s (Version=%d.%d)", GASHA_COMPILER_NAME, GASHA_COMPILER_VER, GASHA_COMPILER_MINOR);
#ifdef GASHA_IS_VC
	size += sprintf(message + size, " ... ");
	switch (GASHA_COMPILER_VER)
	{
	case 600: size += sprintf(message + size, "MS-C Compiler 6.0"); break;
	case 700: size += sprintf(message + size, "MS-C/C++ Compiler 7.0"); break;
	case 800: size += sprintf(message + size, "Visual C++ 1.0"); break;
	case 900: size += sprintf(message + size, "Visual C++ 2.0"); break;
	case 1000: size += sprintf(message + size, "Visual C++ 4.0"); break;
	case 1010: size += sprintf(message + size, "Visual C++ 4.1"); break;
	case 1020: size += sprintf(message + size, "Visual C++ 4.2"); break;
	case 1100: size += sprintf(message + size, "Visual C++ 5.0 (Visual Studio 97)"); break;
	case 1200: size += sprintf(message + size, "Visual C++ 6.0 (Visual Studio 6.0)"); break;
	case 1300: size += sprintf(message + size, "Visual C++ 7.0 (Visual Studio.NET 2002)"); break;
	case 1310: size += sprintf(message + size, "Visual C++ 7.1 (Visual Studio.NET 2003)"); break;
	case 1400: size += sprintf(message + size, "Visual C++ 8.0 (Visual Studio 2005)"); break;
	case 1500: size += sprintf(message + size, "Visual C++ 9.0 (Visual Studio 2008)"); break;
	case 1600: size += sprintf(message + size, "Visual C++ 10.0 (Visual Studio 2010)"); break;
	case 1700: size += sprintf(message + size, "Visual C++ 11.0 (Visual Studio 2012)"); break;
	case 1800: size += sprintf(message + size, "Visual C++ 12.0 (Visual Studio 2013)"); break;
	default: size += sprintf(message + size, "Unknown"); break;
	}
#endif//GASHA_IS_VC
	
	//言語仕様判定
	size += sprintf(message + size, "\n");
	size += sprintf(message + size, "Language: %s", GASHA_PROGRAM_LANGUAGE_NAME);
#ifdef __cplusplus
	size += sprintf(message + size, " (__cplusplus=%ld)", __cplusplus);
#endif//__cplusplus
#ifdef GASHA_HAS_CPP98
	size += sprintf(message + size, " [C++98]");
#endif//GASHA_HAS_CPP98
#ifdef GASHA_HAS_CPP03
	size += sprintf(message + size, " [C++03]");
#endif//GASHA_HAS_CPP03
#ifdef GASHA_HAS_CPP11
	size += sprintf(message + size, " [C++11]");
#endif//GASHA_HAS_CPP11
#ifdef GASHA_HAS_CPP14
	size += sprintf(message + size, " [C++14]");
#endif//GASHA_HAS_CPP14
	size += sprintf(message + size, "\n");

	size += sprintf(message + size, "\n");

	//【C++11仕様】nullptr
	size += sprintf(message + size, "- nullptr ... ");
#ifdef GASHA_HAS_NULLPTR
	size += sprintf(message + size, "is available.\n");
#else//GASHA_HAS_NULLPTR
#ifdef GASHA_HAS_NULLPTR_PROXY
	size += sprintf(message + size, "is PROXY available.\n");
#else//GASHA_HAS_NULLPTR_PROXY
	size += sprintf(message + size, "is NOT available.\n");
#endif//GASHA_HAS_NULLPTR_PROXY
#endif//GASHA_HAS_NULLPTR

	//【C++11仕様】override
	size += sprintf(message + size, "- override ... ");
#ifdef GASHA_HAS_OVERRIDE
	size += sprintf(message + size, "is available.\n");
#else//GASHA_HAS_OVERRIDE
#ifdef GASHA_HAS_OVERRIDE_DUMMY
	size += sprintf(message + size, "is DUMMY.\n");
#else//GASHA_HAS_OVERRIDE_DUMMY
	size += sprintf(message + size, "is NOT available.\n");
#endif//GASHA_HAS_OVERRIDE_DUMMY
#endif//GASHA_HAS_OVERRIDE

	//【C++11仕様】final
	size += sprintf(message + size, "- final ... ");
#ifdef GASHA_HAS_FINAL
	size += sprintf(message + size, "is available.\n");
#else//GASHA_HAS_FINAL
#ifdef GASHA_HAS_FINAL_DUMMY
	size += sprintf(message + size, "is DUMMY.\n");
#else//GASHA_HAS_FINAL_DUMMY
	size += sprintf(message + size, "is NOT available.\n");
#endif//GASHA_HAS_FINAL_DUMMY
#endif//GASHA_HAS_FINAL

	//【C++11仕様】auto型推論
	size += sprintf(message + size, "- auto ... ");
#ifdef GASHA_HAS_AUTO
	size += sprintf(message + size, "is available.\n");
#else//GASHA_HAS_AUTO
	size += sprintf(message + size, "is NOT available.\n");
#endif//GASHA_HAS_AUTO

	//【C++11仕様】decltype型指定子
	size += sprintf(message + size, "- decltype ... ");
#ifdef GASHA_HAS_DECLTYPE
	size += sprintf(message + size, "is available.\n");
#else//GASHA_HAS_DECLTYPE
	size += sprintf(message + size, "is NOT available.\n");
#endif//GASHA_HAS_DECLTYPE
	
	//【C++11仕様】default/delete宣言
	size += sprintf(message + size, "- default/delete ... ");
#ifdef GASHA_HAS_DEFAULT_AND_DELETE
	size += sprintf(message + size, "is available.\n");
#else//GASHA_HAS_DEFAULT_AND_DELETE
	size += sprintf(message + size, "is NOT available.\n");
#endif//GASHA_HAS_DEFAULT_AND_DELETE

	//【C++11仕様】constexpr
	size += sprintf(message + size, "- constexpr ... ");
#ifdef GASHA_HAS_CONSTEXPR
	size += sprintf(message + size, "is available.\n");
#else//GASHA_HAS_CONSTEXPR
#ifdef GASHA_HAS_CONSTEXPR_DUMMY
	size += sprintf(message + size, "is DUMMY.\n");
#else//GASHA_HAS_CONSTEXPR_DUMMY
	size += sprintf(message + size, "is NOT available.\n");
#endif//GASHA_HAS_CONSTEXPR_DUMMY
#endif//GASHA_HAS_CONSTEXPR

	//【C++11仕様】ユーザー定義リテラル
	size += sprintf(message + size, "- user-defined-literal ... ");
#ifdef GASHA_HAS_USER_DEFINED_LITERAL
	size += sprintf(message + size, "is available.\n");
#else//GASHA_HAS_USER_DEFINED_LITERAL
	size += sprintf(message + size, "is NOT available.\n");
#endif//GASHA_HAS_USER_DEFINED_LITERAL

	//【C++11仕様】ラムダ式
	size += sprintf(message + size, "- lambda expression ... ");
#ifdef GASHA_HAS_LAMBDA_EXPRESSION
	size += sprintf(message + size, "is available.\n");
#else//GASHA_HAS_LAMBDA_EXPRESSION
	size += sprintf(message + size, "is NOT available.\n");
#endif//GASHA_HAS_LAMBDA_EXPRESSION

	//【C++11仕様】範囲に基づくforループ
	size += sprintf(message + size, "- range-based for ... ");
#ifdef GASHA_HAS_RANGE_BASED_FOR
	size += sprintf(message + size, "is available.\n");
#else//GASHA_HAS_RANGE_BASED_FOR
	size += sprintf(message + size, "is NOT available.\n");
#endif//GASHA_HAS_RANGE_BASED_FOR

	//【C++11仕様】強い型付けを持った列挙型（enumの型指定）
	size += sprintf(message + size, "- enum class ... ");
#ifdef GASHA_HAS_ENUM_CLASS
	size += sprintf(message + size, "is available.\n");
#else//GASHA_HAS_ENUM_CLASS
	size += sprintf(message + size, "is NOT available.\n");
#endif//GASHA_HAS_ENUM_CLASS

	//【C++11仕様】可変長引数テンプレート
	size += sprintf(message + size, "- variadic template ... ");
#ifdef GASHA_HAS_VARIADIC_TEMPLATE
	size += sprintf(message + size, "is available.\n");
#else//GASHA_HAS_VARIADIC_TEMPLATE
	size += sprintf(message + size, "is NOT available.\n");
#endif//GASHA_HAS_VARIADIC_TEMPLATE

	//【C++11仕様】テンプレートエイリアス（テンプレートの別名using）
	size += sprintf(message + size, "- template aliases ... ");
#ifdef GASHA_HAS_TEMPLATE_ALIASES
	size += sprintf(message + size, "is available.\n");
#else//GASHA_HAS_TEMPLATE_ALIASES
	size += sprintf(message + size, "is NOT available.\n");
#endif//GASHA_HAS_TEMPLATE_ALIASES

	//【C++11仕様】委譲コンストラクタ
	size += sprintf(message + size, "- delegating constructors ... ");
#ifdef GASHA_HAS_DELEGATING_CONSTRUCTORS
	size += sprintf(message + size, "is available.\n");
#else//GASHA_HAS_DELEGATING_CONSTRUCTORS
	size += sprintf(message + size, "is NOT available.\n");
#endif//GASHA_HAS_DELEGATING_CONSTRUCTORS

	//【C++11仕様】継承コンストラクタ
	size += sprintf(message + size, "- inheriting constructors ... ");
#ifdef GASHA_HAS_INHERITING_CONSTRUCTORS
	size += sprintf(message + size, "is available.\n");
#else//GASHA_HAS_INHERITING_CONSTRUCTORS
	size += sprintf(message + size, "is NOT available.\n");
#endif//GASHA_HAS_INHERITING_CONSTRUCTORS

	//【C++11仕様】一様初期化
	size += sprintf(message + size, "- uniform initializer ... ");
#ifdef GASHA_HAS_UNIFORM_INITIALIZATION
	size += sprintf(message + size, "is available.\n");
#else//GASHA_HAS_UNIFORM_INITIALIZATION
	size += sprintf(message + size, "is NOT available.\n");
#endif//GASHA_HAS_UNIFORM_INITIALIZATION

	//【C++11仕様】初期化リスト
	size += sprintf(message + size, "- initializer list ... ");
#ifdef GASHA_HAS_INITIALIZER_LIST
	size += sprintf(message + size, "is available.\n");
#else//GASHA_HAS_INITIALIZER_LIST
	size += sprintf(message + size, "is NOT available.\n");
#endif//GASHA_HAS_INITIALIZER_LIST

	//【C++11仕様】右辺値参照とムーブセマンティクス
	size += sprintf(message + size, "- rvalue reference & std::move ... ");
#ifdef GASHA_HAS_RVALUE_REFERENCE
	size += sprintf(message + size, "is available.\n");
#else//GASHA_HAS_RVALUE_REFERENCE
	size += sprintf(message + size, "is NOT available.\n");
#endif//GASHA_HAS_RVALUE_REFERENCE

	//【C++11仕様】静的アサーション
	size += sprintf(message + size, "- static_assert ... ");
#ifdef GASHA_HAS_STATIC_ASSERT
	size += sprintf(message + size, "is available.\n");
#else//GASHA_HAS_STATIC_ASSERT
#ifdef GASHA_HAS_STATIC_ASSERT_PROXY
	size += sprintf(message + size, "is available.(substitution)\n");
#else//GASHA_HAS_STATIC_ASSERT_PROXY
	size += sprintf(message + size, "is NOT available.\n");
#endif//GASHA_HAS_STATIC_ASSERT_PROXY
#endif//GASHA_HAS_STATIC_ASSERT

	//【C++11仕様】TLS指定子
	size += sprintf(message + size, "- thread_local ... ");
#ifdef GASHA_HAS_THREAD_LOCAL
	size += sprintf(message + size, "is available.\n");
#else//GASHA_HAS_THREAD_LOCAL
#ifdef GASHA_HAS_THREAD_LOCAL_PROXY
	size += sprintf(message + size, "is PROXY available.\n");
#else//GASHA_HAS_THREAD_LOCAL_PROXY
	size += sprintf(message + size, "is NOT available.\n");
#endif//GASHA_HAS_THREAD_LOCAL_PROXY
#endif//GASHA_HAS_THREAD_LOCAL

	//【C++11仕様】alignas
	size += sprintf(message + size, "- alignas ... ");
#ifdef GASHA_HAS_ALIGNAS
	size += sprintf(message + size, "is available.\n");
#else//GASHA_HAS_ALIGNAS
#ifdef GASHA_HAS_ALIGNAS_PROXY
	size += sprintf(message + size, "is PROXY available.\n");
#else//GASHA_HAS_ALIGNAS_PROXY
	size += sprintf(message + size, "is NOT available.\n");
#endif//GASHA_HAS_ALIGNAS_PROXY
#endif//GASHA_HAS_ALIGNAS

	//【C++11仕様】alignof
	size += sprintf(message + size, "- alignof ... ");
#ifdef GASHA_HAS_ALIGNOF
	size += sprintf(message + size, "is available.\n");
#else//GASHA_HAS_ALIGNOF
#ifdef GASHA_HAS_ALIGNOF_PROXY
	size += sprintf(message + size, "is PROXY available.\n");
#else//GASHA_HAS_ALIGNOF_PROXY
	size += sprintf(message + size, "is NOT available.\n");
#endif//GASHA_HAS_ALIGNOF_PROXY
#endif//GASHA_HAS_ALIGNOF

	size += sprintf(message + size, "\n");
	
	//_aligned_malloc 
	//※C++11仕様ではなく、コンパイラ独自仕様の共通化
	size += sprintf(message + size, "- _aligned_malloc ... ");
#ifdef GASHA_HAS_ALIGNED_MALLOC
	size += sprintf(message + size, "is available.\n");
#else//GASHA_HAS_ALIGNED_MALLOC
#ifdef GASHA_HAS_ALIGNED_MALLOC_PROXY
	size += sprintf(message + size, "is available with commonized.\n");
#else//GASHA_HAS_ALIGNED_MALLOC_PROXY
	size += sprintf(message + size, "is NOT available.\n");
#endif//GASHA_HAS_ALIGNED_MALLOC_PROXY
#endif//GASHA_HAS_ALIGNED_MALLOC
	
	//_aligned_free
	//※C++11仕様ではなく、コンパイラ独自仕様の共通化
	size += sprintf(message + size, "- _aligned_free ... ");
#ifdef GASHA_HAS_ALIGNED_FREE
	size += sprintf(message + size, "is available.\n");
#else//GASHA_HAS_ALIGNED_FREE
#ifdef GASHA_HAS_ALIGNED_FREE_PROXY
	size += sprintf(message + size, "is available with commonized.\n");
#else//GASHA_HAS_ALIGNED_FREE_PROXY
	size += sprintf(message + size, "is NOT available.\n");
#endif//GASHA_HAS_ALIGNED_FREE_PROXY
#endif//GASHA_HAS_ALIGNED_FREE

	//no_inline
	//※C++11仕様ではなく、コンパイラ独自仕様の共通化
	size += sprintf(message + size, "- no_inline ... ");
#ifdef GASHA_HAS_NO_INLINE
	size += sprintf(message + size, "is available.\n");
#else//GASHA_HAS_NO_INLINE
#ifdef GASHA_HAS_NO_INLINE_PROXY
	size += sprintf(message + size, "is available with commonized.\n");
#else//GASHA_HAS_NO_INLINE_PROXY
	size += sprintf(message + size, "is NOT available.\n");
#endif//GASHA_HAS_NO_INLINE_PROXY
#endif//GASHA_HAS_NO_INLINE

	//always_inline
	//※C++11仕様ではなく、コンパイラ独自仕様の共通化
	size += sprintf(message + size, "- always_inline ... ");
#ifdef GASHA_HAS_ALWAYS_INLINE
	size += sprintf(message + size, "is available.\n");
#else//GASHA_HAS_ALWAYS_INLINE
#ifdef GASHA_HAS_ALWAYS_INLINE_PROXY
	size += sprintf(message + size, "is available with commonized.\n");
#else//GASHA_HAS_ALWAYS_INLINE_PROXY
	size += sprintf(message + size, "is NOT available.\n");
#endif//GASHA_HAS_ALWAYS_INLINE_PROXY
#endif//GASHA_HAS_ALWAYS_INLINE

	size += sprintf(message + size, "\n");
	size += sprintf(message + size, "------------------------------------------------------------------------------\n");
	size += sprintf(message + size, "[ Build-configuratoin ]\n");

	//ビルド設定
	
	size += sprintf(message + size, "\n");
	size += sprintf(message + size, "Configuration name: %s\n", GASHA_BUILD_CONFIG_NAME);
	size += sprintf(message + size, "\n");

	//#define GASHA_BUILD_CONFIG_IS_DEBUG//フルデバッグ設定
	//#define GASHA_BUILD_CONFIG_IS_DEBUG_MODERATE//プログラム開発向け設定
	//#define GASHA_BUILD_CONFIG_IS_DEBUG_OPT//コンテンツ制作・QA向け設定
	//#define GASHA_BUILD_CONFIG_IS_REGRESSION_TEST//自動回帰テスト向け設定
	//#define GASHA_BUILD_CONFIG_IS_LOCAL_RELEASE//製品テスト向け設定
	//#define GASHA_BUILD_CONFIG_IS_RELEASE//製品向け設定

	size += sprintf(message + size, "- debug feature ... ");
#ifdef GASHA_HAS_DEBUG_FEATURE//デバッグ機能有効化
	size += sprintf(message + size, "is available.");
#else//GASHA_HAS_DEBUG_FEATURE
	size += sprintf(message + size, "is NOT available.");
#endif//GASHA_HAS_DEBUG_FEATURE
#ifdef GASHA_HAS_VERBOSE_DEBUG//冗長デバッグ機能有効化
	size += sprintf(message + size, "(VERBOSE)");
#endif//GASHA_HAS_VERBOSE_DEBUG
	size += sprintf(message + size, "\n");

	size += sprintf(message + size, "- assertion ... ");
#ifdef GASHA_ASSERTION_IS_ENABLED//アサーション有効化
	size += sprintf(message + size, "is enabled.");
#else//GASHA_ASSERTION_IS_ENABLED
	size += sprintf(message + size, "is DISABLED.");
#endif//GASHA_ASSERTION_IS_ENABLED
	size += sprintf(message + size, "\n");

	size += sprintf(message + size, "- optimized ... ");
#ifdef GASHA_NO_OPTIMIZED//最適化なし
	size += sprintf(message + size, "none.");
#endif//GASHA_NO_OPTIMIZED
#ifdef GASHA_OPTIMIZED_MODERATELY//適度に最適化
	size += sprintf(message + size, "moderate.");
#endif//GASHA_OPTIMIZED_MODERATELY
#ifdef GASHA_OPTIMIZED//最大限の最適化
	size += sprintf(message + size, "MAX.");
#endif//GASHA_OPTIMIZED
	size += sprintf(message + size, "\n");
	
	size += sprintf(message + size, "- file-system ... ");
#ifdef GASHA_FILE_SYSTEM_IS_ROM//ROM専用ファイルシステム
	size += sprintf(message + size, "for ROM.");
#endif//GASHA_FILE_SYSTEM_IS_ROM
#ifdef GASHA_FILE_SYSTEM_IS_FLEXIBLE//ローカル／ROM切り替えファイルシステム（ローカルデータで設定）
	size += sprintf(message + size, "flexible(ROM or local-PC).");
#endif//GASHA_FILE_SYSTEM_IS_FLEXIBLE
	size += sprintf(message + size, "\n");

	size += sprintf(message + size, "- unit-test ... ");
#ifdef GASHA_UNITE_TEST_ENABLED//ユニットテスト（の仕組みが）有効
	size += sprintf(message + size, "is available.");
#else//GASHA_UNITE_TEST_ENABLED
	size += sprintf(message + size, "is NOT available.");
#endif//GASHA_UNITE_TEST_ENABLED
#ifdef GASHA_IS_REGRESSION_TEST//回帰テストモード有効
	size += sprintf(message + size, " (for regression-test)");
#endif//GASHA_IS_REGRESSION_TEST
	size += sprintf(message + size, "\n");

	size += sprintf(message + size, "- symbols ... ");
#ifdef GASHA_HAS_SYNBOMS//シンボル情報あり
	size += sprintf(message + size, "contained.");
#endif//GASHA_HAS_SYNBOMS
#ifdef GASHA_IS_STRIPPED_SYMBOLS//シンボル情報なし
	size += sprintf(message + size, "STRIPPED.");
#endif//GASHA_IS_STRIPPED_SYMBOLS
	size += sprintf(message + size, "\n");
	
	size += sprintf(message + size, "\n");
	size += sprintf(message + size, "------------------------------------------------------------------------------\n");

	return !has_error;
}

GASHA_NAMESPACE_END;//ネームスペース：終了

// End of file
