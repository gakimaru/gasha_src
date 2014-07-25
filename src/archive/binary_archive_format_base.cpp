//--------------------------------------------------------------------------------
// archive/binary_archive_format_base.cpp
// アーカイブ/バイナリアーカイブ形式基底クラス【関数／実体定義部】
//
// Gakimaru's standard library for C++ - GASHA
//   Copyright (c) 2014 Itagaki Mamoru
//   Released under the MIT license.
//     https://github.com/gakimaru/gasha/blob/master/LICENSE
//--------------------------------------------------------------------------------

#include <gasha/archive/binary_archive_format_base.inl>//アーカイブ/バイナリアーカイブ形式基底クラス【インライン関数／テンプレート関数定義部】

GASHA_NAMESPACE_BEGIN;//ネームスペース：開始

//--------------------------------------------------------------------------------
//アーカイブ
namespace archive
{
	//--------------------------------------------------------------------------------
	//アーカイブ/バイナリアーカイブ形式基底クラス
	//--------------------------------------------------------------------------------

	//--------------------
	//バイナリ形式アーカイブクラス（共通）：静的変数インスタンス化
	const unsigned char binaryArchiveFormatBase::SIGNATURE[binaryArchiveFormatBase::SIGNATURE_SIZE] = { 0x00, 0xff, 's', 'e', 'r', 'i', 'a', 'l', ':', ':', 'C', 'B', 'i', 'n', 0xff, 0x00 };//シグネチャ
	const unsigned char binaryArchiveFormatBase::TERMINATOR[binaryArchiveFormatBase::TERMINATOR_SIZE] = { 0xff, 0x00, 's', 'e', 'r', 'i', 'a', 'l', ':', ':', 'C', 'B', 'i', 'n', 0x00, 0xff };//ターミネータ
	const unsigned char binaryArchiveFormatBase::BLOCK_BEGIN[binaryArchiveFormatBase::BEGIN_MARK_SIZE] = { 0xfb, 0xff };//ブロック始端
	const unsigned char binaryArchiveFormatBase::BLOCK_END[binaryArchiveFormatBase::END_MARK_SIZE] = { 0xff, 0xfb };//ブロック終端
	const unsigned char binaryArchiveFormatBase::ARRAY_BEGIN[binaryArchiveFormatBase::BEGIN_MARK_SIZE] = { 0xfa, 0xff };//配列始端
	const unsigned char binaryArchiveFormatBase::ARRAY_END[binaryArchiveFormatBase::END_MARK_SIZE] = { 0xff, 0xfa };//配列終端
	const unsigned char binaryArchiveFormatBase::ELEM_BEGIN[binaryArchiveFormatBase::BEGIN_MARK_SIZE] = { 0xfe, 0xff };//要素始端
	const unsigned char binaryArchiveFormatBase::ELEM_END[binaryArchiveFormatBase::END_MARK_SIZE] = { 0xff, 0xfe };//要素終端
	const unsigned char binaryArchiveFormatBase::ITEM_BEGIN[binaryArchiveFormatBase::BEGIN_MARK_SIZE] = { 0xfd, 0xff };//データ項目始端
	const unsigned char binaryArchiveFormatBase::ITEM_END[binaryArchiveFormatBase::END_MARK_SIZE] = { 0xff, 0xfd };//データ項目終端

}//namespace archive

GASHA_NAMESPACE_END;//ネームスペース：終了

// End of file
