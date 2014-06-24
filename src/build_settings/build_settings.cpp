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

#ifdef GASHA_IS_VC
#include <intrin.h>//__cpuid()
#endif//GASHA_IS_VC

#ifdef GASHA_USE_AVX
#include <immintrin.h>//AVX
#endif//GASHA_USE_AVX

#include <stdio.h>//printf(), fprintf()
#include <string.h>//strncpy_s()
#include <assert.h>//assert()
#include <stdlib.h>//abort()

GASHA_NAMESPACE_BEGIN;//ネームスペース：開始

//--------------------
//ビルド設定が実行環境に適合するかチェック
//※ダメな要素があった場合、即abort
void checkBuildSettings()
{
	printf("------------------------------------------------------------------------------\n");
	printf("Check build-settings suitable for runtime environment.\n");
	printf("\n");
	bool is_error = false;
//Visual C++ の場合のチェック
#ifdef GASHA_IS_VC
	char cpu_info_str[12];
	int cpu_info[4] = {-1, -1, -1, -1};
	__cpuid(cpu_info, 0);//CPU情報取得：Type0
	strncpy_s(cpu_info_str, sizeof(cpu_info_str), reinterpret_cast<const char*>(&cpu_info[1]), sizeof(cpu_info_str) - 1);
	printf("cpu_string=\"%s\"\n", cpu_info_str);
	__cpuid(cpu_info, 1);//CPU情報取得：Type1
	const bool sse_is_supported = (cpu_info[3] & (1 << 25)) || false;//SSE対応
	const bool sse2_is_supported = (cpu_info[3] & (1 << 26)) || false;//SSE2対応
	const bool sse3_is_supported = (cpu_info[2] & (1 << 9)) || false;//SSE3対応
	const bool sse4a_is_supported = (cpu_info[2] & (1 << 6)) || false;//SSE4a対応
	const bool sse4_1_is_supported = (cpu_info[2] & (1 << 19)) || false;//SSE4.1対応
	const bool sse4_2_is_supported = (cpu_info[2] & (1 << 20)) || false;//SSE4.2対応
	const bool poopcnt_is_supported = (cpu_info[2] & (1 << 23)) || false;//POPCNT対応
	const bool aes_is_supported = (cpu_info[2] & (1 << 25)) || false;//AES対応
	const bool xsave_is_supported = (cpu_info[2] & (1 << 26)) || false;//XSAVE対応
	const bool osxsave_is_supported = (cpu_info[2] & (1 << 27)) || false;//OSXSAVE対応
	const bool _avx_is_supported = (cpu_info[2] & (1 << 28)) || false;//AVX対応（仮）
	bool avx_is_supported = false;//AVX対応
#ifdef GASHA_USE_SSE
	if(!sse_is_supported)
	{
		is_error = true;
		fprintf(stderr, "[NG] SSE is NOT supported!\n");
	}
	printf("[OK] SSE is supported.\n");
#else//GASHA_USE_SSE
	printf("[--] SSE is not used.\n");
#endif//GASHA_USE_SSE
#ifdef GASHA_USE_SSE2
	if(!sse2_is_supported)
	{
		is_error = true;
		fprintf(stderr, "[NG] SSE2 is NOT supported!\n");
	}
	printf("[OK] SSE2 is supported.\n");
#else//GASHA_USE_SSE2
	printf("[--] SSE2 is not used.\n");
#endif//GASHA_USE_SSE2
#ifdef GASHA_USE_SSE3
	if(!sse3_is_supported)
	{
		is_error = true;
		fprintf(stderr, "[NG] SSE3 is NOT supported!\n");
	}
	printf("[OK] SSE3 is supported.\n");
#else//GASHA_USE_SSE3
	printf("[--] SSE3 is not used.\n");
#endif//GASHA_USE_SSE3
#ifdef GASHA_USE_SSE4A
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
	if(!sse4_1_is_supported)
	{
		is_error = true;
		fprintf(stderr, "[NG] SSE4.1 is NOT supported!\n");
	}
	printf("[OK] SSE4.1 is NOT supported.\n");
#else//GASHA_USE_SSE4_1
	printf("[--] SSE4.1 is not used.\n");
#endif//GASHA_USE_SSE4_1
#ifdef GASHA_USE_SSE4_2
	if(!sse4_2_is_supported)
	{
		is_error = true;
		fprintf(stderr, "[NG] SSE4.2 is NOT supported!\n");
	}
	printf("[OK] SSE4.2 is supported.\n");
#else//GASHA_USE_SSE4_2
	printf("[--] SSE4.2 is not used.\n");
#endif//GASHA_USE_SSE4_2
#ifdef GASHA_USE_POPCNT
	if(!poopcnt_is_supported)
	{
		is_error = true;
		fprintf(stderr, "[NG] POPCNT is NOT supported!\n");
	}
	printf("[OK] POPCNT is supported.\n");
#else//GASHA_USE_POPCNT
	printf("[--] POPCNT is not used.\n");
#endif//GASHA_USE_POPCNT
#ifdef GASHA_USE_AES
	if(!aes_is_supported)
	{
		is_error = true;
		fprintf(stderr, "[NG] AES is NOT supported!\n");
	}
	printf("[OK] AES is supported.\n");
#else//GASHA_USE_AES
	printf("[--] AES is not used.\n");
#endif//GASHA_USE_AES
#ifdef GASHA_USE_AVX
	if(osxsave_is_supported && _avx_is_supported)
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
#endif//GASHA_IS_VC

//GCCの場合のチェック
//※未作成
#ifdef GASHA_IS_GCC
#ifdef GASHA_USE_SSE
	printf("[OK?] SSE is supported?\n");
#else//GASHA_USE_SSE
	printf("[--] SSE is not used.\n");
#endif//GASHA_USE_SSE
#ifdef GASHA_USE_SSE2
	printf("[OK?] SSE2 is supported?\n");
#else//GASHA_USE_SSE2
	printf("[--] SSE2 is not used.\n");
#endif//GASHA_USE_SSE2
#ifdef GASHA_USE_SSE3
	printf("[OK?] SSE3 is supported?\n");
#else//GASHA_USE_SSE3
	printf("[--] SSE3 is not used.\n");
#endif//GASHA_USE_SSE3
#ifdef GASHA_USE_SSE4A
	printf("[OK?] SSE4a is NOT supported?\n");
#else//GASHA_USE_SSE4A
	printf("[--] SSE4a is not used.\n");
#endif//GASHA_USE_SSE4A
#ifdef GASHA_USE_SSE4_1
	printf("[OK?] SSE4.1 is NOT supported?\n");
#else//GASHA_USE_SSE4_1
	printf("[--] SSE4.1 is not used.\n");
#endif//GASHA_USE_SSE4_1
#ifdef GASHA_USE_SSE4_2
	printf("[OK?] SSE4.2 is supported?\n");
#else//GASHA_USE_SSE4_2
	printf("[--] SSE4.2 is not used.\n");
#endif//GASHA_USE_SSE4_2
#ifdef GASHA_USE_POPCNT
	printf("[OK?] POPCNT is supported?\n");
#else//GASHA_USE_POPCNT
	printf("[--] POPCNT is not used.\n");
#endif//GASHA_USE_POPCNT
#ifdef GASHA_USE_AES
	printf("[OK?] AES is supported?\n");
#else//GASHA_USE_AES
	printf("[--] AES is not used.\n");
#endif//GASHA_USE_AES
#ifdef GASHA_USE_AVX
	printf("[OK?] AVX is supported?\n");
#else//GASHA_USE_AVX
	printf("[--] AVX is not used.\n");
#endif//GASHA_USE_AVX
#ifdef GASHA_USE_AVX2
	printf("[OK?] AVX2 is supported?\n");
#else//GASHA_USE_AVX2
	printf("[--] AVX2 is not used.\n");
#endif//GASHA_USE_AVX2
#endif//GASHA_IS_GCC

	printf("------------------------------------------------------------------------------\n");
	
	if (is_error)
	{
		printf("Is NOT suitable!\n");
		printf("Abort this program.\n");
		abort();
	}
}

GASHA_NAMESPACE_END;//ネームスペース：終了

// End of file
