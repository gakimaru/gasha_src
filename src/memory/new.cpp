//--------------------------------------------------------------------------------
// new.cpp
// 多態アロケータ対応標準new/delete【宣言部】
//
// Gakimaru's researched and standard library for C++ - GASHA
//   Copyright (c) 2014 Itagaki Mamoru
//   Released under the MIT license.
//     https://github.com/gakimaru/gasha/blob/master/LICENSE
//--------------------------------------------------------------------------------

#include <gasha/new.inl>//多態アロケータ対応標準new/delete【インライン関数／テンプレート関数定義部】

#include <gasha/poly_allocator.h>//多態アロケータ

//【VC++】例外を無効化した状態で <new> をインクルードすると、warning C4530 が発生する
//  warning C4530: C++ 例外処理を使っていますが、アンワインド セマンティクスは有効にはなりません。/EHsc を指定してください。
#pragma warning(disable: 4530)//C4530を抑える

#include <new>//配置new/配置delete

//【VC++】例外を無効化した状態で例外つきのnewをオーバーロードすると、warning C4290 が発生する
//warning C4290: C++ の例外の指定は無視されます。関数が __declspec(nothrow) でないことのみ表示されます。
#pragma warning(disable: 4290)//C4290を抑える

GASHA_NAMESPACE_BEGIN;//ネームスペース：開始

//なにもなし

GASHA_NAMESPACE_END;//ネームスペース：終了

//--------------------------------------------------------------------------------
//【多態アロケータ適用版】標準new/delete
//--------------------------------------------------------------------------------
#ifdef GASHA_ENABLE_POLY_ALLOCATOR

//--------------------
//new
void* operator new(const std::size_t size) GASHA_STDNEW_THROW
{
	GASHA_ polyAllocator allocator;
	void* p = allocator->alloc(size, allocator.align());
	allocator.callbackAtNew(p, size, GASHA_ newMethod_t::methodOfNew);
	return p;
}

//--------------------
//配列new
void* operator new[](const std::size_t size) GASHA_STDNEW_THROW
{
	GASHA_ polyAllocator allocator;
	void* p = allocator->alloc(size, allocator.align());
	allocator.callbackAtNew(p, size, GASHA_ newMethod_t::methodOfNewArrays);
	return p;
}

//--------------------
//delete
void operator delete(void* p) GASHA_STDDELETE_THROW
{
	GASHA_ polyAllocator allocator;
	allocator.callbackAtDelete(p, GASHA_ deleteMethod_t::methodOfDelete);
	allocator->free(p);
}

//--------------------
//配列delete
void operator delete[](void* p) GASHA_STDDELETE_THROW
{
	GASHA_ polyAllocator allocator;
	allocator.callbackAtDelete(p, GASHA_ deleteMethod_t::methodOfDeleteArrays);
	allocator->free(p);
}

#endif//GASHA_ENABLE_POLY_ALLOCATOR

// End of file
