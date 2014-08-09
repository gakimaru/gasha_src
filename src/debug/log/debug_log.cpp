//--------------------------------------------------------------------------------
// debug_log.cpp
// ログ操作【関数／実体定義部】
//
// Gakimaru's standard library for C++ - GASHA
//   Copyright (c) 2014 Itagaki Mamoru
//   Released under the MIT license.
//     https://github.com/gakimaru/gasha/blob/master/LICENSE
//--------------------------------------------------------------------------------

#include <gasha/debug_log.inl>//ログ操作【インライン関数／テンプレート関数定義部】

GASHA_NAMESPACE_BEGIN;//ネームスペース：開始

//--------------------------------------------------------------------------------
//ログ操作
//--------------------------------------------------------------------------------

#ifdef GASHA_LOG_IS_ENABLED//デバッグログ無効時はまるごと無効化

//--------------------
//ログ操作

//【使用注意】ログ関係の処理を一括して初期化する
void debugLog::initialize()
{
	logLevelContainer level_con(logLevelContainer::explicitInit);//ログレベルの明示的な初期化 ※既に初期化済みなら何もしない
	logCategoryContainer cate_con(logCategoryContainer::explicitInit);//ログカテゴリの明示的な初期化 ※既に初期化済みなら何もしない
	logMask mask(logMask::explicitInit);//ログレベルマスクの明示的な初期化 ※既に初期化済みなら何もしない
	logAttr attr(logAttr::explicitInit);//ログ属性の明示的な初期化 ※既に初期化済みなら何もしない
	logWorkBuff log_buff(logWorkBuff::explicitInit);//ログワークバッファの明示的な初期化 ※【注意】強制初期化
	logQueue log_queue(logQueue::explicitInit);//ログキューの明示的な初期化 ※【注意】強制初期化
	logQueueMonitor log_queue_monitor(logQueueMonitor::explicitInit);//ログキューモニターの明示的な初期化 ※【注意】強制初期化
}

//【使用注意】ログキューモニターに溜まっているキューを全て出力する
void debugLog::flush()
{
	logQueueMonitor log_queue_monitor;//ログキューモニター
	log_queue_monitor.flush();
}

//【使用注意】ログ関係の処理を一括して中断する
void debugLog::abort()
{
	logWorkBuff log_buff;//ログワークバッファ
	logQueue log_queue;//ログキュー
	logQueueMonitor log_queue_monitor;//ログキューモニター
	log_buff.abort();
	log_queue.abort();
	log_queue_monitor.abort();
}

//【使用注意】ログ関係の処理を一括して一時停止する
void debugLog::pause()
{
	logWorkBuff log_buff;//ログワークバッファ
	logQueue log_queue;//ログキュー
	logQueueMonitor log_queue_monitor;//ログキューモニター
	log_queue.pause();
	log_buff.pause();
	log_queue_monitor.flush();
}

//【使用注意】ログ関係の処理を一括して一時停止解除する
void debugLog::resume()
{
	logWorkBuff log_buff;//ログワークバッファ
	logQueue log_queue;//ログキュー
	log_queue.resume();
	log_buff.resume();
}

#else//GASHA_LOG_IS_ENABLED

//【VC++】LNK4221回避用のダミー関数
namespace _private{
	void debug_log_dummy(){}
}//namespace _private

#endif//GASHA_LOG_IS_ENABLED//デバッグログ無効時はまるごと無効化

GASHA_NAMESPACE_END;//ネームスペース：終了

// End of file
