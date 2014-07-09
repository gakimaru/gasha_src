//--------------------------------------------------------------------------------
// 【テンプレートライブラリ】
// lf_mono_allocator.cpp
// ロックフリー単一アロケータ【関数定義部】
//
// ※クラスのインスタンス化が必要な場所でインクルード。
// ※基本的に、ヘッダーファイル内でのインクルード禁止。
// 　（コンパイル・リンク時間への影響を気にしないならOK）
// ※明示的なインスタンス化を避けたい場合は、ヘッダーファイルと共にインクルード。
// 　（この場合、実際に使用するメンバー関数しかインスタンス化されないので、対象クラスに不要なインターフェースを実装しなくても良い）
//
// Gakimaru's researched and standard library for C++ - GASHA
//   Copyright (c) 2014 Itagaki Mamoru
//   Released under the MIT license.
//     https://github.com/gakimaru/gasha/blob/master/LICENSE
//--------------------------------------------------------------------------------

#include <gasha/lf_mono_allocator.inl>//ロックフリー単一アロケータ【インライン関数／テンプレート関数定義部】

#include <gasha/type_traits.inl>//型特性ユーティリティ：toStr()

#include <utility>//C++11 std::move

#include <assert.h>//assert()

//【VC++】ワーニング設定を退避
#pragma warning(push)

//【VC++】例外を無効化した状態で <new> をインクルードすると、warning C4530 が発生する
//  warning C4530: C++ 例外処理を使っていますが、アンワインド セマンティクスは有効にはなりません。/EHsc を指定してください。
#pragma warning(disable: 4530)//C4530を抑える

#include <new>//配置new,配置delete用

//【VC++】sprintf を使用すると、error C4996 が発生する
//  error C4996: 'sprintf': This function or variable may be unsafe. Consider using strncpy_fast_s instead. To disable deprecation, use _CRT_SECURE_NO_WARNINGS. See online help for details.
#pragma warning(disable: 4996)//C4996を抑える

GASHA_NAMESPACE_BEGIN;//ネームスペース：開始

//--------------------------------------------------------------------------------
//ロックフリー単一アロケータクラス

//メモリ確保
void* lfMonoAllocator::alloc(const std::size_t size, const std::size_t align)
{
	if (m_size.load() > 0)//既にメモリ確保中なら失敗
		return nullptr;
	//サイズが0バイトならサイズを1に、アラインメントを0にする
	//※要求サイズが0でも必ずメモリを割り当てる点に注意（ただし、アラインメントは守らない）
	const std::size_t _size = size == 0 ? 1 : size;
	const std::size_t _align = size == 0 ? 0 : align;
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
	if(!m_size.compare_exchange_strong(now_size, alloc_size))
		return nullptr;//確保失敗（他のスレッドが割り込んでいる）

	//空き領域を確保
	return reinterpret_cast<void*>(new_ptr);
}

//デバッグ情報作成
std::size_t lfMonoAllocator::debugInfo(char* message)
{
#ifdef GASHA_HAS_DEBUG_FEATURE
	std::size_t size = 0;
	size += sprintf(message + size, "----- Debug Info for lfMonoAllocator -----\n");
	size += sprintf(message + size, "buffRef=%p, maxSize=%d, size=%d, remain=%d, isAllocated=%s\n", m_buffRef, maxSize(), this->size(), remain(), toStr(isAllocated()));
	size += sprintf(message + size, "----------\n");
	return size;
#else//GASHA_HAS_DEBUG_FEATURE
	message[0] = '\0';
	return 0;
#endif//GASHA_HAS_DEBUG_FEATURE
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
