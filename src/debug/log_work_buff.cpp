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

#include <gasha/lf_pool_allocator.cpp.h>//ロックフリープールアロケータ【関数／実体定義定義部】

GASHA_NAMESPACE_BEGIN;//ネームスペース：開始

//--------------------------------------------------------------------------------
//ログワークバッファ
//--------------------------------------------------------------------------------

#ifdef GASHA_LOG_IS_ENABLED//デバッグログ無効時はまるごと無効化

//ワークバッファの取得
char* logWorkBuff::alloc()
{
	static const int spin_count = GASHA_ DEFAULT_SPIN_COUNT;
	int spin_count_now = spin_count;
	while (!m_abort.load())
	{
		//一時停止中は何もせずループする
		if (m_pause.load())
		{
			GASHA_ defaultContextSwitch();
			continue;
		}

		//ワークバッファが取得
		char* buff = reinterpret_cast<char*>(m_workBuff.alloc());
		if (buff)
			return buff;

		//ワークバッファが取得できなければリトライ
		if (spin_count == 1 || (spin_count > 1 && --spin_count_now == 0))
		{
			defaultContextSwitch();
			spin_count_now = spin_count;
		}
	}
	return nullptr;
}

//strcpy
std::size_t logWorkBuff::strcpy(char* message, std::size_t& pos, const char* src)
{
	std::size_t remain = MAX_MESSAGE_SIZE - pos;
	const std::size_t src_size = GASHA_ strlen_fast(src);
	std::size_t ret = 0;
	std::size_t copy_size;
	if (src_size < remain)
	{
		copy_size = src_size;
		ret = src_size;
	}
	else
		copy_size = remain - 1;
	std::memcpy(message + pos, src, copy_size);
	message[copy_size] = '\0';
	return ret;
}

//初期化メソッド（一回限り）
void logWorkBuff::initializeOnce()
{
	m_abort.store(false);//中断解除
	m_pause.store(false);//一時停止解除
	m_workBuff.clear();//ワークバッファ強制クリア
}

//静的フィールド
const logWorkBuff::explicitInit_type logWorkBuff::explicitInit;//明示的な初期化指定用
std::once_flag logWorkBuff::m_initialized;//初期化済み
std::atomic<bool> logWorkBuff::m_abort(false);//中断
std::atomic<bool> logWorkBuff::m_pause(false);//一時停止
GASHA_ lfPoolAllocator_withBuff<logWorkBuff::MAX_MESSAGE_SIZE, logWorkBuff::MESSAGE_POOL_SIZE> logWorkBuff::m_workBuff;//ワークバッファ

#endif//GASHA_LOG_IS_ENABLED//デバッグログ無効時はまるごと無効化

GASHA_NAMESPACE_END;//ネームスペース：終了

// End of file
