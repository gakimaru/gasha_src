//--------------------------------------------------------------------------------
// serialization/item_base.cpp
// シリアライズ/データ項目情報基底クラス【関数／実体定義部】
//
// Gakimaru's researched and standard library for C++ - GASHA
//   Copyright (c) 2014 Itagaki Mamoru
//   Released under the MIT license.
//     https://github.com/gakimaru/gasha/blob/master/LICENSE
//--------------------------------------------------------------------------------

#include <gasha/serialization/item_info_base.inl>//シリアライズ/データ項目情報基底クラス【インライン関数／テンプレート関数定義部】

GASHA_NAMESPACE_BEGIN;//ネームスペース：開始

//--------------------------------------------------------------------------------
//シリアライズ
namespace serialization
{
	//--------------------------------------------------------------------------------
	//シリアライズ/データ項目情報基底クラス
	//--------------------------------------------------------------------------------

	//--------------------
	//データ項目情報基底クラス
	
	//読み込み情報をクリア
	void itemInfoBase::clearForLoad()
	{
		//m_name = nullptr;//データ項目名
		*const_cast<crc32_t*>(&m_nameCrc) = 0;//データ項目名CRC
		//m_itemP;//データの参照ポインタ
		//m_itemType;//データの型情報
		*const_cast<std::size_t*>(&m_itemSize) = 0;//データサイズ
		*const_cast<std::size_t*>(&m_arrNum) = 0;//データの配列要素数
		const_cast<GASHA_ serialization::itemAttr*>(&m_attr)->clear();//属性
		m_nowItemSize = 0;//現在のデータサイズ
		m_nowArrNum = 0;//現在のデータの配列要素数
		m_nowAttr.clear();//現在の属性
		m_hasNowInfo = false;//現在の情報コピー済み
		m_isOnlyOnSaveData = false;//セーブデータ上にのみ存在するデータ
		m_isOnlyOnMem = false;//セーブデータ上にないデータ
		m_isAlready = false;//処理済み
	}
	
	//現在の情報をコピー
	void itemInfoBase::copyFromOnMem(const itemInfoBase& src)
	{
		//assert(m_nameCrc == src.m_nameCrc);//CRCチェック⇒しない
		m_name = src.m_name;//データ項目名
		*const_cast<crc32_t*>(&m_nameCrc) = src.m_nameCrc;//データ項目名CRC ※委譲対応のためCRCも更新
		m_itemP = src.m_itemP;//データの参照ポインタ
		m_itemType = src.m_itemType;//データの型情報
		m_nowItemSize = src.m_itemSize;//現在のデータサイズ
		m_nowArrNum = src.m_arrNum;//現在のデータの配列要素数
		m_nowAttr = src.m_attr;//現在の属性
		m_nowTypeCtrl = src.m_typeCtrl;//型操作
		m_deserialier = src.m_deserialier;//個別デシリアライザー
		m_hasNowInfo = true;//現在の情報コピー済み
		m_isOnlyOnSaveData = false;//セーブデータ上にのみ存在するデータ
		m_isOnlyOnMem = false;//セーブデータ上にないデータ
		src.resetOnlyOnMem();//コピー元の「セーブデータ上にないデータ」をリセット
	}

}//namespace serialization

GASHA_NAMESPACE_END;//ネームスペース：終了

// End of file
