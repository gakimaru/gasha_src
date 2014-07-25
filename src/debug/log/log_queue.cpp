//--------------------------------------------------------------------------------
// log_queue.cpp
// ログキュー【関数／実体定義部】
//
// Gakimaru's standard library for C++ - GASHA
//   Copyright (c) 2014 Itagaki Mamoru
//   Released under the MIT license.
//     https://github.com/gakimaru/gasha/blob/master/LICENSE
//--------------------------------------------------------------------------------

#include <gasha/log_queue.inl>//ログキュー【インライン関数／テンプレート関数定義部】

#include <gasha/fast_string.h>//高速文字列処理：strlen_fast(), strstrncpy_fast()

GASHA_NAMESPACE_BEGIN;//ネームスペース：開始

//--------------------------------------------------------------------------------
//ログキュー
//--------------------------------------------------------------------------------

#ifdef GASHA_LOG_IS_ENABLED//デバッグログ無効時はまるごと無効化

//エンキュー
bool logQueue::enqueue(const logPrintInfo& print_info)
{
	const char* message = print_info.message();
	if (!message)//messege が nullptr ならエンキュー成功扱い
		return true;

	//メッセージのバッファ割り当て
	char* queue_message = nullptr;
	const std::size_t queue_message_size = print_info.messageSize() > 0 ? print_info.messageSize() : strlen_fast(message) + 1;
	{
		static const int spin_count = GASHA_ DEFAULT_SPIN_COUNT;
		int spin_count_now = spin_count;
		while (!m_abort.load())
		{
			//一時停止中は何もせずループする
			if (m_pause.load())
			{
				GASHA_ contextSwitch(GASHA_ short_sleep_switch);
				continue;
			}

			//メッセージバッファの割り当て
			queue_message = reinterpret_cast<char*>(m_messageBuff.alloc(queue_message_size));
			if (queue_message || IS_NO_WAIT_MODE)
				break;
			
			//メッセージバッファの割り当てができなければリトライ
			if (spin_count == 1 || (spin_count > 1 && --spin_count_now == 0))
			{
				defaultContextSwitch();
				spin_count_now = spin_count;
			}
		}
		if (!queue_message)
			return false;
		std::memcpy(queue_message, message, queue_message_size);
	}

	//メッセージのエンキュー
	logPrintInfo* info = nullptr;
	{
		const id_type id = print_info.id() > 0 ? print_info.id() : reserve(1);
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

			//エンキュー
			logPrintInfo _print_info(print_info);
			_print_info.setId(id);
			_print_info.setMessage(queue_message);
			_print_info.setMessageSize(queue_message_size);
			info = m_queue.push(_print_info);
			if (info || IS_NO_WAIT_MODE)
				break;

			//キューイングできなければリトライ
			if (spin_count == 1 || (spin_count > 1 && --spin_count_now == 0))
			{
				defaultContextSwitch();
				spin_count_now = spin_count;
			}
		}
		//キューイングに失敗したらメッセージを解放して終了
		if (!info)
		{
			m_messageBuff.free(queue_message);
			return false;
		}
	}

	return true;
}

//先頭キューのIDを確認
logQueue::id_type logQueue::top()
{
	auto lock = m_queue.lockScoped();//ロック取得
	const logPrintInfo* top_info = m_queue.top();
	if (!top_info)
		return 0;
	return top_info->id();
}

//初期化メソッド（一回限り）
void logQueue::initializeOnce()
{
	m_abort.store(false);//中断解除
	m_pause.store(false);//一時停止解除
	m_id.store(INIT_ID);//キューID発番用 ※1から始まる
	m_messageBuff.clear();//メッセージバッファ
	m_queue.clear();//ログキュー
}

//静的フィールド
const logQueue::explicitInit_type logQueue::explicitInit;//明示的な初期化指定用
std::once_flag logQueue::m_initialized;//初期化済み
std::atomic<bool> logQueue::m_abort(false);//中断
std::atomic<bool> logQueue::m_pause(false);//一時停止
std::atomic<logQueue::id_type> logQueue::m_id(logQueue::INIT_ID);//キューID発番用
GASHA_ lfSmartStackAllocator_withBuff<logQueue::MESSAGE_BUFF_SIZE> logQueue::m_messageBuff;//メッセージバッファ
GASHA_ binary_heap::container<logQueue::queueOpe, logQueue::QUEUE_SIZE> logQueue::m_queue;//ログキュー

#endif//GASHA_LOG_IS_ENABLED//デバッグログ無効時はまるごと無効化

GASHA_NAMESPACE_END;//ネームスペース：終了

#ifdef GASHA_LOG_IS_ENABLED//デバッグログ無効時はまるごと無効化

//明示的なインスタンス化
#include <gasha/lf_stack_allocator.cpp.h>//ロックフリースタックアロケータ【関数／実体定義定義部】
#include <gasha/binary_heap.cpp.h>//二分ヒープ【関数／実体定義定義部】

GASHA_INSTANCING_lfSmartStackAllocator();
GASHA_INSTANCING_bHeap(GASHA_ logQueue::queueOpe, GASHA_ logQueue::QUEUE_SIZE);

#endif//GASHA_LOG_IS_ENABLED//デバッグログ無効時はまるごと無効化

// End of file
