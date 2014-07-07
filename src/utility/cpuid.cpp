//--------------------------------------------------------------------------------
// cpuid.cpp
// CPU情報（x86系CPU用）【関数／実体定義部】
//
// Gakimaru's researched and standard library for C++ - GASHA
//   Copyright (c) 2014 Itagaki Mamoru
//   Released under the MIT license.
//     https://github.com/gakimaru/gasha/blob/master/LICENSE
//--------------------------------------------------------------------------------

#include <gasha/cpuid.h>//CPU情報（x86系CPU用）

#include <memory.h>//memcpy()

GASHA_NAMESPACE_BEGIN;//ネームスペース：開始

#ifdef GASHA_IS_X86
#ifdef GASHA_IS_GCC
void __cpuid(int cpu_info[4], const int type)
{
	const std::uint32_t _type = type;
	std::uint32_t _cpu_info[4] = { 0, 0, 0, 0 };
	__asm__ __volatile__(
		"cpuid"
		: "=&a" (_cpu_info[0]),
		  "=&b" (_cpu_info[1]),
		  "=&c" (_cpu_info[2]),
		  "=&d" (_cpu_info[3])
		: "0"   (_type)
	);
	for(int i = 0; i < 4; ++i)
		cpu_info[i] = static_cast<int>(_cpu_info[i]);
}
std::uint64_t _xgetbv(const std::uint32_t xcr)
{
	std::uint32_t lo, hi;
	__asm__ __volatile__(
		"xgetbv" 
		: "=a"(lo),
		  "=d"(hi)
		: "c"(xcr)
	);
	return (static_cast<std::uint64_t>(hi) << 32) | static_cast<std::uint64_t>(lo);
}
#define _XCR_XFEATURE_ENABLED_MASK 0
#endif//GASHA_IS_GCC
#endif//GASHA_IS_X86

GASHA_NAMESPACE_END;//ネームスペース：終了

// End of file
