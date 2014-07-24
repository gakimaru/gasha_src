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
		m_numInvalidItem += src.m_numInvalidItem;//（名前が衝突するなどして）無効となったデータ項目の数
		m_numSmallerSizeItem += src.m_numSmallerSizeItem;//サイズが縮小されたデータ項目の数
		m_numLargerSizeItem += src.m_numLargerSizeItem;//サイズが拡大されたデータ項目の数
		m_numSmallerArrItem += src.m_numSmallerArrItem;//配列要素数が縮小されたデータ項目の数
		m_numLargerArrItem += src.m_numLargerArrItem;//配列要素数が拡大されたデータ項目の数
		m_numOnlyOnSaveData += src.m_numOnlyOnSaveData;//セーブデータ上にのみ存在するデータ項目の数
		m_numOnlyOnMem += src.m_numOnlyOnMem;//セーブデータ上にないデータ項目の数
		m_numObjOnSaveDataOnly += src.m_numObjOnSaveDataOnly;//現在オブジェクト型ではないが、セーブデータ上ではそうだったデータ項目の数
		m_numObjOnMemOnly += src.m_numObjOnMemOnly;//現在オブジェクト型だが、セーブデータ上ではそうではなかったデータ項目の数
		m_numArrOnSaveDataOnly += src.m_numArrOnSaveDataOnly;//現在配列型ではないが、セーブデータ上ではそうだったデータ項目の数
		m_numArrOnMemOnly += src.m_numArrOnMemOnly;//現在配列型だが、セーブデータ上ではそうではなかったデータ項目の数
		m_numPtrOnSaveDataOnly += src.m_numPtrOnSaveDataOnly;//現在ポインタ型ではないが、セーブデータ上ではそうだったデータ項目の数
		m_numPtrOnMemOnly += src.m_numPtrOnMemOnly;//現在ポインタ型だが、セーブデータ上ではそうではなかったデータ項目の数
		m_numNulOnSaveDataOnly += src.m_numNulOnSaveDataOnly;//現在ヌルではないが、セーブデータ上ではそうだったデータ項目の数
		m_numNulOnMemOnly += src.m_numNulOnMemOnly;//現在ヌルだが、セーブデータ上ではそうではなかったデータ項目の数
		m_copiedSize += src.m_copiedSize;//コピー済みサイズ
		updatePeakWorkSize(src.m_peakWorkSize);//最も多く消費したワークバッファサイズ
	}
	
	//処理結果を計上
	void results::addResult(const GASHA_ serialization::itemInfoBase& src)
	{
		addNumSmallerSizeItem(src.nowSizeIsSamall());//サイズが縮小されたデータ項目の数
		addNumLargerSizeItem(src.nowSizeIsLarge());//サイズが拡大されたデータ項目の数
		addNumSmallerArrItem(src.nowArrIsSmall());//配列要素数が縮小されたデータ項目の数
		addNumLargerArrItem(src.nowArrIsLarge());//配列要素数が拡大されたデータ項目の数
		addNumOnlyOnSaveData(src.isOnlyOnSaveData());//セーブデータ上にのみ存在するデータ項目の数
		addNumOnlyOnMem(src.isOnlyOnMem());//セーブデータ上にないデータ項目の数
		addNumObjOnSaveDataOnly(src.nowIsNotObjButSaveDataIs());//現在オブジェクト型ではないが、セーブデータ上ではそうだったデータ項目の数
		addNumObjOnMemOnly(src.nowIsObjButSaveDataIsNot());//現在オブジェクト型だが、セーブデータ上ではそうではなかったデータ項目の数
		addNumArrOnSaveDataOnly(src.nowIsNotArrButSaveDataIs());//現在配列型ではないが、セーブデータ上ではそうだったデータ項目の数
		addNumArrOnMemOnly(src.nowIsArrButSaveDataIsNot());//現在配列型だが、セーブデータ上ではそうではなかったデータ項目の数
		addNumPtrOnSaveDataOnly(src.nowIsNotPtrButSaveDataIs());//現在ポインタ型ではないが、セーブデータ上ではそうだったデータ項目の数
		addNumPtrOnMemOnly(src.nowIsPtrButSaveDataIsNot());//現在ポインタ型だが、セーブデータ上ではそうではなかったデータ項目の数
		addNumNulOnSaveDataOnly(src.nowIsNotNulButSaveDataIs());//現在ヌルではないが、セーブデータ上ではそうだったデータ項目の数
		addNumNulOnMemOnly(src.nowIsNulButSaveDataIsNot());//現在ヌルだが、セーブデータ上ではそうではなかったデータ項目の数
	}

}//namespace archive

GASHA_NAMESPACE_END;//ネームスペース：終了

// End of file
