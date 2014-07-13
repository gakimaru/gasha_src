//--------------------------------------------------------------------------------
// lf_mono_allocator.cpp
// ロックフリー単一アロケータ【関数／実体定義部】
//
// Gakimaru's researched and standard library for C++ - GASHA
//   Copyright (c) 2014 Itagaki Mamoru
//   Released under the MIT license.
//     https://github.com/gakimaru/gasha/blob/master/LICENSE
//--------------------------------------------------------------------------------

#include <gasha/lf_mono_allocator.inl>//ロックフリー単一アロケータ【インライン関数／テンプレート関数定義部】

#include <gasha/type_traits.inl>//型特性ユーティリティ：toStr()

#include <cassert>//assert()

//【VC++】ワーニング設定を退避
#pragma warning(push)

//【VC++】sprintf を使用すると、error C4996 が発生する
//  error C4996: 'sprintf': This function or variable may be unsafe. Consider using strncpy_fast_s instead. To disable deprecation, use _CRT_SECURE_NO_WARNINGS. See online help for details.
#pragma warning(disable: 4996)//C4996を抑える

GASHA_NAMESPACE_BEGIN;//ネームスペース：開始

//--------------------------------------------------------------------------------
//ロックフリー単一アロケータクラス

//メモリ確保
void* lfMonoAllocator::alloc(const std::size_t size, const std::size_t align)
{
	//サイズが0バイトならサイズを1に、アラインメントを0にする
	//※要求サイズが0でも必ずメモリを割り当てる点に注意（ただし、アラインメントは守らない）
	const std::size_t _size = size == 0 ? 1 : size;
	const std::size_t _align = size == 0 ? 0 : align;
	while (m_size.load() == 0)
	{
		//サイズとアラインメントをチェック
		char* new_ptr = adjustAlign(m_buffRef, _align);
		const std::size_t padding_size = new_ptr - m_buffRef;
		const size_type alloc_size = static_cast<size_type>(padding_size + _size);
		if (alloc_size > m_maxSize)
		{
		#ifdef GASHA_LF_MONO_ALLOCATOR_ENABLE_ASSERTION
			static const bool NOT_ENOUGH_SPACE = false;
			assert(NOT_ENOUGH_SPACE);
		#endif//GASHA_LF_MONO_ALLOCATOR_ENABLE_ASSERTION
			return nullptr;
		}

		//使用中のサイズを更新
		size_type now_size = 0;
		if (m_size.compare_exchange_weak(now_size, alloc_size))
		{
			//空き領域を確保
			return reinterpret_cast<void*>(new_ptr);
		}
	}
	return nullptr;
}

//デバッグ情報作成
std::size_t lfMonoAllocator::debugInfo(char* message)
{
	std::size_t size = 0;
	size += std::sprintf(message + size, "----- Debug Info for lfMonoAllocator -----\n");
	size += std::sprintf(message + size, "buff=%p, maxSize=%d, size=%d, remain=%d, isAllocated=%s\n", m_buffRef, maxSize(), this->size(), remain(), toStr(isAllocated()));
	size += std::sprintf(message + size, "------------------------------------------\n");
	return size;
}

//メモリ解放（共通処理）
bool lfMonoAllocator::_free(void* p)
{
	//バッファの使用中サイズを更新
	m_size.store(0);
	return true;
}

//【VC++】ワーニング設定を復元
#pragma warning(pop)

GASHA_NAMESPACE_END;//ネームスペース：終了

// End of file
