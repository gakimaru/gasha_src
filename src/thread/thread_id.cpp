//--------------------------------------------------------------------------------
// thread_id.cpp
// スレッドID【関数／実体定義部】
//
// Gakimaru's standard library for C++ - GASHA
//   Copyright (c) 2014 Itagaki Mamoru
//   Released under the MIT license.
//     https://github.com/gakimaru/gasha/blob/master/LICENSE
//--------------------------------------------------------------------------------

#include <gasha/thread_id.inl>//スレッドID【インライン関数／テンプレート関数定義部】

GASHA_NAMESPACE_BEGIN;//ネームスペース：開始

//--------------------------------------------------------------------------------
//スレッドID
//--------------------------------------------------------------------------------

//----------------------------------------
//スレッドIDクラス

//現在のスレッドのスレッドIDをセット
void threadId::setThisId() const
{
	if (!m_thisIdIsInitialized)
	{
		m_thisIdIsInitialized = true;
		m_thisId = std::hash<std::thread::id>()(std::this_thread::get_id());
		m_thisName = "(no name)";
		m_thisNameCrc = 0;
	}
}
//現在のスレッドのスレッド名をセット
void threadId::setThisName(const char* name) const
{
	setThisId();
	m_thisName = name;
	m_thisNameCrc = GASHA_ calcCRC32(name);
}

//静的フィールドのインスタンス化
thread_local bool threadId::m_thisIdIsInitialized = false;
thread_local threadId::id_type threadId::m_thisId = threadId::INITIAL_ID;
thread_local const char* threadId::m_thisName = "(no name)";
thread_local GASHA_ crc32_t threadId::m_thisNameCrc = 0;

GASHA_NAMESPACE_END;//ネームスペース：終了

// End of file
