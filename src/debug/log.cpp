//--------------------------------------------------------------------------------
// log.cpp
// ログ操作【関数／実体定義部】
//
// Gakimaru's researched and standard library for C++ - GASHA
//   Copyright (c) 2014 Itagaki Mamoru
//   Released under the MIT license.
//     https://github.com/gakimaru/gasha/blob/master/LICENSE
//--------------------------------------------------------------------------------

#include <gasha/log.inl>//ログ操作【インライン関数／テンプレート関数定義部】

GASHA_NAMESPACE_BEGIN;//ネームスペース：開始

//--------------------------------------------------------------------------------
//ログ操作
//--------------------------------------------------------------------------------

#ifdef GASHA_LOG_IS_ENABLED//デバッグログ無効時はまるごと無効化

//--------------------
//ログ操作

//ログ出力：書式なし出力
bool log::put(const bool is_reserved, const log::level_type level, const log::category_type category, const char* str)
{
	//ログレベルマスク判定とコンソール取得
	GASHA_ logMask mask;
	GASHA_ logMask::consolesInfo_type masked;
	mask.consolesInfo(masked, level, category);
	if (masked.m_cond == logMask::hasNotAvailableConsole)//利用可能なコンソールがない
		return false;
	else if (masked.m_cond == logMask::everyConsoleIsDummy)//ダミーコンソールしかない
		return true;//成功扱い

	//ログ出力情報作成
	GASHA_ logPrintInfo print_info;
	print_info.setId(is_reserved ? m_reservedId : 0);
	print_info.setTime(nowElapsedTime());
	print_info.setMessage(str);
	print_info.setMessageSize(0);
	print_info.setLevel(level);
	print_info.setCategory(category);
	print_info.setAttr(*GASHA_ logAttr());
	for (purpose_type purpose = 0; purpose < PURPOSE_NUM; ++purpose)
	{
		print_info.setConsole(purpose, masked.m_consoles[purpose]);
		print_info.setColor(purpose, masked.m_colors[purpose]);
	}

	//キューイング
	logQueue queue;
	const bool result = queue.enqueue(print_info);//キューイング

	//予約IDを更新
	if (is_reserved && m_reservedNum > 0)
	{
		--m_reservedNum;
		if (m_reservedNum == 0)
			m_reservedId = 0;
		else
			++m_reservedId;
	}

	//ログキューモニターに通知
	if (result)
	{
		logQueueMonitor mon;
		mon.notify();
	}

	//終了
	return result;
}

//【使用注意】ログ関係の処理を一括して初期化する
void log::initialize()
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
void log::flush()
{
	logQueueMonitor log_queue_monitor;//ログキューモニター
	log_queue_monitor.flush();
}

//【使用注意】ログ関係の処理を一括して中断する
void log::abort()
{
	logWorkBuff log_buff;//ログワークバッファ
	logQueue log_queue;//ログキュー
	logQueueMonitor log_queue_monitor;//ログキューモニター
	log_buff.abort();
	log_queue.abort();
	log_queue_monitor.abort();
}

//【使用注意】ログ関係の処理を一括して一時停止する
void log::pause()
{
	logWorkBuff log_buff;//ログワークバッファ
	logQueue log_queue;//ログキュー
	logQueueMonitor log_queue_monitor;//ログキューモニター
	log_queue.pause();
	log_buff.pause();
	log_queue_monitor.flush();
}

//【使用注意】ログ関係の処理を一括して一時停止解除する
void log::resume()
{
	logWorkBuff log_buff;//ログワークバッファ
	logQueue log_queue;//ログキュー
	log_queue.resume();
	log_buff.resume();
}

#endif//GASHA_LOG_IS_ENABLED//デバッグログ無効時はまるごと無効化

GASHA_NAMESPACE_END;//ネームスペース：終了

// End of file
