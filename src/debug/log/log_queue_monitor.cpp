//--------------------------------------------------------------------------------
// log_queue_monitor.cpp
// ログキューモニター【関数／実体定義部】
//
// Gakimaru's researched and standard library for C++ - GASHA
//   Copyright (c) 2014 Itagaki Mamoru
//   Released under the MIT license.
//     https://github.com/gakimaru/gasha/blob/master/LICENSE
//--------------------------------------------------------------------------------

#include <gasha/log_queue_monitor.inl>//ログキューモニター【インライン関数／テンプレート関数定義部】

#include <gasha/lock_common.h>//ロック共通設定

GASHA_NAMESPACE_BEGIN;//ネームスペース：開始

//--------------------------------------------------------------------------------
//ログキューモニター
//--------------------------------------------------------------------------------

#ifdef GASHA_LOG_IS_ENABLED//デバッグログ無効時はまるごと無効化

//キューイングを通知
bool logQueueMonitor::notify()
{
	//中断時は即終了
	if (m_abort.load() || m_isEnd.load())
		return false;

	//要求数をカウントアップ
	{
		std::lock_guard<std::mutex> lock(m_mutex);
		m_request.fetch_add(1);
	}

	//通知
	m_cond.notify_one();

	return true;
}

//ID欠番通知
bool logQueueMonitor::skip(const logQueueMonitor::id_type skip_id)
{
	const id_type new_next_id = skip_id + 1;
	id_type next_id = m_nextId.load();//現在の「次のID」を取得
	while (next_id < new_next_id)
	{
		if (m_nextId.compare_exchange_weak(next_id, new_next_id))//他のスレッドによって処理が進行していなければ値を置き換える
			return true;
		next_id = m_nextId.load();//失敗したら再確認
	}
	return false;
}

//中断
void logQueueMonitor::abort()
{
	//すでに中断時は即終了
	if (m_abort.load() || m_isEnd.load())
		return;

	//中断
	m_abort.store(true);

	//通知
	m_cond.notify_one();

	//終了待ち
	static const int spin_count = GASHA_ DEFAULT_SPIN_COUNT;
	int spin_count_now = spin_count;
	while (!m_isEnd.load())
	{
		if (spin_count <= 1 || (spin_count > 1 && --spin_count_now == 0))//※DEFAULT_SPIN_COUNT が 0 でもコンテキストスイッチする
		{
			//再通知
			m_cond.notify_one();
			
			//コンテキストスイッチ
			defaultContextSwitch();
			spin_count_now = spin_count;
		}
	}
}

//フラッシュ
//※すべての出力が完了するのを待つ
//※中断時は即終了する
bool logQueueMonitor::flush()
{
	//中断時は即終了
	if (m_abort.load() || m_isEnd.load())
		return false;

	//フラッシュ要求数をカウントアップ
	{
		std::lock_guard<std::mutex> lock(m_mutex);
		m_flush.fetch_add(1);
	}

	//通知
	m_cond.notify_one();

	//終了待ち
	static const int spin_count = GASHA_ DEFAULT_SPIN_COUNT;
	int spin_count_now = spin_count;
	while (!m_abort.load() && !m_isEnd.load() && (m_request.load() > 0 || m_flush.load() > 0))
	{
		if (spin_count <= 1 || (spin_count > 1 && --spin_count_now == 0))//※DEFAULT_SPIN_COUNT が 0 でもコンテキストスイッチする
		{
			//再通知
			m_cond.notify_one();
			
			//コンテキストスイッチ
			defaultContextSwitch();
			spin_count_now = spin_count;
		}
	}

	return !m_abort.load();
}

//初期化メソッド（一回限り）
void logQueueMonitor::initializeOnce()
{
	m_abort.store(false);//中断解除
	m_isEnd.store(false);//終了状態断解除
	m_request.store(0);//要求数
	m_flush.store(0);//フラッシュ要求数
	m_nextId.store(1);//次のID
}

//静的フィールド
const logQueueMonitor::explicitInit_type logQueueMonitor::explicitInit;//明示的な初期化指定用
std::once_flag logQueueMonitor::m_initialized;//初期化済み
std::atomic<bool> logQueueMonitor::m_abort(false);//中断
std::atomic<bool> logQueueMonitor::m_isEnd(false);//終了状態
std::atomic<std::int32_t> logQueueMonitor::m_request(0);//要求数
std::atomic<std::int32_t> logQueueMonitor::m_flush(0);//フラッシュ要求数
std::atomic<logQueueMonitor::id_type> logQueueMonitor::m_nextId(logQueueMonitor::INIT_ID);//次のID
std::mutex logQueueMonitor::m_mutex;//ミューテックス
std::condition_variable logQueueMonitor::m_cond;//条件変数

#endif//GASHA_LOG_IS_ENABLED//デバッグログ無効時はまるごと無効化

GASHA_NAMESPACE_END;//ネームスペース：終了

#ifdef GASHA_LOG_IS_ENABLED//デバッグログ無効時はまるごと無効化

#endif//GASHA_LOG_IS_ENABLED//デバッグログ無効時はまるごと無効化

// End of file
