//--------------------------------------------------------------------------------
// new.cpp
// 多態アロケータ対応標準new/delete【関数／実体定義部】
//
// Gakimaru's standard library for C++ - GASHA
//   Copyright (c) 2014 Itagaki Mamoru
//   Released under the MIT license.
//     https://github.com/gakimaru/gasha/blob/master/LICENSE
//--------------------------------------------------------------------------------

#include <gasha/new.inl>//多態アロケータ対応標準new/delete【インライン関数／テンプレート関数定義部】

#include <gasha/poly_allocator.h>//多態アロケータ

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
	allocator.callbackAtNew(p, size, GASHA_ newMethod_type::methodOfNew);
	return p;
}

//--------------------
//配列new
void* operator new[](const std::size_t size) GASHA_STDNEW_THROW
{
	GASHA_ polyAllocator allocator;
	void* p = allocator->alloc(size, allocator.align());
	allocator.callbackAtNew(p, size, GASHA_ newMethod_type::methodOfNewArrays);
	return p;
}

//--------------------
//new
void* operator new(const std::size_t size, const std::nothrow_t&) GASHA_STDNEW_NOTHROW
{
	GASHA_ polyAllocator allocator;
	void* p = allocator->alloc(size, allocator.align());
	allocator.callbackAtNew(p, size, GASHA_ newMethod_type::methodOfNew);
	return p;
}

//--------------------
//配列new
void* operator new[](const std::size_t size, const std::nothrow_t&) GASHA_STDNEW_NOTHROW
{
	GASHA_ polyAllocator allocator;
	void* p = allocator->alloc(size, allocator.align());
	allocator.callbackAtNew(p, size, GASHA_ newMethod_type::methodOfNewArrays);
	return p;
}

//--------------------
//delete
void operator delete(void* p) GASHA_STDDELETE_THROW
{
	GASHA_ polyAllocator allocator;
	allocator.callbackAtDelete(p, GASHA_ deleteMethod_type::methodOfDelete);
	allocator->free(p);
}

//--------------------
//配列delete
void operator delete[](void* p) GASHA_STDDELETE_THROW
{
	GASHA_ polyAllocator allocator;
	allocator.callbackAtDelete(p, GASHA_ deleteMethod_type::methodOfDeleteArrays);
	allocator->free(p);
}

#endif//GASHA_ENABLE_POLY_ALLOCATOR

// End of file
