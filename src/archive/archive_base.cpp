//--------------------------------------------------------------------------------
// archive/archive_base.cpp
// アーカイブ/アーカイブ基底クラス【関数／実体定義部】
//
// Gakimaru's researched and standard library for C++ - GASHA
//   Copyright (c) 2014 Itagaki Mamoru
//   Released under the MIT license.
//     https://github.com/gakimaru/gasha/blob/master/LICENSE
//--------------------------------------------------------------------------------

#include <gasha/archive/archive_base.inl>//アーカイブ/アーカイブ基底クラス【インライン関数／テンプレート関数定義部】

GASHA_NAMESPACE_BEGIN;//ネームスペース：開始

//--------------------------------------------------------------------------------
//アーカイブ
namespace archive
{
	//--------------------------------------------------------------------------------
	//アーカイブ/アーカイブ基底クラス
	//--------------------------------------------------------------------------------

	//--------------------
	//アーカイブ基底クラス

	//リストにデータ項目追加
	bool archiveBase::addItem(const GASHA_ serialization::itemInfoBase& item)
	{
		//データ項目を追加
		itemInfoNode* node = m_itemInfoTree.at(item.m_nameCrc);
		GASHA_SIMPLE_ASSERT(node == nullptr, "Already registered item.");//重複チェック
		if(node)
			return false;
		node = m_workBuff.newObj<itemInfoNode>(item);
		GASHA_SIMPLE_ASSERT(node != nullptr, "m_workBuff is not enough memory.");//ワークバッファのメモリ不足チェック
		if(!node)
			return false;
		m_itemInfoTree.insert(*node);//データ項目情報を木に連結
		m_result.updatePeakWorkSize(m_workBuff.size());//ワークバッファの最大使用量更新
		return true;
	}

}//namespace archive

GASHA_NAMESPACE_END;//ネームスペース：終了

//テンプレートクラスの明示的なインスタンス化
#include <gasha/rb_tree.cpp.h>//赤黒木【関数／実体定義部】

GASHA_INSTANCING_rbTree(GASHA_ archive::archiveBase::itemInfoOpe);//プロファイル情報木型

// End of file
