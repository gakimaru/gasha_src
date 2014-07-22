//--------------------------------------------------------------------------------
// archive/input_text_archive_format.cpp
// アーカイブ/テキストアーカイブ形式クラス（読み込み用）【関数／実体定義部】
//
// ※基本的に明示的なインクルードの必要はなし。（.h ファイルの末尾でインクルード）
//
// Gakimaru's researched and standard library for C++ - GASHA
//   Copyright (c) 2014 Itagaki Mamoru
//   Released under the MIT license.
//     https://github.com/gakimaru/gasha/blob/master/LICENSE
//--------------------------------------------------------------------------------

#include <gasha/archive/input_text_archive_format.inl>//アーカイブ/テキストアーカイブ形式クラス（読み込み用）【インライン関数／テンプレート関数定義部】

#include <gasha/archive/results.h>//アーカイブ/処理結果

GASHA_NAMESPACE_BEGIN;//ネームスペース：開始

//--------------------------------------------------------------------------------
//アーカイブ
namespace archive
{
	//--------------------------------------------------------------------------------
	//アーカイブ/テキストアーカイブ形式クラス（読み込み用）
	//--------------------------------------------------------------------------------

	//--------------------
	//テキスト形式アーカイブクラス（アーカイブ読み込み用）

	//パース
	bool inputTextArchiveFormat::parse(GASHA_ archive::inputArchiveAdapter arc)
	{
		GASHA_ archive::results& result = arc.result();
		if (result.hasFatalError())//致命的なエラーが出ている時は即時終了する
			return false;
		//未実装
		return !result.hasFatalError();
	}
	
	//シグネチャ読み込み
	bool inputTextArchiveFormat::readSignature(GASHA_ archive::inputArchiveAdapter arc)
	{
		GASHA_ archive::results& result = arc.result();
		if (result.hasFatalError())//致命的なエラーが出ている時は即時終了する
			return false;
		//未実装
		return !result.hasFatalError();
	}
	
	//ブロックヘッダー読み込み
	bool inputTextArchiveFormat::readBlockHeader(GASHA_ archive::inputArchiveAdapter arc, const GASHA_ serialization::itemInfoBase& item, const GASHA_ serialization::itemInfoBase* delegate_item, const GASHA_ serialization::version& ver, GASHA_ serialization::itemInfoBase& input_item, GASHA_ serialization::version& input_ver, std::size_t& block_size)
	{
		GASHA_ archive::results& result = arc.result();
		if (result.hasFatalError())//致命的なエラーが出ている時は即時終了する
			return false;
		//未実装
		return !result.hasFatalError();
	}
	
	//配列ブロックヘッダー読み込み
	bool inputTextArchiveFormat::readArrayHeader(GASHA_ archive::inputArchiveAdapter arc, const GASHA_ serialization::itemInfoBase& item, std::size_t& array_elem_num, std::size_t& array_block_size)
	{
		GASHA_ archive::results& result = arc.result();
		if (result.hasFatalError())//致命的なエラーが出ている時は即時終了する
			return false;
		//未実装
		return !result.hasFatalError();
	}
	
	//要素ヘッダー読み込み
	bool inputTextArchiveFormat::readElemHeader(GASHA_ archive::inputArchiveAdapter arc, const GASHA_ serialization::itemInfoBase& item, const std::size_t index, short& items_num, std::size_t& elem_size)
	{
		GASHA_ archive::results& result = arc.result();
		if (result.hasFatalError())//致命的なエラーが出ている時は即時終了する
			return false;
		//未実装
		return !result.hasFatalError();
	}
	
	//データ項目読み込み
	bool inputTextArchiveFormat::readDataItem(GASHA_ archive::inputArchiveAdapter arc, const GASHA_ serialization::itemInfoBase& item, const GASHA_ serialization::itemInfoBase* delegate_child_item_now, const bool item_is_valid, const bool is_required_retry)
	{
		GASHA_ archive::results& result = arc.result();
		if (result.hasFatalError())//致命的なエラーが出ている時は即時終了する
			return false;
		//未実装
		return !result.hasFatalError();
	}
	
	//要素フッター読み込み
	bool inputTextArchiveFormat::tryAndReadElemFooter(inputArchiveAdapter arc, const GASHA_ serialization::itemInfoBase& item, const std::size_t index, bool& is_elem_end)
	{
		GASHA_ archive::results& result = arc.result();
		if (result.hasFatalError())//致命的なエラーが出ている時は即時終了する
			return false;
		//未実装
		return !result.hasFatalError();
	}
	
	//要素読み込み終了
	bool inputTextArchiveFormat::finishReadElem(GASHA_ archive::inputArchiveAdapter parent_arc, GASHA_ archive::inputArchiveAdapter child_arc)
	{
		GASHA_ archive::results& result = child_arc.result();
		if (result.hasFatalError())//致命的なエラーが出ている時は即時終了する
			return false;
		//未実装
		return !result.hasFatalError();
	}
	
	//配列ブロックフッター読み込み
	bool inputTextArchiveFormat::tryAndReadArrayFooter(GASHA_ archive::inputArchiveAdapter arc, const GASHA_ serialization::itemInfoBase& item, bool& is_array_block_end)
	{
		GASHA_ archive::results& result = arc.result();
		if (result.hasFatalError())//致命的なエラーが出ている時は即時終了する
			return false;
		//未実装
		return !result.hasFatalError();
	}
	
	//配列ブロック読み込み終了
	bool inputTextArchiveFormat::finishReadArray(GASHA_ archive::inputArchiveAdapter parent_arc, GASHA_ archive::inputArchiveAdapter child_arc)
	{
		GASHA_ archive::results& result = child_arc.result();
		if (result.hasFatalError())//致命的なエラーが出ている時は即時終了する
			return false;
		//未実装
		return !result.hasFatalError();
	}
	
	//ブロックの読み込みをスキップ
	bool inputTextArchiveFormat::skipReadBlock(GASHA_ archive::inputArchiveAdapter arc)
	{
		GASHA_ archive::results& result = arc.result();
		if (result.hasFatalError())//致命的なエラーが出ている時は即時終了する
			return false;
		//未実装
		return !result.hasFatalError();
	}
	
	//ブロックフッター読み込み
	bool inputTextArchiveFormat::tryAndReadBlockFooter(GASHA_ archive::inputArchiveAdapter arc, const GASHA_ serialization::itemInfoBase& item, bool& is_block_end)
	{
		GASHA_ archive::results& result = arc.result();
		if (result.hasFatalError())//致命的なエラーが出ている時は即時終了する
			return false;
		//未実装
		return !result.hasFatalError();
	}
	
	//ブロック読み込み終了
	bool inputTextArchiveFormat::finishReadBlock(GASHA_ archive::inputArchiveAdapter parent_arc,GASHA_ archive:: inputArchiveAdapter child_arc)
	{
		GASHA_ archive::results& result = child_arc.result();
		if (result.hasFatalError())//致命的なエラーが出ている時は即時終了する
			return false;
		//未実装
		return !result.hasFatalError();
	}
	
	//読み込みテストの結果、ブロックフッターでなければデータ項目（オブジェクト）の読み込みを継続する
	bool inputTextArchiveFormat::requireNextBlockHeader(GASHA_ archive::inputArchiveAdapter arc, GASHA_ serialization::itemInfoBase& require_item, std::size_t& require_block_size, bool& is_found_next_block)
	{
		GASHA_ archive::results& result = arc.result();
		if (result.hasFatalError())//致命的なエラーが出ている時は即時終了する
			return false;
		//未実装
		return !result.hasFatalError();
	}
	
	//ターミネータ読み込み
	bool inputTextArchiveFormat::readTerminator(GASHA_ archive::inputArchiveAdapter arc)
	{
		GASHA_ archive::results& result = arc.result();
		if (result.hasFatalError())//致命的なエラーが出ている時は即時終了する
			return false;
		//未実装
		return !result.hasFatalError();
	}

}//namespace archive

GASHA_NAMESPACE_END;//ネームスペース：終了

// End of file
