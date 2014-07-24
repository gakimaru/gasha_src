//--------------------------------------------------------------------------------
// archive/output_text_archive_format.cpp
// アーカイブ/テキストアーカイブ形式クラス（書き込み用）【関数／実体定義部】
//
// Gakimaru's researched and standard library for C++ - GASHA
//   Copyright (c) 2014 Itagaki Mamoru
//   Released under the MIT license.
//     https://github.com/gakimaru/gasha/blob/master/LICENSE
//--------------------------------------------------------------------------------

#include <gasha/archive/output_text_archive_format.inl>//アーカイブ/テキストアーカイブ形式クラス（書き込み用）【インライン関数／テンプレート関数定義部】

#include <gasha/archive/results.h>//アーカイブ/処理結果

GASHA_NAMESPACE_BEGIN;//ネームスペース：開始

//--------------------------------------------------------------------------------
//アーカイブ
namespace archive
{
	//--------------------------------------------------------------------------------
	//アーカイブ/テキストアーカイブ形式クラス（書き込み用）
	//--------------------------------------------------------------------------------

	//--------------------
	//テキスト形式アーカイブクラス（アーカイブ書き込み用）

	//シグネチャ書き込み
	bool outputTextArchiveFormat::writeSignature(GASHA_ archive::outputArchiveAdapter arc)
	{
		GASHA_ archive::results& result = arc.result();
		if (result.hasFatalError())//致命的なエラーが出ている時は即時終了する
			return false;
		m_process = PROCESS_TOP;
		arc.print(result, "{\"serialization\": {");
		m_blockIndex = 0;
		return !result.hasFatalError();
	}
	
	//ブロックヘッダー書き込み
	bool outputTextArchiveFormat::writeBlockHeader(GASHA_ archive::outputArchiveAdapter arc, const GASHA_ serialization::itemInfoBase& item, const GASHA_ serialization::version& ver)
	{
		GASHA_ archive::results& result = arc.result();
		if (result.hasFatalError())//致命的なエラーが出ている時は即時終了する
			return false;
		if (m_blockIndex != 0 || m_process == PROCESS_ARRAY_BLOCK || (m_process == PROCESS_ELEM && m_itemIndex != 0))
			arc.print(result, ",");
		if (m_process == PROCESS_ELEM)
			++m_itemIndex;
		arc.print(result, "\n");
		m_process = PROCESS_BLOCK;
		arc.printIndent(result, 0);
		arc.print(result, "\"%s\": {", item.m_name);
		arc.print(result, "\"crc\": 0x%08x, ", item.m_nameCrc);
		arc.print(result, "\"itemType\": \"%s\", ", item.m_itemType->name());
		arc.print(result, "\"itemSize\": %d, ", item.m_itemSize);
		arc.print(result, "\"isObj\": %d, ", item.isObj());
		arc.print(result, "\"isArr\": %d, ", item.isArr());
		arc.print(result, "\"isPtr\": %d, ", item.isPtr());
		arc.print(result, "\"isNul\": %d, ", item.isNul());
		arc.print(result, "\"isVLen\": %d, ", item.isVLen());
		arc.print(result, "\"hasVer\": %d, ", item.m_attr.hasVer());
		arc.print(result, "\"ver\": \"%d.%d\"", ver.majorVer(), ver.minorVer());
		m_arrayBlockIndex = 0;
		return !result.hasFatalError();
	}
	
	//配列ブロックヘッダー書き込み
	bool outputTextArchiveFormat::writeArrayHeader(GASHA_ archive::outputArchiveAdapter arc, const GASHA_ serialization::itemInfoBase& item, const std::size_t array_elem_num)
	{
		GASHA_ archive::results& result = arc.result();
		if (result.hasFatalError())//致命的なエラーが出ている時は即時終了する
			return false;
		if (item.isArr())
		{
			arc.print(result, ",");
			arc.print(result, "\n");
			m_process = PROCESS_ARRAY_BLOCK;
			arc.printIndent(result, 0);
			arc.print(result, "\"arrayNum\": %d, \"array\": [", array_elem_num);
			m_elemIndex = 0;
		}
		else
		{
			m_process = PROCESS_ARRAY_BLOCK;
			m_elemIndex = -1;
		}
		return !result.hasFatalError();
	}
	
	//要素ヘッダー書き込み
	bool outputTextArchiveFormat::writeElemHeader(GASHA_ archive::outputArchiveAdapter arc, const GASHA_ serialization::itemInfoBase& item, const std::size_t index)
	{
		GASHA_ archive::results& result = arc.result();
		if (result.hasFatalError())//致命的なエラーが出ている時は即時終了する
			return false;
		if (!item.isArr() || (item.isArr() && m_elemIndex != 0))
			arc.print(result, ",");
		arc.print(result, "\n");
		m_process = PROCESS_ELEM;
		arc.printIndent(result, 0);
		if (item.isArr())
		{
			arc.print(result, "{");
		}
		else
		{
			arc.print(result, "\"elem\": {");
		}
		m_itemIndex = 0;
		return !result.hasFatalError();
	}
	
	//データ項目書き込み
	bool outputTextArchiveFormat::writeDataItem(GASHA_ archive::outputArchiveAdapter arc, const GASHA_ serialization::itemInfoBase& item, const GASHA_ serialization::itemInfoBase& child_item)
	{
		GASHA_ archive::results& result = arc.result();
		if (result.hasFatalError())//致命的なエラーが出ている時は即時終了する
			return false;
		if (m_itemIndex != 0)
			arc.print(result, ",");
		arc.print(result, "\n");
		m_process = PROCESS_ITEM;
		arc.printIndent(result, 0);
		arc.print(result, "\"%s\": {", child_item.m_name);
		arc.print(result, "\"crc\": 0x%08x, ", child_item.m_nameCrc);
		arc.print(result, "\"itemType\": \"%s\", ", child_item.m_itemType->name());
		arc.print(result, "\"itemSize\": %d, ", child_item.m_itemSize);
		arc.print(result, "\"isObj\": %d, ", child_item.isObj());
		arc.print(result, "\"isArr\": %d, ", child_item.isArr());
		arc.print(result, "\"isPtr\": %d, ", child_item.isPtr());
		arc.print(result, "\"isNul\": %d, ", child_item.isNul());
		arc.print(result, "\"isVLen\": %d, ", child_item.isVLen());
		arc.print(result, "\"hasVer\": %d, ", child_item.m_attr.hasVer());
		if (child_item.isArr())
			arc.print(result, "\"arrNum\": %d, ", child_item.m_arrNum);
		arc.print(result, "\"data\": ");
		{
			if (child_item.isNul())
				arc.print(result, "null");
			else
			{
				unsigned char* p = reinterpret_cast<unsigned char*>(const_cast<void*>(child_item.m_itemP));
				const std::size_t elem_num = child_item.extent();
				if (child_item.isArr())
					arc.print(result, "[");
				for (std::size_t index = 0; index < elem_num && !result.hasFatalError(); ++index)//配列要素数分データ書き込み
				{
					if (index != 0)
						arc.print(result, ",");
					arc.printWithFunc(result, child_item.m_typeCtrl.m_toStrFuncP, p, child_item.m_itemSize);
					p += child_item.m_itemSize;
				}
				if (child_item.isArr())
					arc.print(result, "]");
			}
		}
		arc.print(result, "}");
		++m_itemIndex;
		m_process = m_parentProcess;
		return !result.hasFatalError();
	}
	
	//要素フッター書き込み
	bool outputTextArchiveFormat::writeElemFooter(GASHA_ archive::outputArchiveAdapter parent_arc, GASHA_ archive::outputArchiveAdapter child_arc, const GASHA_ serialization::itemInfoBase& item, const std::size_t index, std::size_t& items_num, std::size_t& elem_size)
	{
		GASHA_ archive::results& result = child_arc.result();
		if (result.hasFatalError())//致命的なエラーが出ている時は即時終了する
		{
			parent_arc.addResult(result);//親に処理結果を計上
			return false;
		}
		child_arc.print(result, "}");
		parent_arc.seek(result, static_cast<int>(child_arc.size()));
		parent_arc.addResult(result);//親に処理結果を計上
		if (m_parent)
			++m_parent->m_elemIndex;
		m_process = m_parentProcess;
		return !result.hasFatalError();
	}
	
	//配列ブロックフッター書き込み
	bool outputTextArchiveFormat::writeArrayFooter(GASHA_ archive::outputArchiveAdapter parent_arc, GASHA_ archive::outputArchiveAdapter child_arc, const GASHA_ serialization::itemInfoBase& item, std::size_t& array_block_size)
	{
		GASHA_ archive::results& result = child_arc.result();
		if (result.hasFatalError())//致命的なエラーが出ている時は即時終了する
		{
			parent_arc.addResult(result);//親に処理結果を計上
			return false;
		}
		if (item.isArr())
		{
			child_arc.print(result, "]");
		}
		parent_arc.seek(result, static_cast<int>(child_arc.size()));
		parent_arc.addResult(result);//親に処理結果を計上
		if (m_parent)
			++m_parent->m_arrayBlockIndex;
		m_process = m_parentProcess;
		return !result.hasFatalError();
	}
	
	//ブロックフッター書き込み
	bool outputTextArchiveFormat::writeBlockFooter(GASHA_ archive::outputArchiveAdapter parent_arc, GASHA_ archive::outputArchiveAdapter child_arc, const GASHA_ serialization::itemInfoBase& item, std::size_t& block_size)
	{
		GASHA_ archive::results& result = child_arc.result();
		if (result.hasFatalError())//致命的なエラーが出ている時は即時終了する
		{
			parent_arc.addResult(result);//親に処理結果を計上
			return false;
		}
		child_arc.print(result, "}");
		parent_arc.seek(result, static_cast<int>(child_arc.size()));
		parent_arc.addResult(result);//親に処理結果を計上
		if (m_parent)
			++m_parent->m_blockIndex;
		m_process = m_parentProcess;
		return !result.hasFatalError();
	}
	
	//ターミネータ書き込み
	bool outputTextArchiveFormat::writeTerminator(GASHA_ archive::outputArchiveAdapter arc)
	{
		results& result = arc.result();
		if (result.hasFatalError())//致命的なエラーが出ている時は即時終了する
			return false;
		arc.print(result, "\n");
		arc.print(result, "}, \"terminator\": \"ok\"}\n");
		return !result.hasFatalError();
	}

}//namespace archive

GASHA_NAMESPACE_END;//ネームスペース：終了

// End of file
