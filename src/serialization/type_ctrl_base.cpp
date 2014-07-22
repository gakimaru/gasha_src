//--------------------------------------------------------------------------------
// serialization/type_ctrl_base.cpp
// シリアライズ/型操作基底クラス【関数／実体定義部】
//
// Gakimaru's researched and standard library for C++ - GASHA
//   Copyright (c) 2014 Itagaki Mamoru
//   Released under the MIT license.
//     https://github.com/gakimaru/gasha/blob/master/LICENSE
//--------------------------------------------------------------------------------

#include <gasha/serialization/type_ctrl_base.inl>//シリアライズ/型操作基底クラス【インライン関数／テンプレート関数定義部】

#include <cstring>//std::std::memcpy(), std::memset()

GASHA_NAMESPACE_BEGIN;//ネームスペース：開始

//--------------------------------------------------------------------------------
//シリアライズ
namespace serialization
{
	//--------------------------------------------------------------------------------
	//シリアライズ/型操作基底クラス
	//--------------------------------------------------------------------------------

	//メモリへコピー
	std::size_t typeCtrlBase::toMem(void* mem, const std::size_t mem_size, const void* value_p, const std::size_t value_size)
	{
		if (mem_size < value_size)
		{
			//書き込み先のサイズの方が小さい場合
			//※書き込み先のサイズ分だけコピーする
			std::memcpy(mem, value_p, mem_size);
		}
		else if (mem_size > value_size)
		{
			//書き込み先のサイズの方が大きい場合
			//※後方をゼロクリア
			std::memcpy(mem, value_p, value_size);
			std::memset(reinterpret_cast<char*>(mem)+value_size, 0, mem_size - value_size);
		}
		else//if (mem_size == value_size)
		{
			//サイズが一致する場合
			//※そのままコピーするだけ
			std::memcpy(mem, value_p, mem_size);
		}
		return mem_size;
	}
	//※エンディアン調整版
	std::size_t typeCtrlBase::toMemWithConv(void* mem, const std::size_t mem_size, const void* value_p, const std::size_t value_size)
	{
	#ifdef GASHA_IS_BIG_ENDIAN
		if (mem_size < value_size)
		{
			//書き込み先のサイズの方が小さい場合
			//※書き込み先のサイズ分だけ後方をコピーする
			std::memcpy(mem, reinterpret_cast<const char*>(value_p)+(value_size - mem_size), mem_size);
		}
		else if (mem_size > value_size)
		{
			//書き込み先のサイズの方が大きい場合
			//※後詰めで前方をゼロクリア
			std::memcpy(reinterpret_cast<char*>(mem)+value_size, value_p, value_size);
			std::memset(mem, 0, mem_size - value_size);
		}
		else//if (mem_size == value_size)
		{
			//サイズが一致する場合
			//※そのままコピーするだけ
			std::memcpy(mem, value_p, mem_size);
		}
		return mem_size;
	#else//GASHA_IS_BIG_ENDIAN
		return toMem(mem, mem_size, value_p, value_size);
	#endif//GASHA_IS_BIG_ENDIAN
	}

	//文字列へ変換
	std::size_t typeCtrlBase::toStr(char* str, const std::size_t str_max, const void* value_p, const std::size_t value_size)
	{
		std::size_t used = 0;
		std::size_t remain = str_max;
		char* write_p = str;
		if (remain > 3)
		{
			*(write_p++) = '[';
			++used;
			--remain;
		}
		const unsigned char* read_p = reinterpret_cast<const unsigned char*>(value_p);
		for (unsigned int i = 0; i < value_size && remain > 5 + 2; ++i)
		{
			const unsigned char c = *(read_p++);
			const unsigned char hi = c >> 4;
			const unsigned char lo = c & 0xf;
			if (i != 0)
			{
				*(write_p++) = ',';
				++used;
				--remain;
			}
			*(write_p++) = '0';
			*(write_p++) = 'x';
			*(write_p++) = hi >= 10 ? 'a' + hi - 10 : '0' + hi;
			*(write_p++) = lo >= 10 ? 'a' + lo - 10 : '0' + lo;
			used += 4;
			remain -= 4;
		}
		if (remain > 2)
		{
			*(write_p++) = ']';
			++used;
			--remain;
		}
		if (remain > 1)
			*(write_p) = '\0';
		return used;
	}
	//文字列から変換
	std::size_t typeCtrlBase::fromStr(const char* str, const std::size_t str_size, void* value_p, const std::size_t value_size_max)
	{
		//未実装
		return 0;
	}

}//namespace serialization

GASHA_NAMESPACE_END;//ネームスペース：終了

// End of file
