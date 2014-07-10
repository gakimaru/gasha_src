﻿//--------------------------------------------------------------------------------
// poly_allocator.cpp
// 多態アロケータ【関数定義部】
//
// Gakimaru's researched and standard library for C++ - GASHA
//   Copyright (c) 2014 Itagaki Mamoru
//   Released under the MIT license.
//     https://github.com/gakimaru/gasha/blob/master/LICENSE
//--------------------------------------------------------------------------------

#include <gasha/poly_allocator.h>//多態アロケータ【インライン関数／テンプレート関数定義部】

#include <gasha/allocator_common.h>//メモリアロケータ共通設定

GASHA_NAMESPACE_BEGIN;//ネームスペース：開始

//--------------------------------------------------------------------------------
//多態アロケータクラス

//コールバック
void polyAllocator::callbackAtNew(void *p, std::size_t size, const GASHA_ newMethod_t method)
{
#if defined(GASHA_HAS_DEBUG_FEATURE) && defined(GASHA_ENABLE_POLY_ALLOCATOR)
	if (m_adapter && m_observer && static_cast<bool>(m_observer->m_atDelete))
		m_observer->m_atNew(*m_adapter, p, size, m_align, method, m_debugInfo);
#endif//GASHA_HAS_DEBUG_FEATURE
}
//delete時のコールバック
void polyAllocator::callbackAtDelete(void *p, const GASHA_ deleteMethod_t method)
{
#if defined(GASHA_HAS_DEBUG_FEATURE) && defined(GASHA_ENABLE_POLY_ALLOCATOR)
	if (m_adapter && m_observer && static_cast<bool>(m_observer->m_atDelete))
		m_observer->m_atDelete(*m_adapter, p, method, m_debugInfo);
#endif//GASHA_HAS_DEBUG_FEATURE
}
//アロケータアダプター変更時のコールバック
void polyAllocator::callbackAtChangeAllocator(const GASHA_ IAllocatorAdapter& adapter, const GASHA_ IAllocatorAdapter& next_adapter)
{
#if defined(GASHA_HAS_DEBUG_FEATURE) && defined(GASHA_ENABLE_POLY_ALLOCATOR)
	if (m_adapter && m_observer && static_cast<bool>(m_observer->m_atChangeAllocator))
		m_observer->m_atChangeAllocator(adapter, next_adapter);
#endif//GASHA_HAS_DEBUG_FEATURE
}
//アロケータアダプター復帰時のコールバック
void polyAllocator::callbackAtReturnAllocator(const GASHA_ IAllocatorAdapter& adapter, const GASHA_ IAllocatorAdapter& prev_adapter)
{
#if defined(GASHA_HAS_DEBUG_FEATURE) && defined(GASHA_ENABLE_POLY_ALLOCATOR)
	if (m_observer && static_cast<bool>(m_observer->m_atReturnAllocator))
		m_observer->m_atReturnAllocator(adapter, prev_adapter);
#endif//GASHA_HAS_DEBUG_FEATURE
}

//標準アロケータアダプターの強制初期化
void polyAllocator::initlaizeStdAllocatorAdapter()
{
#ifdef GASHA_ENABLE_POLY_ALLOCATOR
	new(&m_stdAllocator)GASHA_ stdAllocator<>();
	new(&m_stdAllocatorAdapter)GASHA_ allocatorAdapter<GASHA_ stdAllocator<>>(m_stdAllocator.adapter());
	if (!m_adapter)
		m_adapter = &m_stdAllocatorAdapter;
#endif//GASHA_ENABLE_POLY_ALLOCATOR
}

//--------------------------------------------------------------------------------
//多態アロケータ：静的メンバー変数のインスタンス

#ifdef GASHA_ENABLE_POLY_ALLOCATOR
GASHA_ stdAllocator<> polyAllocator::m_stdAllocator;//標準アロケータ
GASHA_ allocatorAdapter<GASHA_ stdAllocator<>> polyAllocator::m_stdAllocatorAdapter(polyAllocator::m_stdAllocator.adapter());//標準アロケータアダプター
thread_local GASHA_ IAllocatorAdapter* polyAllocator::m_adapter = nullptr;//現在のアロケータ
thread_local const GASHA_ debugAllocationObserver* polyAllocator::m_observer = nullptr;//現在の観察者
thread_local std::size_t polyAllocator::m_align = DEFAULT_ALIGN;//現在のアラインメントサイズ
thread_local const GASHA_ debugAllocationInfo* polyAllocator::m_debugInfo = nullptr;//現在のデバッグ情報
#else//GASHA_ENABLE_POLY_ALLOCATOR
GASHA_ IAllocatorAdapter* m_dummyAdapter = nullptr;//アロケータアダプターダミー
#endif//GASHA_ENABLE_POLY_ALLOCATOR

GASHA_NAMESPACE_END;//ネームスペース：終了

// End of file
