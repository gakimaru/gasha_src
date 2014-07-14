//--------------------------------------------------------------------------------
// log_work_buff.cpp
// ログワークバッファ【関数／実体定義部】
//
// Gakimaru's researched and standard library for C++ - GASHA
//   Copyright (c) 2014 Itagaki Mamoru
//   Released under the MIT license.
//     https://github.com/gakimaru/gasha/blob/master/LICENSE
//--------------------------------------------------------------------------------

#include <gasha/log_work_buff.inl>//ログワークバッファ【インライン関数／テンプレート関数定義部】

GASHA_NAMESPACE_BEGIN;//ネームスペース：開始

//--------------------------------------------------------------------------------
//ログワークバッファ
//--------------------------------------------------------------------------------

#ifdef GASHA_HAS_DEBUG_LOG//デバッグログ無効時はまるごと無効化

//ワークバッファの取得
char* logWorkBuff::alloc()
{
	static const int spin_count = GASHA_ DEFAULT_SPIN_COUNT;
	int spin_count_now = GASHA_ DEFAULT_SPIN_COUNT;
	while (!m_abort.load())
	{
		char* buff = reinterpret_cast<char*>(m_workBuff.alloc());
		if (buff)
			return buff;
		if (spin_count == 1 || (spin_count > 1 && --spin_count_now == 0))
		{
			contextSwitch();
			spin_count_now = spin_count;
		}
	}
	return nullptr;
}

//初期化メソッド（一回限り）
void logWorkBuff::initializeOnce()
{
	m_abort.store(false);//中断解除
	m_workBuff.clear();//ワークバッファ強制クリア
}

//静的フィールド
const logWorkBuff::explicitInitialize_t logWorkBuff::explicitInitialize;//明示的な初期化指定用
std::once_flag logWorkBuff::m_initialized;//初期化済み
std::atomic<bool> logWorkBuff::m_abort(false);//中断
GASHA_ lfPoolAllocator_withBuff<GASHA_LOG_WORK_BUFF_BLOCK_SIZE, GASHA_LOG_WORK_BUFF_POOL_SIZE> logWorkBuff::m_workBuff;//ワークバッファ

#endif//GASHA_HAS_DEBUG_LOG//デバッグログ無効時はまるごと無効化

GASHA_NAMESPACE_END;//ネームスペース：終了

// End of file
