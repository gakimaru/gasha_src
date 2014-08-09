﻿//--------------------------------------------------------------------------------
// i_allocator_adapter.cpp
// アロケータアダプタインターフェース【関数／実体部】
//
// Gakimaru's standard library for C++ - GASHA
//   Copyright (c) 2014 Itagaki Mamoru
//   Released under the MIT license.
//     https://github.com/gakimaru/gasha/blob/master/LICENSE
//--------------------------------------------------------------------------------

#include <gasha/i_allocator_adapter.inl>//メモリアロケータインターフェース【インライン関数／テンプレート関数定義部】

GASHA_NAMESPACE_BEGIN;//ネームスペース：開始

//--------------------------------------------------------------------------------
//アロケータアダプタインターフェースクラス

//デストラクタ
iAllocatorAdapter::~iAllocatorAdapter()
{
	m_isInstanced = false;
}

GASHA_NAMESPACE_END;//ネームスペース：終了

// End of file
