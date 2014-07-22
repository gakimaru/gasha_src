//--------------------------------------------------------------------------------
// archive/results.cpp
// アーカイブ/処理結果【関数／実体定義部】
//
// Gakimaru's researched and standard library for C++ - GASHA
//   Copyright (c) 2014 Itagaki Mamoru
//   Released under the MIT license.
//     https://github.com/gakimaru/gasha/blob/master/LICENSE
//--------------------------------------------------------------------------------

#include <gasha/archive/results.inl>//アーカイブ/処理結果【インライン関数／テンプレート関数部】

GASHA_NAMESPACE_BEGIN;//ネームスペース：開始

//--------------------------------------------------------------------------------
//アーカイブ
namespace archive
{
	//--------------------------------------------------------------------------------
	//アーカイブ/処理結果
	//--------------------------------------------------------------------------------

	//--------------------
	//処理結果クラス

	//処理結果に加算
	void results::addResult(const results& src)
	{
		setHasFatalError(src.m_hasFatalError);//致命的なエラーあり
		m_numSmallerSizeItem += src.m_numSmallerSizeItem;//サイズが縮小されたデータ項目の数
		m_numLargerSizeItem += src.m_numLargerSizeItem;//サイズが拡大されたデータ項目の数
		m_numSmallerArrItem += src.m_numSmallerArrItem;//配列要素数が縮小されたデータ項目の数
		m_numLargerArrItem += src.m_numLargerArrItem;//配列要素数が拡大されたデータ項目の数
		m_numIsOnlyOnSaveData += src.m_numIsOnlyOnSaveData;//セーブデータ上にのみ存在するデータ項目の数
		m_numIsOnlyOnMem += src.m_numIsOnlyOnMem;//セーブデータ上にないデータ項目の数
		m_numIsObjOnSaveDataOnly += src.m_numIsObjOnSaveDataOnly;//現在オブジェクト型ではないが、セーブデータ上ではそうだったデータ項目の数
		m_numIsObjOnMemOnly += src.m_numIsObjOnMemOnly;//現在オブジェクト型だが、セーブデータ上ではそうではなかったデータ項目の数
		m_numIsArrOnSaveDataOnly += src.m_numIsArrOnSaveDataOnly;//現在配列型ではないが、セーブデータ上ではそうだったデータ項目の数
		m_numIsArrOnMemOnly += src.m_numIsArrOnMemOnly;//現在配列型だが、セーブデータ上ではそうではなかったデータ項目の数
		m_numIsPtrOnSaveDataOnly += src.m_numIsPtrOnSaveDataOnly;//現在ポインタ型ではないが、セーブデータ上ではそうだったデータ項目の数
		m_numIsPtrOnMemOnly += src.m_numIsPtrOnMemOnly;//現在ポインタ型だが、セーブデータ上ではそうではなかったデータ項目の数
		m_numIsNulOnSaveDataOnly += src.m_numIsNulOnSaveDataOnly;//現在ヌルではないが、セーブデータ上ではそうだったデータ項目の数
		m_numIsNulOnMemOnly += src.m_numIsNulOnMemOnly;//現在ヌルだが、セーブデータ上ではそうではなかったデータ項目の数
		m_copiedSize += src.m_copiedSize;//コピー済みサイズ
	}
	
	//処理結果を計上
	void results::addResult(const GASHA_ serialization::itemInfoBase& src)
	{
		addNumSmallerSizeItem(src.nowSizeIsSamall());//サイズが縮小されたデータ項目の数
		addNumLargerSizeItem(src.nowSizeIsLarge());//サイズが拡大されたデータ項目の数
		addNumSmallerArrItem(src.nowArrIsSmall());//配列要素数が縮小されたデータ項目の数
		addNumLargerArrItem(src.nowArrIsLarge());//配列要素数が拡大されたデータ項目の数
		addNumIsOnlyOnSaveData(src.isOnlyOnSaveData());//セーブデータ上にのみ存在するデータ項目の数
		addNumIsOnlyOnMem(src.isOnlyOnMem());//セーブデータ上にないデータ項目の数
		addNumIsObjOnSaveDataOnly(src.nowIsNotObjButSaveDataIs());//現在オブジェクト型ではないが、セーブデータ上ではそうだったデータ項目の数
		addNumIsObjOnMemOnly(src.nowIsObjButSaveDataIsNot());//現在オブジェクト型だが、セーブデータ上ではそうではなかったデータ項目の数
		addNumIsArrOnSaveDataOnly(src.nowIsNotArrButSaveDataIs());//現在配列型ではないが、セーブデータ上ではそうだったデータ項目の数
		addNumIsArrOnMemOnly(src.nowIsArrButSaveDataIsNot());//現在配列型だが、セーブデータ上ではそうではなかったデータ項目の数
		addNumIsPtrOnSaveDataOnly(src.nowIsNotPtrButSaveDataIs());//現在ポインタ型ではないが、セーブデータ上ではそうだったデータ項目の数
		addNumIsPtrOnMemOnly(src.nowIsPtrButSaveDataIsNot());//現在ポインタ型だが、セーブデータ上ではそうではなかったデータ項目の数
		addNumIsNulOnSaveDataOnly(src.nowIsNotNulButSaveDataIs());//現在ヌルではないが、セーブデータ上ではそうだったデータ項目の数
		addNumIsNulOnMemOnly(src.nowIsNulButSaveDataIsNot());//現在ヌルだが、セーブデータ上ではそうではなかったデータ項目の数
	}

}//namespace archive

GASHA_NAMESPACE_END;//ネームスペース：終了

// End of file
