//--------------------------------------------------------------------------------
// archive/output_archive_adapter.cpp
// アーカイブ/アーカイブ書き込みアダプタークラス【関数／実体定義部】
//
// Gakimaru's researched and standard library for C++ - GASHA
//   Copyright (c) 2014 Itagaki Mamoru
//   Released under the MIT license.
//     https://github.com/gakimaru/gasha/blob/master/LICENSE
//--------------------------------------------------------------------------------

#include <gasha/archive/output_archive_adapter.inl>//アーカイブ/アーカイブ書き込みアダプタークラス【インライン関数／テンプレート関数定義部】

#include <gasha/string.h>//文字列処理

#include <utility>//C++11 std::forward

GASHA_NAMESPACE_BEGIN;//ネームスペース：開始

//--------------------------------------------------------------------------------
//アーカイブ
namespace archive
{
	//--------------------------------------------------------------------------------
	//アーカイブ/アーカイブ書き込みアダプタークラス
	//--------------------------------------------------------------------------------

	//--------------------
	//アーカイブ書き込みアダプタークラス

	//バッファへのデータ書き込み
	bool outputArchiveAdapter::write(const void* data, const std::size_t size, std::size_t& written_size)
	{
		const std::size_t remain = m_arc.remain();
		written_size = remain > size ? size : remain;
		if (data)//データがヌルならサイズ分0で埋める
			std::memcpy(m_arc.m_buff + m_arc.m_buffPos, data, written_size);
		else//データコピー
			std::memset(m_arc.m_buff + m_arc.m_buffPos, 0, written_size);
		m_arc.m_buffPos += written_size;
		return written_size == size;
	}
	//※処理結果オブジェクト使用版
	bool outputArchiveAdapter::write(GASHA_ archive::results& result, const void* data, const std::size_t size, std::size_t* written_size)
	{
		if (result.hasFatalError())//致命的なエラーが出ている時は即時終了する
			return false;
		//データ書き込み
		std::size_t written_size_tmp = 0;
		const bool result_now = write(data, size, written_size_tmp);
		//処理結果記録
		result.addCopiedSize(written_size_tmp);
		if (!result_now)
			result.setHasFatalError();
		if (written_size)
			*written_size += written_size_tmp;
		return result_now;
	}
	//バッファへの文字列書き込み
	std::size_t outputArchiveAdapter::printWithFunc(GASHA_ archive::results& result, GASHA_ serialization::toStrFunc_type to_str_func, const void* data_p, const std::size_t data_size)
	{
		if (result.hasFatalError())//致命的なエラーが出ている時は即時終了する
			return false;
		const std::size_t remain = m_arc.remain();
		const std::size_t ret = to_str_func(reinterpret_cast<char*>(m_arc.m_buff + m_arc.m_buffPos), remain, data_p, data_size);
		m_arc.m_buffPos += ret;
		return ret;
	}
	//バッファへのインデント書き込み
	void outputArchiveAdapter::printIndent(GASHA_ archive::results& result, const int add)
	{
		if (result.hasFatalError())//致命的なエラーが出ている時は即時終了する
			return;
		const int remain = static_cast<int>(m_arc.remain());
		int space_len = (static_cast<int>(m_arc.m_nestLevel) + add) * 2 + 1;
		space_len = space_len > 0 ? space_len : 0;
		space_len = space_len < remain ? space_len : remain;
		if (space_len >= 1)
		{
			std::memset(m_arc.m_buff + m_arc.m_buffPos, ' ', space_len);
			m_arc.m_buffPos += space_len;
			*(m_arc.m_buff + m_arc.m_buffPos) = '\0';
		}
	}

}//namespace archive

GASHA_NAMESPACE_END;//ネームスペース：終了

// End of file
