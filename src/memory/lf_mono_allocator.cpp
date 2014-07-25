//--------------------------------------------------------------------------------
// lf_mono_allocator.cpp
// ロックフリー単一アロケータ【関数／実体定義部】
//
// Gakimaru's standard library for C++ - GASHA
//   Copyright (c) 2014 Itagaki Mamoru
//   Released under the MIT license.
//     https://github.com/gakimaru/gasha/blob/master/LICENSE
//--------------------------------------------------------------------------------

#include <gasha/lf_mono_allocator.inl>//ロックフリー単一アロケータ【インライン関数／テンプレート関数定義部】

#include <gasha/type_traits.h>//型特性ユーティリティ：toStr()
#include <gasha/string.h>//文字列処理：spprintf()
#include <gasha/simple_assert.h>//シンプルアサーション

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
	#ifdef GASHA_LF_MONO_ALLOCATOR_ENABLE_ASSERTION
		GASHA_SIMPLE_ASSERT(alloc_size <= m_maxSize, "lfMonoAllocator is not enough space.");
	#endif//GASHA_LF_MONO_ALLOCATOR_ENABLE_ASSERTION
		if (alloc_size > m_maxSize)
			return nullptr;

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
std::size_t lfMonoAllocator::debugInfo(char* message, const std::size_t max_size) const
{
	std::size_t message_len = 0;
	GASHA_ spprintf(message, max_size, message_len, "----- Debug-info for lfMonoAllocator -----\n");
	GASHA_ spprintf(message, max_size, message_len, "buff=%p, maxSize=%d, size=%d, remain=%d, isAllocated=%s\n", m_buffRef, maxSize(), this->size(), remain(), toStr(isAllocated()));
	GASHA_ spprintf(message, max_size, message_len, "------------------------------------------");//最終行改行なし
	return message_len;
}

//メモリ解放（共通処理）
bool lfMonoAllocator::_free(void* p)
{
	//バッファの使用中サイズを更新
	m_size.store(0);
	return true;
}

GASHA_NAMESPACE_END;//ネームスペース：終了

// End of file
