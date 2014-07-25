//--------------------------------------------------------------------------------
// archive/output_binary_archive_format.cpp
// アーカイブ/バイナリアーカイブ形式クラス（書き込み用）【関数／実体定義部】
//
// Gakimaru's standard library for C++ - GASHA
//   Copyright (c) 2014 Itagaki Mamoru
//   Released under the MIT license.
//     https://github.com/gakimaru/gasha/blob/master/LICENSE
//--------------------------------------------------------------------------------

#include <gasha/archive/output_binary_archive_format.inl>//アーカイブ/バイナリアーカイブ形式クラス（書き込み用）【インライン関数／テンプレート関数定義部】

#include <gasha/archive/results.h>//アーカイブ/処理結果

#include <cstdint>//C++11 std::uint8_t, std::uint16_t, std::uint32_t

GASHA_NAMESPACE_BEGIN;//ネームスペース：開始

//--------------------------------------------------------------------------------
//アーカイブ
namespace archive
{
	//--------------------------------------------------------------------------------
	//アーカイブ/バイナリアーカイブ形式クラス（書き込み用）
	//--------------------------------------------------------------------------------

	//--------------------
	//バイナリ形式アーカイブクラス（アーカイブ書き込み用）

	//シグネチャ書き込み
	bool outputBinaryArchiveFormat::writeSignature(GASHA_ archive::outputArchiveAdapter arc)
	{
		results& result = arc.result();
		if (result.hasFatalError())//致命的なエラーが出ている時は即時終了する
			return false;
		arc.write(result, SIGNATURE, SIGNATURE_SIZE);//シグネチャ書き込み
		return !result.hasFatalError();
	}
	
	//ブロックヘッダー書き込み
	bool outputBinaryArchiveFormat::writeBlockHeader(GASHA_ archive::outputArchiveAdapter arc, const GASHA_ serialization::itemInfoBase& item, const GASHA_ serialization::version& ver)
	{
		results& result = arc.result();
		if (result.hasFatalError())//致命的なエラーが出ている時は即時終了する
			return false;
		arc.write(result, BLOCK_BEGIN, BEGIN_MARK_SIZE);//ブロック始端書き込み
		const std::uint32_t _name_crc = item.m_nameCrc;
		arc.write(result, &_name_crc, sizeof(_name_crc));//名前CRC書き込み
		item.m_attr.setHasVer();//バージョン情報ありにする
		const std::uint8_t _item_attr = item.m_attr.m_value;
		arc.write(result, &_item_attr, sizeof(_item_attr));//属性書き込み
		const std::uint32_t _version = ver.value();
		arc.write(result, &_version, sizeof(_version));//バージョン書き込み
		const std::uint32_t _block_size = 0;
		arc.write(result, &_block_size, sizeof(_block_size));//ブロックサイズ仮書き込み　※ブロック終了時に書き換える
		return !result.hasFatalError();
	}
	
	//配列ブロックヘッダー書き込み
	bool outputBinaryArchiveFormat::writeArrayHeader(GASHA_ archive::outputArchiveAdapter arc, const GASHA_ serialization::itemInfoBase& item, const std::size_t array_elem_num)
	{
		results& result = arc.result();
		if (result.hasFatalError())//致命的なエラーが出ている時は即時終了する
			return false;
		if (item.isArr())
		{
			arc.write(result, ARRAY_BEGIN, BEGIN_MARK_SIZE);//配列ブロック始端書き込み
			const std::uint32_t _array_elem_num = static_cast<std::uint32_t>(array_elem_num);
			arc.write(result, &_array_elem_num, sizeof(_array_elem_num));//配列要素数仮書き込み
			const std::uint32_t _array_block_size = 0;
			arc.write(result, &_array_block_size, sizeof(_array_block_size));//配ブロック列サイズ仮書き込み　※配列終了時に書き換える
		}
		return !result.hasFatalError();
	}
	
	//要素ヘッダー書き込み
	bool outputBinaryArchiveFormat::writeElemHeader(GASHA_ archive::outputArchiveAdapter arc, const GASHA_ serialization::itemInfoBase& item, const std::size_t index)
	{
		results& result = arc.result();
		if (result.hasFatalError())//致命的なエラーが出ている時は即時終了する
			return false;
		arc.write(result, ELEM_BEGIN, BEGIN_MARK_SIZE);//要素始端書き込み
		const std::uint16_t _items_num = 0;
		arc.write(result, &_items_num, sizeof(_items_num));//データ項目数仮書き込み　※要素終了時に書き換える
		const std::uint32_t _elem_size = 0;
		arc.write(result, &_elem_size, sizeof(_elem_size));//要素サイズ仮書き込み　※要素終了時に書き換える
		return !result.hasFatalError();
	}
	
	//データ項目書き込み
	bool outputBinaryArchiveFormat::writeDataItem(GASHA_ archive::outputArchiveAdapter arc, const GASHA_ serialization::itemInfoBase& item, const GASHA_ serialization::itemInfoBase& child_item)
	{
		results& result = arc.result();
		if (result.hasFatalError())//致命的なエラーが出ている時は即時終了する
			return false;
		arc.write(result, ITEM_BEGIN, BEGIN_MARK_SIZE);//データ項目始端書き込み
		const std::uint32_t _name_crc = child_item.m_nameCrc;
		arc.write(result, &_name_crc, sizeof(_name_crc));//名前CRC書き込み
		child_item.m_attr.resetHasVer();//バージョン情報なしにする
		const std::uint8_t _item_attr = child_item.m_attr.m_value;
		arc.write(result, &_item_attr, sizeof(_item_attr));//属性書き込み
		const std::uint32_t _item_size = child_item.m_itemSize;
		arc.write(result, &_item_size, sizeof(_item_size));//データサイズ書き込み
		if (!child_item.isNul())//ヌル時はここまでの情報で終わり
		{
			if (child_item.isArr())//配列か？
			{
				const std::uint32_t _extent = child_item.m_arrNum;
				arc.write(result, &_extent, sizeof(_extent));//配列要素数書き込み
			}
			unsigned char* p = reinterpret_cast<unsigned char*>(const_cast<void*>(child_item.m_itemP));
			const std::size_t extent = child_item.extent();
			for (std::size_t index = 0; index < extent && !result.hasFatalError(); ++index)//配列要素数分データ書き込み
			{
				arc.write(result, p, child_item.m_itemSize);//データ書き込み
				p += child_item.m_itemSize;
			}
		}
		arc.write(result, ITEM_END, END_MARK_SIZE);//データ項目終端書き込み
		return !result.hasFatalError();
	}
	
	//要素フッター書き込み
	bool outputBinaryArchiveFormat::writeElemFooter(GASHA_ archive::outputArchiveAdapter parent_arc, GASHA_ archive::outputArchiveAdapter child_arc, const GASHA_ serialization::itemInfoBase& item, const std::size_t index, std::size_t& items_num, std::size_t& elem_size)
	{
		results& result = child_arc.result();
		if (result.hasFatalError())//致命的なエラーが出ている時は即時終了する
		{
			parent_arc.addResult(result);//親に処理結果を計上
			return false;
		}
		items_num = child_arc.itemsNum();//データ項目数取得
		elem_size = child_arc.size();//データサイズ取得
		const std::uint16_t _items_num = static_cast<std::uint16_t>(items_num);
		const std::uint32_t _elem_size = static_cast<std::uint32_t>(elem_size);
		parent_arc.seek(result, -static_cast<int>(sizeof(_elem_size)));//要素サイズ情報の分、バッファのカレントポインタを戻す
		parent_arc.seek(result, -static_cast<int>(sizeof(_items_num)));//データ項目数情報の分、バッファのカレントポインタを戻す
		parent_arc.write(result, &_items_num, sizeof(_items_num));//要素サイズを更新（書き込み）
		parent_arc.write(result, &_elem_size, sizeof(_elem_size));//要素サイズを更新（書き込み）
		parent_arc.seek(result, static_cast<int>(elem_size));//要素サイズ分（要素の終わりまで）、バッファのカレントポインタを進める
		parent_arc.write(result, ELEM_END, END_MARK_SIZE);//要素終端書き込み
		parent_arc.addResult(result);//親に処理結果を計上
		return !result.hasFatalError();
	}
	
	//配列ブロックフッター書き込み
	bool outputBinaryArchiveFormat::writeArrayFooter(GASHA_ archive::outputArchiveAdapter parent_arc, GASHA_ archive::outputArchiveAdapter child_arc, const GASHA_ serialization::itemInfoBase& item, std::size_t& array_block_size)
	{
		results& result = child_arc.result();
		if (result.hasFatalError())//致命的なエラーが出ている時は即時終了する
		{
			parent_arc.addResult(result);//親に処理結果を計上
			return false;
		}
		array_block_size = child_arc.size();//データサイズ取得
		if (item.isArr())
		{
			const std::uint32_t _array_block_size = static_cast<std::uint32_t>(array_block_size);
			parent_arc.seek(result, -static_cast<int>(sizeof(_array_block_size)));//配列ブロックサイズ情報の分、バッファのカレントポインタを戻す
			parent_arc.write(result, &_array_block_size, sizeof(_array_block_size));//配列ブロックサイズを更新（書き込み）
		}
		parent_arc.seek(result, static_cast<int>(array_block_size));//配列ブロックサイズ分（要素の終わりまで）、バッファのカレントポインタを進める
		if (item.isArr())
		{
			parent_arc.write(result, ARRAY_END, END_MARK_SIZE);//配列ブロック終端書き込み
		}
		parent_arc.addResult(result);//親に処理結果を計上
		return !result.hasFatalError();
	}
	
	//ブロックフッター書き込み
	bool outputBinaryArchiveFormat::writeBlockFooter(GASHA_ archive::outputArchiveAdapter parent_arc, GASHA_ archive::outputArchiveAdapter child_arc, const GASHA_ serialization::itemInfoBase& item, std::size_t& block_size)
	{
		GASHA_ archive::results& result = child_arc.result();
		if (result.hasFatalError())//致命的なエラーが出ている時は即時終了する
		{
			parent_arc.addResult(result);//親に処理結果を計上
			return false;
		}
		block_size = child_arc.size();//データサイズ取得
		const std::uint32_t _block_size = static_cast<std::uint32_t>(block_size);
		parent_arc.seek(result, -static_cast<int>(sizeof(_block_size)));//ブロックサイズ情報の分、バッファのカレントポインタを戻す
		parent_arc.write(result, &_block_size, sizeof(_block_size));//ブロックサイズを更新（書き込み）
		parent_arc.seek(result, static_cast<int>(block_size));//ブロックサイズ分（ブロックの終わりまで）、バッファのカレントポインタを進める
		parent_arc.write(result, BLOCK_END, END_MARK_SIZE);//ブロック終端書き込み
		parent_arc.addResult(result);//親に処理結果を計上
		return !result.hasFatalError();
	}
	
	//ターミネータ書き込み
	bool outputBinaryArchiveFormat::writeTerminator(GASHA_ archive::outputArchiveAdapter arc)
	{
		GASHA_ archive::results& result = arc.result();
		if (result.hasFatalError())//致命的なエラーが出ている時は即時終了する
			return false;
		arc.write(result, TERMINATOR, TERMINATOR_SIZE);//ターミネータ書き込み
		return !result.hasFatalError();
	}

}//namespace archive

GASHA_NAMESPACE_END;//ネームスペース：終了

// End of file
