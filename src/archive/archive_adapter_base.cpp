//--------------------------------------------------------------------------------
// archive/archive_adapter_adapter_base.cpp
// アーカイブ/アーカイブアダプター基底クラス【関数／実体定義部】
//
// Gakimaru's standard library for C++ - GASHA
//   Copyright (c) 2014 Itagaki Mamoru
//   Released under the MIT license.
//     https://github.com/gakimaru/gasha/blob/master/LICENSE
//--------------------------------------------------------------------------------

#include <gasha/archive/archive_adapter_base.inl>//アーカイブ/アーカイブアダプター基底クラス【インライン関数／テンプレート関数定義部】

GASHA_NAMESPACE_BEGIN;//ネームスペース：開始

//--------------------------------------------------------------------------------
//アーカイブ
namespace archive
{
	//--------------------------------------------------------------------------------
	//アーカイブ/アーカイブアダプター基底クラス
	//--------------------------------------------------------------------------------

	//--------------------
	//アーカイブアダプター基底クラス

	//バッファのカレントポインタを移動
	bool archiveAdapterBase::seek(const int seek_, int& real_seek)
	{
		const int used = static_cast<int>(m_arc.m_buffPos);
		const int remain = static_cast<int>(m_arc.remain());
		real_seek =
			seek_ < 0 ?
				used >(-seek_) ?
					seek_ :
					-used :
				remain > seek_ ?
					seek_ :
					remain;
		m_arc.m_buffPos = static_cast<std::size_t>(used + real_seek);
		return real_seek == seek_;
	}
	//※処理結果オブジェクト使用版
	bool archiveAdapterBase::seek(GASHA_ archive::results& result, const int seek_)
	{
		if (result.hasFatalError())//致命的なエラーが出ている時は即時終了する
			return false;
		int real_seek = 0;
		//カレントポインタ移動
		const bool result_now = seek(seek_, real_seek);
		//処理結果記録
		if (!result_now)
			result.setHasFatalError();
		return result_now;
	}

}//namespace archive

GASHA_NAMESPACE_END;//ネームスペース：終了

// End of file
