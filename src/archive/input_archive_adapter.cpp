//--------------------------------------------------------------------------------
// archive/input_archive_adapter.cpp
// アーカイブ/アーカイブ読み込みアダプタ【関数／実体定義部】
//
// Gakimaru's standard library for C++ - GASHA
//   Copyright (c) 2014 Itagaki Mamoru
//   Released under the MIT license.
//     https://github.com/gakimaru/gasha/blob/master/LICENSE
//--------------------------------------------------------------------------------

#include <gasha/archive/input_archive_adapter.inl>//アーカイブ/アーカイブ読み込みアダプタ【インライン関数／テンプレート関数定義部】

GASHA_NAMESPACE_BEGIN;//ネームスペース：開始

//--------------------------------------------------------------------------------
//アーカイブ
namespace archive
{
	//--------------------------------------------------------------------------------
	//アーカイブ/アーカイブ読み込みアダプタ
	//--------------------------------------------------------------------------------

	//--------------------
	//アーカイブ読み込みアダプタクラス

	//バッファからのデータ読み込み
	bool inputArchiveAdapter::read(void* data, const std::size_t size, std::size_t& read_size)
	{
		const std::size_t remain = m_arc.remain();
		read_size = remain > size ? size : remain;
		if (data)//dataがヌルならコピーしないがポインタは進める
			std::memcpy(data, m_arc.m_buff + m_arc.m_buffPos, read_size);
		m_arc.m_buffPos += read_size;
		return read_size == size;
	}
	//※処理結果オブジェクト使用版
	bool inputArchiveAdapter::read(GASHA_ archive::results& result, void* data, const std::size_t size, std::size_t* read_size)
	{
		if (result.hasFatalError())//致命的なエラーが出ている時は即時終了する
			return false;
		//データ読み込み
		std::size_t read_size_tmp = 0;
		const bool result_now = read(data, size, read_size_tmp);
		//処理結果記録
		if (!result_now)
			result.setHasFatalError();
		if (read_size)
			*read_size += read_size_tmp;
		return result_now;
	}
	
	//バッファからのデータ読み込み
	bool inputArchiveAdapter::readWithFunc(GASHA_ serialization::fromMemFunc_type from_mem_func, void* data, const std::size_t dst_size, const std::size_t src_size, std::size_t& read_size)
	{
		const std::size_t remain = m_arc.remain();
		read_size = remain > src_size ? src_size : remain;
		if (data)//dataがヌルならコピーしないがポインタは進める
		{
			from_mem_func(m_arc.m_buff + m_arc.m_buffPos, read_size, data, dst_size);
		}
		m_arc.m_buffPos += read_size;
		return read_size == src_size;
	}
	//※処理結果オブジェクト使用版
	bool inputArchiveAdapter::readWithFunc(GASHA_ archive::results& result, GASHA_ serialization::fromMemFunc_type from_mem_func, void* data, const std::size_t dst_size, const std::size_t src_size, std::size_t* read_size)
	{
		if (result.hasFatalError())//致命的なエラーが出ている時は即時終了する
			return false;
		//データ読み込み
		std::size_t read_size_tmp = 0;
		const bool result_now = readWithFunc(from_mem_func, data, dst_size, src_size, read_size_tmp);
		//処理結果記録
		if (!result_now)
			result.setHasFatalError();
		if (read_size)
			*read_size += read_size_tmp;
		return result_now;
	}

}//namespace archive

GASHA_NAMESPACE_END;//ネームスペース：終了

// End of file
