//--------------------------------------------------------------------------------
// archive/input_binary_archive_format.cpp
// アーカイブ/バイナリアーカイブ形式クラス（読み込み用）【関数／実体定義部】
//
// Gakimaru's researched and standard library for C++ - GASHA
//   Copyright (c) 2014 Itagaki Mamoru
//   Released under the MIT license.
//     https://github.com/gakimaru/gasha/blob/master/LICENSE
//--------------------------------------------------------------------------------

#include <gasha/archive/input_binary_archive_format.inl>//アーカイブ/バイナリアーカイブ形式クラス（読み込み用）【インライン関数／テンプレート関数定義部】

#include <gasha/archive/results.h>//アーカイブ/処理結果
#include <gasha/serialization/item_attr.h>//アーカイブ/データ項目属性
#include <gasha/serialization/item_info_base.h>//アーカイブ/データ項目情報基底クラス

#include <gasha/crc32.h>//CRC32計算
#include <gasha/simple_assert.h>//シンプルアサーション

#include <cstring>//std::memcmp()

GASHA_NAMESPACE_BEGIN;//ネームスペース：開始

//--------------------------------------------------------------------------------
//アーカイブ
namespace archive
{
	//--------------------------------------------------------------------------------
	//アーカイブ/バイナリアーカイブ形式クラス（読み込み用）
	//--------------------------------------------------------------------------------

	//--------------------
	//バイナリ形式アーカイブクラス（アーカイブ読み込み用）

	//パース
	bool inputBinaryArchiveFormat::parse(GASHA_ archive::inputArchiveAdapter arc)
	{
		results& result = arc.result();
		if (result.hasFatalError())//致命的なエラーが出ている時は即時終了する
			return false;
		//何もしない
		return !result.hasFatalError();
	}
	
	//シグネチャ読み込み
	bool inputBinaryArchiveFormat::readSignature(GASHA_ archive::inputArchiveAdapter arc)
	{
		results& result = arc.result();
		if (result.hasFatalError())//致命的なエラーが出ている時は即時終了する
			return false;
		char signature[SIGNATURE_SIZE];//シグネチャ読み込みバッファ
		arc.read(result, signature, SIGNATURE_SIZE);//シグネチャ読み込み
		if (std::memcmp(signature, SIGNATURE, SIGNATURE_SIZE) != 0)//シグネチャチェック
			result.setHasFatalError();
		return !result.hasFatalError();
	}
	
	//ブロックヘッダー読み込み
	bool inputBinaryArchiveFormat::readBlockHeader(GASHA_ archive::inputArchiveAdapter arc, const GASHA_ serialization::itemInfoBase& item, const GASHA_ serialization::itemInfoBase* delegate_item, const GASHA_ serialization::version& ve, GASHA_ serialization::itemInfoBase& input_item, GASHA_ serialization::version& input_ver, std::size_t& block_size)
	{
		results& result = arc.result();
		if (result.hasFatalError())//致命的なエラーが出ている時は即時終了する
			return false;
		char begin_mark[BEGIN_MARK_SIZE];//ブロック始端読み込みバッファ
		arc.read(result, begin_mark, BEGIN_MARK_SIZE);//ブロック始端読み込み
		if (std::memcmp(begin_mark, BLOCK_BEGIN, BEGIN_MARK_SIZE) != 0)//ブロック始端チェック
		{
			result.setHasFatalError();
			return false;
		}
		input_item.clearForLoad();//読み込み情報を一旦クリア
		arc.read(result, const_cast<GASHA_ crc32_t*>(&input_item.m_nameCrc), sizeof(input_item.m_nameCrc));//名前CRC読み込み
		arc.read(result, const_cast<GASHA_ serialization::itemAttr::value_type*>(&input_item.m_attr.m_value), sizeof(input_item.m_attr.m_value));//属性読み込み
		if (input_item.m_attr.hasVer())//バージョン情報があるか？
		{
			arc.read(result, const_cast<unsigned int*>(input_ver.refVersion()), input_ver.size());//バージョン読み込み
			input_ver.calcFromVer();
		}
		arc.read(result, const_cast<std::size_t*>(&input_item.m_itemSize), sizeof(input_item.m_itemSize));//ブロックサイズ読み込み
		block_size = input_item.m_itemSize;//ブロックサイズ
		//委譲アイテムの指定があれば、名前をチェックせずに入力データに情報をコピーする
		if (delegate_item)
		{
			input_item.copyFromOnMem(*delegate_item);//セーブデータの情報に現在の情報をコピー（統合）
			input_item.setOnlyOnSaveData();//セーブデータ上のみのデータ扱いにする（集計のため）
		}
		else
		{
			//名前CRCをチェックして情報を統合
			//※違っていたら致命的エラー（セーブデータが適合していない）
			if (input_item == item)
			{
				//両者に存在する
				input_item.copyFromOnMem(item);//セーブデータの情報に現在の情報をコピー（統合）
			}
			else
			{
				//一致しない
				input_item.setOnlyOnSaveData();//セーブデータにしか存在しないデータ項目
				result.setHasFatalError();//致命的エラー設定
			}
		}
		//ヌルの時はこの時点で処理済みにする
		//※ヌルじゃない時は配列読み込みが済んだら処理済みにする
		if (input_item.m_attr.isNul())
		{
			item.setIsAlready();//処理済みにする
			result.addResult(input_item);//結果を計上
		}
		return !result.hasFatalError();
	}
	
	//配列ブロックヘッダー読み込み
	bool inputBinaryArchiveFormat::readArrayHeader(GASHA_ archive::inputArchiveAdapter arc, const GASHA_ serialization::itemInfoBase& item, std::size_t& array_elem_num, std::size_t& array_block_size)
	{
		results& result = arc.result();
		if (result.hasFatalError())//致命的なエラーが出ている時は即時終了する
			return false;
		if (item.isArr())//配列型の時だけ読み込む
		{
			char begin_mark[BEGIN_MARK_SIZE];//配列ブロック始端読み込みバッファ
			arc.read(result, begin_mark, BEGIN_MARK_SIZE);//配列ブロック始端読み込み
			if (std::memcmp(begin_mark, ARRAY_BEGIN, BEGIN_MARK_SIZE) != 0)//配列ブロック始端チェック
			{
				result.setHasFatalError();
				return false;
			}
			arc.read(result, const_cast<std::size_t*>(&item.m_arrNum), sizeof(item.m_arrNum));//配列要素数読み込み
			arc.read(result, const_cast<std::size_t*>(&array_block_size), sizeof(array_block_size));//配列ブロックサイズ読み込み
			array_elem_num = item.m_arrNum;//配列要素数
		}
		else
		{
			*const_cast<std::size_t*>(&item.m_arrNum) = 0;//配列要素数
			array_elem_num = 0;//配列要素数
			array_block_size = 0;//配列ブロックサイズ
		}
		item.setIsAlready();//処理済みにする
		result.addResult(item);//結果を計上
		return !result.hasFatalError();
	}
	
	//要素ヘッダー読み込み
	bool inputBinaryArchiveFormat::readElemHeader(GASHA_ archive::inputArchiveAdapter arc, const GASHA_ serialization::itemInfoBase& item, const std::size_t index, short& items_num, std::size_t& elem_size)
	{
		results& result = arc.result();
		items_num = 0;
		if (result.hasFatalError())//致命的なエラーが出ている時は即時終了する
			return false;
		char begin_mark[BEGIN_MARK_SIZE];//配列始端読み込みバッファ
		arc.read(result, begin_mark, BEGIN_MARK_SIZE);//配列始端読み込み
		if (std::memcmp(begin_mark, ELEM_BEGIN, BEGIN_MARK_SIZE) != 0)//要素始端チェック
		{
			result.setHasFatalError();
			return false;
		}
		arc.read(result, &items_num, sizeof(items_num));//データ項目数読み込み
		arc.read(result, &elem_size, sizeof(elem_size));//要素サイズ読み込み
		return !result.hasFatalError();
	}
	
	//データ項目読み込み
	bool inputBinaryArchiveFormat::readDataItem(GASHA_ archive::inputArchiveAdapter arc, const GASHA_ serialization::itemInfoBase& item, const GASHA_ serialization::itemInfoBase* delegate_child_item_now, GASHA_ serialization::itemInfoBase& child_item, const bool item_is_valid, const bool is_required_retry)
	{
		results& result = arc.result();
		if (result.hasFatalError())//致命的なエラーが出ている時は即時終了する
			return false;
		if (is_required_retry)//リトライ要求
		{
			//記憶している位置に戻す
			arc.seek(result, -static_cast<int>(m_readSizeForPrevDataItem));//【リトライ用】前回のデータ読み込みサイズ
		}
		m_readSizeForPrevDataItem = 0;//【リトライ用】前回のデータ読み込みサイズをリセット
		std::size_t read_size = 0;//この処理中で計上する読み込みサイズ
		char begin_mark[BEGIN_MARK_SIZE];//データ項目／ブロック始端読み込みバッファ
		arc.read(result, begin_mark, BEGIN_MARK_SIZE, &read_size);//データ項目／ブロック始端読み込み
		bool is_block_begin = false;
		if (std::memcmp(begin_mark, BLOCK_BEGIN, BEGIN_MARK_SIZE) == 0)//ブロック始端チェック
			is_block_begin = true;
		else
		{
			if (std::memcmp(begin_mark, ITEM_BEGIN, BEGIN_MARK_SIZE) != 0)//データ項目始端チェック
			{
				result.setHasFatalError();
				return false;
			}
		}
		arc.read(result, const_cast<GASHA_ crc32_t*>(&child_item.m_nameCrc), sizeof(child_item.m_nameCrc), &read_size);//名前CRC読み込み
		arc.read(result, const_cast<GASHA_ serialization::itemAttr::value_type*>(&child_item.m_attr.m_value), sizeof(child_item.m_attr.m_value), &read_size);//属性読み込み
		const GASHA_ serialization::itemInfoBase* child_item_now = nullptr;
		if (delegate_child_item_now)//委譲データ項目があればそれを優先的に使用
		{
			child_item_now = delegate_child_item_now;//委譲データ項目
			//child_itemの名前とCRCを書き換える
			child_item.m_name = delegate_child_item_now->m_name;//名前
			*const_cast<GASHA_ crc32_t*>(&child_item.m_nameCrc) = delegate_child_item_now->m_nameCrc;//名前のCRC
		}
		else
			child_item_now = arc.findItem(child_item.m_nameCrc);//対応するデータ項目情報を検索
		if (child_item_now)//対応するデータ項目が見つかったか？
			child_item.copyFromOnMem(*child_item_now);//現在の情報をセーブデータの情報にコピー（統合）
		else
			child_item.setOnlyOnSaveData();//対応するデータがない：セーブデータにしかデータが存在しない
		//対象データ項目はオブジェクト型か？
		if (child_item.isObj())
		{
			//対象がオブジェクト（ブロック）なら、オブジェクトとして処理をやり直すために、この時点で処理終了
			arc.seek(result, -static_cast<int>(read_size));//やり直しのために、バッファのカレントポインタを先頭に戻す
			//ブロック始端を検出していなかったらデータ不整合でエラー終了
			if (!is_block_begin)
			{
				result.setHasFatalError();
				return false;
			}
			return !result.hasFatalError();
		}
		//通常データの読み込み処理
		GASHA_SIMPLE_ASSERT(!child_item.m_attr.hasVer(), "Vesion is exist in non-object item.(maybe broken data?)");//オブジェクトでもないのにバージョン情報があればNG
		arc.read(result, const_cast<std::size_t*>(&child_item.m_itemSize), sizeof(child_item.m_itemSize), &read_size);//データサイズ読み込み
		if (!child_item.isNul())//【セーブデータ上の】データがヌルでなければ処理する
		{
			if (child_item.isArr())//配列か？
				arc.read(result, const_cast<std::size_t*>(&child_item.m_arrNum), sizeof(child_item.m_arrNum), &read_size);//配列要素数読み込み
			unsigned char* p = reinterpret_cast<unsigned char*>(const_cast<void*>(child_item.m_itemP));
			const std::size_t elem_num = child_item.extent();
			for (std::size_t index = 0; index < elem_num && !result.hasFatalError(); ++index)//【セーブデータ上の】配列要素数分データ書き込み
			{
				const bool element_is_valid =//有効なデータか？
					item_is_valid && //親のデータが有効か？
					!child_item.isOnlyOnSaveData() && //セーブデータにしかないデータではないか？
					!child_item.nowIsNul() && //現在の（コピー先の）データがヌルではないか？
					index < child_item.nowExtent();//現在の（コピー先の）配列の範囲内か？
				void* p_tmp = element_is_valid ? p : nullptr;//有効なデータでなければnullptrを渡し、空読み込みする
				arc.readWithFunc(result, child_item.m_nowTypeCtrl.m_fromMemFuncP, p_tmp, child_item.m_nowItemSize, child_item.m_itemSize, &read_size);//データ読み込み
				if (p)
					p += child_item.m_nowItemSize;//次の要素
			}
		}
		char end_mark[END_MARK_SIZE];//データ項目終端読み込みバッファ
		arc.read(result, end_mark, END_MARK_SIZE, &read_size);//データ項目終端読み込み
		if (std::memcmp(end_mark, ITEM_END, END_MARK_SIZE) != 0)//データ項目始端チェック
		{
			result.setHasFatalError();
			return false;
		}
		child_item.setIsAlready();//処理済みにする
		result.addResult(child_item);//結果を計上
		m_readSizeForPrevDataItem = read_size;//【リトライ用】前回のデータ読み込みサイズを記憶
		return true;
	}
	
	//要素フッター読み込み
	bool inputBinaryArchiveFormat::tryAndReadElemFooter(GASHA_ archive::inputArchiveAdapter arc, const GASHA_ serialization::itemInfoBase& item, const std::size_t index, bool& is_elem_end)
	{
		is_elem_end = true;//要素終了扱い
		results& result = arc.result();
		if (result.hasFatalError())//致命的なエラーが出ている時は即時終了する
			return false;
		std::size_t read_size = 0;
		char end_mark[END_MARK_SIZE];
		arc.read(result, end_mark, END_MARK_SIZE, &read_size);//要素終端読み込み
		if (std::memcmp(end_mark, ELEM_END, END_MARK_SIZE) != 0)//要素終端チェック
		{
			is_elem_end = false;//要素終了ではない
			arc.seek(result, -static_cast<int>(read_size));//要素の終端ではなかったため、バッファのカレントポインタを戻す
		}
		return !result.hasFatalError();
	}
	
	//要素読み込み終了
	bool inputBinaryArchiveFormat::finishReadElem(GASHA_ archive::inputArchiveAdapter parent_arc, GASHA_ archive::inputArchiveAdapter child_arc)
	{
		parent_arc.addResult(child_arc.result());//親に処理結果を計上
		results& result = child_arc.result();
		if (result.hasFatalError())//致命的なエラーが出ている時は即時終了する
			return false;
		parent_arc.seek(result, static_cast<int>(child_arc.size()));
		return !result.hasFatalError();
	}
	
	//配列ブロックフッター読み込み
	bool inputBinaryArchiveFormat::tryAndReadArrayFooter(GASHA_ archive::inputArchiveAdapter arc, const GASHA_ serialization::itemInfoBase& item, bool& is_array_block_end)
	{
		is_array_block_end = true;//配列ブロック終了扱い
		results& result = arc.result();
		if (result.hasFatalError())//致命的なエラーが出ている時は即時終了する
			return false;
		if (item.isArr())
		{
			std::size_t read_size = 0;
			char end_mark[END_MARK_SIZE];
			arc.read(result, end_mark, END_MARK_SIZE, &read_size);//配列ブロック終端読み込み
			if (std::memcmp(end_mark, ARRAY_END, END_MARK_SIZE) != 0)//要素終端チェック
			{
				is_array_block_end = false;//配列ブロック終了ではない
				arc.seek(result, -static_cast<int>(read_size));//要素の終端ではなかったため、バッファのカレントポインタを戻す
			}
		}
		return !result.hasFatalError();
	}
	
	//配列ブロック読み込み終了
	bool inputBinaryArchiveFormat::finishReadArray(GASHA_ archive::inputArchiveAdapter parent_arc, GASHA_ archive::inputArchiveAdapter child_arc)
	{
		parent_arc.addResult(child_arc.result());//親に処理結果を計上
		results& result = child_arc.result();
		if (result.hasFatalError())//致命的なエラーが出ている時は即時終了する
			return false;
		parent_arc.seek(result, static_cast<int>(child_arc.size()));
		return !result.hasFatalError();
	}
	
	//ブロックの読み込みをスキップ
	bool inputBinaryArchiveFormat::skipReadBlock(GASHA_ archive::inputArchiveAdapter arc)
	{
		results& result = arc.result();
		if (result.hasFatalError())//致命的なエラーが出ている時は即時終了する
			return false;
		char begin_mark[BEGIN_MARK_SIZE];//ブロック始端読み込みバッファ
		arc.read(result, begin_mark, BEGIN_MARK_SIZE);//ブロック始端読み込み
		if (std::memcmp(begin_mark, BLOCK_BEGIN, BEGIN_MARK_SIZE) != 0)//ブロック始端チェック
		{
			result.setHasFatalError();
			return false;
		}
		GASHA_ crc32_t name_crc = 0;
		GASHA_ serialization::itemAttr attr(false, false, false, false, false);
		arc.read(result, &name_crc, sizeof(name_crc));//名前CRC読み込み
		arc.read(result, const_cast<GASHA_ serialization::itemAttr::value_type*>(&attr.m_value), sizeof(attr.m_value));//属性書き込み
		if (attr.hasVer())//バージョン情報があるか？
		{
			GASHA_ serialization::version input_ver_dummy;
			arc.read(result, const_cast<unsigned int*>(input_ver_dummy.refVersion()), input_ver_dummy.size());//バージョン読み込み
		}
		if (!attr.isNul())//ヌル時はここまでの情報で終わり
		{
			std::size_t item_size;
			arc.read(result, &item_size, sizeof(item_size));//ブロックサイズ読み込み
			arc.seek(result, static_cast<int>(item_size));//ブロックサイズ分、バッファのカレントポインタを進める
		}
		//ブロックフッター読み込み
		char end_mark[END_MARK_SIZE];
		arc.read(result, end_mark, END_MARK_SIZE);//ブロック終端読み込み
		if (std::memcmp(end_mark, BLOCK_END, END_MARK_SIZE) != 0)//ブロック終端チェック
		{
			result.setHasFatalError();
			return false;
		}
		return !result.hasFatalError();
	}
	
	//ブロックフッター読み込み
	bool inputBinaryArchiveFormat::tryAndReadBlockFooter(GASHA_ archive::inputArchiveAdapter arc, const GASHA_ serialization::itemInfoBase& item, bool& is_block_end)
	{
		is_block_end = true;//ブロック終了扱い
		results& result = arc.result();
		if (result.hasFatalError())//致命的なエラーが出ている時は即時終了する
			return false;
		std::size_t read_size = 0;
		char end_mark[END_MARK_SIZE];
		arc.read(result, end_mark, END_MARK_SIZE, &read_size);//ブロック終端読み込み
		if (std::memcmp(end_mark, BLOCK_END, END_MARK_SIZE) != 0)//ブロック終端チェック
		{
			is_block_end = false;//ブロック終了ではない
			arc.seek(result, -static_cast<int>(read_size));//ブロックの終端ではなかったため、バッファのカレントポインタを戻す
		}
		return !result.hasFatalError();
	}
	
	//ブロック読み込み終了
	bool inputBinaryArchiveFormat::finishReadBlock(GASHA_ archive::inputArchiveAdapter parent_arc, GASHA_ archive::inputArchiveAdapter child_arc)
	{
		parent_arc.addResult(child_arc.result());//親に処理結果を計上
		results& result = child_arc.result();
		if (result.hasFatalError())//致命的なエラーが出ている時は即時終了する
			return false;
		parent_arc.seek(result, static_cast<int>(child_arc.size()));
		return !result.hasFatalError();
	}
	
	//読み込みテストの結果、ブロックフッターでなければデータ項目（オブジェクト）の読み込みを継続する
	bool inputBinaryArchiveFormat::requireNextBlockHeader(GASHA_ archive::inputArchiveAdapter arc, GASHA_ serialization::itemInfoBase& require_item, std::size_t& require_block_size, bool& is_found_next_block)
	{
		is_found_next_block = false;//ブロック情報が見つかってないことにする
		results& result = arc.result();
		if (result.hasFatalError())//致命的なエラーが出ている時は即時終了する
			return false;
		std::size_t read_size = 0;//読み込みサイズ
		require_item.clearForLoad();//読み込み情報を一旦クリア
		char begin_mark[BEGIN_MARK_SIZE];//ブロック始端読み込みバッファ
		arc.read(result, begin_mark, BEGIN_MARK_SIZE, &read_size);//ブロック始端読み込み
		if (std::memcmp(begin_mark, BLOCK_BEGIN, BEGIN_MARK_SIZE) != 0)//ブロック始端チェック
		{
			arc.seek(result, -static_cast<int>(read_size));//仮読みした分、バッファのカレントポインタを戻す
			return !result.hasFatalError();
		}
		is_found_next_block = true;//見つかった
		arc.read(result, const_cast<GASHA_ crc32_t*>(&require_item.m_nameCrc), sizeof(require_item.m_nameCrc), &read_size);//名前CRC書き込み
		arc.read(result, const_cast<GASHA_ serialization::itemAttr::value_type*>(&require_item.m_attr.m_value), sizeof(require_item.m_attr.m_value), &read_size);//属性書き込み
		if (require_item.m_attr.hasVer())//バージョン情報があるか？
		{
			GASHA_ serialization::version input_ver_dummy;
			arc.read(result, const_cast<unsigned int*>(input_ver_dummy.refVersion()), input_ver_dummy.size(), &read_size);//バージョン読み込み
		}
		require_block_size = read_size;//ブロックサイズ一旦計上
		if (!require_item.isNul())//ヌル時はここまでの情報で終わり
		{
			arc.read(result, const_cast<std::size_t*>(&require_item.m_itemSize), sizeof(require_item.m_itemSize), &read_size);//ブロックサイズ読み込み
			require_block_size += require_item.m_itemSize;//ブロックサイズ計上
			if (require_item.isArr())//配列時は配列要素数も読み込み
			{
				char begin_mark[BEGIN_MARK_SIZE];//配列ブロック始端読み込みバッファ
				arc.read(result, begin_mark, BEGIN_MARK_SIZE);//配列ブロック始端読み込み
				if (std::memcmp(begin_mark, ARRAY_BEGIN, BEGIN_MARK_SIZE) == 0)//配列ブロック始端チェック
				{
					//std::size_t array_block_size = 0;
					arc.read(result, const_cast<std::size_t*>(&require_item.m_arrNum), sizeof(require_item.m_arrNum));//配列要素数読み込み
					//arc.read(result, const_cast<std::size_t*>(&array_block_size), sizeof(array_block_size));//配列ブロックサイズ読み込み
				}
			}
		}
		arc.seek(result, -static_cast<int>(read_size));//仮読みした分、バッファのカレントポインタを戻す
		return !result.hasFatalError();
	}
	
	//ターミネータ読み込み
	bool inputBinaryArchiveFormat::readTerminator(GASHA_ archive::inputArchiveAdapter arc)
	{
		results& result = arc.result();
		if (result.hasFatalError())//致命的なエラーが出ている時は即時終了する
			return false;
		char terminator[TERMINATOR_SIZE];
		arc.read(result, terminator, TERMINATOR_SIZE);//ターミネータ読み込み
		if (std::memcmp(terminator, TERMINATOR, TERMINATOR_SIZE) != 0)//ターミネータチェック
			result.setHasFatalError();
		return true;
	}

}//namespace archive

GASHA_NAMESPACE_END;//ネームスペース：終了

// End of file
