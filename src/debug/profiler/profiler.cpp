//--------------------------------------------------------------------------------
// profiler.cpp
// プロファイラー【関数／実体定義部】
//
// Gakimaru's standard library for C++ - GASHA
//   Copyright (c) 2014 Itagaki Mamoru
//   Released under the MIT license.
//     https://github.com/gakimaru/gasha/blob/master/LICENSE
//--------------------------------------------------------------------------------

#include <gasha/profiler.inl>//プロファイラー【インライン関数／テンプレート関数定義部】

#include <gasha/fast_string.h>//高速文字列処理
#include <gasha/crc32.h>//CRC32計算
#include <gasha/iterator.h>//イテレータ操作
#include <gasha/utility.h>//汎用ユーティリティ
#include <gasha/simple_assert.h>//シンプルアサーション
#include <gasha/intro_sort.h>//イントロソート

#pragma warning(push)//【VC++】ワーニング設定を退避
#pragma warning(disable: 4530)//【VC++】C4530を抑える
#include <functional>//C++11 std::fucntion
#pragma warning(pop)//【VC++】ワーニング設定を復元

GASHA_NAMESPACE_BEGIN;//ネームスペース：開始

//--------------------------------------------------------------------------------
//プロファイラー
//--------------------------------------------------------------------------------

#ifdef GASHA_PROFILE_IS_AVAILABLE//プロファイル機能無効時はまるごと無効化

//----------------------------------------
//処理時間情報

//処理時間加算
bool profiler::timeInfo::add(const GASHA_ sec_t time)
{
	++m_count;
	m_time += time;
	//m_avgTime = m_time / static_cast<GASHA_ sec_t>(m_count);//平均の算出は取得時に行う
	m_avgTime = static_cast<GASHA_ sec_t>(0.f);
	m_maxTime = GASHA_ maxIf(m_maxTime, time);
	m_minTime = GASHA_ minIf(m_minTime, time);
	return true;
}

//集計
bool profiler::timeInfo::sumup(const profiler::timeInfo& time_info)
{
	m_count += time_info.m_count;
	m_time += time_info.m_time;
	//m_avgTime = m_time / static_cast<GASHA_ sec_t>(m_count);//平均の算出は取得時に行う
	m_avgTime = static_cast<GASHA_ sec_t>(0.f);
	m_maxTime = GASHA_ maxIf(m_maxTime, time_info.m_maxTime);
	m_minTime = GASHA_ minIf(m_minTime, time_info.m_minTime);
	return true;
}

//----------------------------------------
//処理時間集計情報

//集計
bool profiler::summarizedTimeInfo::sumup(const profiler::timeInfo& time_info)
{
	m_time.sumup(time_info);
	++m_summarizedCount;
	//m_avgCount = static_cast<float>(m_time.m_count) / static_cast<float>(m_summarizedCount);//平均の算出は取得時に行う
	m_avgCount = 0.f;
	m_maxCount = GASHA_ maxIf(m_maxCount, time_info.m_count);
	m_minCount = GASHA_ minIf(m_minCount, time_info.m_count);
	return true;
}

//----------------------------------------
//プロファイル情報

//処理時間加算
bool profiler::profileInfo::add(const GASHA_ sec_t time)
{
	auto lock = m_lock.lockScoped();//排他ロック
	return m_time.add(time);
}

//処理時間集計
bool profiler::profileInfo::sumup(const profileSumup_type type)
{
	auto lock = m_lock.lockScoped();//排他ロック
	m_totalTime.sumup(m_time);//全体集計を更新
	m_periodicTimeWork.sumup(m_time);//期間集計（ワーク）を更新
	m_time.clear();
	if (type == withUpdatePeriod)//期間集計を反映
	{
		m_periodicTime = m_periodicTimeWork;
		m_periodicTimeWork.clear();
	}
	return true;
}

//----------------------------------------
//スレッド情報

//プロファイル情報登録
profiler::profileInfo* profiler::threadInfo::regProfile(const GASHA_ crc32_t name_crc, const char* name, profileInfoPool_type& profile_info_pool)
{
	//登録済みなら共有ロックで情報を取得
	{
		auto lock = m_profileInfoTree.lockSharedScoped();//共有ロック
		//プロファイル情報を取得
		profileInfo* profile_info = m_profileInfoTree.at(name_crc);
		if (profile_info)
			return profile_info;
	}
	//未登録なら排他ロックで登録
	{
		auto lock = m_profileInfoTree.lockScoped();//排他ロック
		//他のスレッドが先に登録した可能性があるので再チェック
		profileInfo* profile_info = m_profileInfoTree.at(name_crc);
		if (profile_info)
			return profile_info;
		//プロファイル情報プールからプロファイル情報を生成
		profile_info = profile_info_pool.newDefault(name_crc, name);
		GASHA_SIMPLE_ASSERT(profile_info != nullptr, "profile_info_pool is not enough memory.");
		if (!profile_info)
			return nullptr;
		//プロファイル情報を登録
		//※排他ロックしているので結果の確認とリトライは不要
		return m_profileInfoTree.insert(*profile_info);
	}
}

//処理時間加算
bool profiler::threadInfo::add(const strPoolInfo& pooled_name, const GASHA_ sec_t elapsed_time, profileInfoPool_type& profile_info_pool)
{
	//プロファイル情報を登録
	profileInfo* profile_info = regProfile(pooled_name.m_strCrc, pooled_name.m_str, profile_info_pool);
	if (!profile_info)
		return false;
	//処理時間を加算
	return profile_info->add(elapsed_time);
}

//処理時間集計
bool profiler::threadInfo::sumup(const profileSumup_type type)
{
	auto lock = m_profileInfoTree.lockScoped();//排他ロック
	GASHA_ forEach(m_profileInfoTree, [&type](profileInfo& profile_info)
		{
			profile_info.sumup(type);
		}
	);
	return true;
}

//プロファイル情報を取得
std::size_t profiler::threadInfo::getProfileInfo(profiler::profileInfo* array, const std::size_t max_size, const profiler::profileOrder_type order)
{
	std::size_t copy_size = 0;
	{
		auto lock = m_profileInfoTree.lockSharedScoped();//共有ロック
		GASHA_ forEach(m_profileInfoTree, [&copy_size, &array, max_size](profileInfo& profile_info)
			{
				if (copy_size < max_size)
				{
					auto lock = profile_info.m_lock.lockSharedScoped();//共有ロック
					*(array + copy_size) = profile_info;
					++copy_size;
				}
			}
		);
	}

	//平均値計算
	{
		profiler::profileInfo* info = array;
		for (std::size_t i = 0; i < copy_size; ++i, ++info)
		{
			info->m_periodicTime.calcAvg();
			info->m_totalTime.calcAvg();
		}
	}

	//ソート
	std::function<bool(const profileInfo&, const profileInfo&)> less_pred = nullptr;
	switch (order)
	{
	case unordered:
		//ソートなし
		break;
	
	//全体集計処理時間でソート
	case ascOfTotalTime://処理時間昇順
		less_pred = [](const profileInfo& lhs, const profileInfo& rhs) -> bool
			{ return lhs.m_totalTime.m_time.m_time < rhs.m_totalTime.m_time.m_time; };
		break;
	case descOfTotalTime://処理時間降順
		less_pred = [](const profileInfo& lhs, const profileInfo& rhs) -> bool
			{ return lhs.m_totalTime.m_time.m_time > rhs.m_totalTime.m_time.m_time; };
		break;
	case ascOfAvgTotalTime://平均処理時間昇順
		less_pred = [](const profileInfo& lhs, const profileInfo& rhs) -> bool
			{ return lhs.m_totalTime.m_time.m_avgTime < rhs.m_totalTime.m_time.m_avgTime; };
		break;
	case descOfAvgTotalTime://平均処理時間降順
		less_pred = [](const profileInfo& lhs, const profileInfo& rhs) -> bool
			{ return lhs.m_totalTime.m_time.m_avgTime > rhs.m_totalTime.m_time.m_avgTime; };
		break;
	case ascOfMaxTotalTime://最長処理時間昇順
		less_pred = [](const profileInfo& lhs, const profileInfo& rhs) -> bool
			{ return lhs.m_totalTime.m_time.m_maxTime < rhs.m_totalTime.m_time.m_maxTime; };
		break;
	case descOfMaxTotalTime://最長処理時間降順 ※デフォルト
		less_pred = [](const profileInfo& lhs, const profileInfo& rhs) -> bool
			{ return lhs.m_totalTime.m_time.m_maxTime > rhs.m_totalTime.m_time.m_maxTime; };
		break;
	case ascOfMinTotalTime://最短処理時間昇順
		less_pred = [](const profileInfo& lhs, const profileInfo& rhs) -> bool
			{ return lhs.m_totalTime.m_time.m_minTime < rhs.m_totalTime.m_time.m_minTime; };
		break;
	case descOfMinTotalTime://最短処理時間降順
		less_pred = [](const profileInfo& lhs, const profileInfo& rhs) -> bool
			{ return lhs.m_totalTime.m_time.m_minTime > rhs.m_totalTime.m_time.m_minTime; };
		break;
	case ascOfTotalCount://計測回数昇順
		less_pred = [](const profileInfo& lhs, const profileInfo& rhs) -> bool
			{ return lhs.m_totalTime.m_time.m_count < rhs.m_totalTime.m_time.m_count; };
		break;
	case descOfTotalCount://計測回数降順
		less_pred = [](const profileInfo& lhs, const profileInfo& rhs) -> bool
			{ return lhs.m_totalTime.m_time.m_count > rhs.m_totalTime.m_time.m_count; };
		break;
	case ascOfMaxTotalCount://最大計測回数昇順
		less_pred = [](const profileInfo& lhs, const profileInfo& rhs) -> bool
			{ return lhs.m_totalTime.m_maxCount < rhs.m_totalTime.m_maxCount; };
		break;
	case descOfMaxTotalCount://最大計測回数降順
		less_pred = [](const profileInfo& lhs, const profileInfo& rhs) -> bool
			{ return lhs.m_totalTime.m_maxCount > rhs.m_totalTime.m_maxCount; };
		break;
	case ascOfMinTotalCount://最小計測回数昇順
		less_pred = [](const profileInfo& lhs, const profileInfo& rhs) -> bool
			{ return lhs.m_totalTime.m_minCount < rhs.m_totalTime.m_minCount; };
		break;
	case descOfMinTotalCount://最小計測回数降順
		less_pred = [](const profileInfo& lhs, const profileInfo& rhs) -> bool
			{ return lhs.m_totalTime.m_minCount > rhs.m_totalTime.m_minCount; };
		break;
	case ascOfAvgTotalCount://平均計測回数昇順
		less_pred = [](const profileInfo& lhs, const profileInfo& rhs) -> bool
			{ return lhs.m_totalTime.m_avgCount < rhs.m_totalTime.m_avgCount; };
		break;
	case descOfAvgTotalCount://平均計測回数降順
		less_pred = [](const profileInfo& lhs, const profileInfo& rhs) -> bool
			{ return lhs.m_totalTime.m_avgCount > rhs.m_totalTime.m_avgCount; };
		break;

	//期間集計処理時間でソート
	case ascOfPeriodicTime://処理時間昇順
		less_pred = [](const profileInfo& lhs, const profileInfo& rhs) -> bool
			{ return lhs.m_periodicTime.m_time.m_time < rhs.m_periodicTime.m_time.m_time; };
		break;
	case descOfPeriodicTime://処理時間降順
		less_pred = [](const profileInfo& lhs, const profileInfo& rhs) -> bool
			{ return lhs.m_periodicTime.m_time.m_time > rhs.m_periodicTime.m_time.m_time; };
		break;
	case ascOfAvgPeriodicTime://平均処理時間昇順
		less_pred = [](const profileInfo& lhs, const profileInfo& rhs) -> bool
			{ return lhs.m_periodicTime.m_time.m_avgTime < rhs.m_periodicTime.m_time.m_avgTime; };
		break;
	case descOfAvgPeriodicTime://平均処理時間降順
		less_pred = [](const profileInfo& lhs, const profileInfo& rhs) -> bool
			{ return lhs.m_periodicTime.m_time.m_avgTime > rhs.m_periodicTime.m_time.m_avgTime; };
		break;
	case ascOfMaxPeriodicTime://最長処理時間昇順
		less_pred = [](const profileInfo& lhs, const profileInfo& rhs) -> bool
			{ return lhs.m_periodicTime.m_time.m_maxTime < rhs.m_periodicTime.m_time.m_maxTime; };
		break;
	case descOfMaxPeriodicTime://最長処理時間降順 ※デフォルト
		less_pred = [](const profileInfo& lhs, const profileInfo& rhs) -> bool
			{ return lhs.m_periodicTime.m_time.m_maxTime > rhs.m_periodicTime.m_time.m_maxTime; };
		break;
	case ascOfMinPeriodicTime://最短処理時間昇順
		less_pred = [](const profileInfo& lhs, const profileInfo& rhs) -> bool
			{ return lhs.m_periodicTime.m_time.m_minTime < rhs.m_periodicTime.m_time.m_minTime; };
		break;
	case descOfMinPeriodicTime://最短処理時間降順
		less_pred = [](const profileInfo& lhs, const profileInfo& rhs) -> bool
			{ return lhs.m_periodicTime.m_time.m_minTime > rhs.m_periodicTime.m_time.m_minTime; };
		break;
	case ascOfPeriodicCount://計測回数昇順
		less_pred = [](const profileInfo& lhs, const profileInfo& rhs) -> bool
			{ return lhs.m_periodicTime.m_time.m_count < rhs.m_periodicTime.m_time.m_count; };
		break;
	case descOfPeriodicCount://計測回数降順
		less_pred = [](const profileInfo& lhs, const profileInfo& rhs) -> bool
			{ return lhs.m_periodicTime.m_time.m_count > rhs.m_periodicTime.m_time.m_count; };
		break;
	case ascOfMaxPeriodicCount://最大計測回数昇順
		less_pred = [](const profileInfo& lhs, const profileInfo& rhs) -> bool
			{ return lhs.m_periodicTime.m_maxCount < rhs.m_periodicTime.m_maxCount; };
		break;
	case descOfMaxPeriodicCount://最大計測回数降順
		less_pred = [](const profileInfo& lhs, const profileInfo& rhs) -> bool
			{ return lhs.m_periodicTime.m_maxCount > rhs.m_periodicTime.m_maxCount; };
		break;
	case ascOfMinPeriodicCount://最小計測回数昇順
		less_pred = [](const profileInfo& lhs, const profileInfo& rhs) -> bool
			{ return lhs.m_periodicTime.m_minCount < rhs.m_periodicTime.m_minCount; };
		break;
	case descOfMinPeriodicCount://最小計測回数降順
		less_pred = [](const profileInfo& lhs, const profileInfo& rhs) -> bool
			{ return lhs.m_periodicTime.m_minCount > rhs.m_periodicTime.m_minCount; };
		break;
	case ascOfAvgPeriodicCount://平均計測回数昇順
		less_pred = [](const profileInfo& lhs, const profileInfo& rhs) -> bool
			{ return lhs.m_periodicTime.m_avgCount < rhs.m_periodicTime.m_avgCount; };
		break;
	case descOfAvgPeriodicCount://平均計測回数降順
		less_pred = [](const profileInfo& lhs, const profileInfo& rhs) -> bool
			{ return lhs.m_periodicTime.m_avgCount > rhs.m_periodicTime.m_avgCount; };
		break;
	}
	if (less_pred)
		GASHA_ introSort(array, copy_size, less_pred);
	return copy_size;
}

//----------------------------------------
//プロファイラー

//文字列プール登録
const profiler::strPoolInfo* profiler::regStrPool(const char* name, GASHA_ crc32_t& name_crc)
{
	if (name_crc == 0)
		name_crc = GASHA_ calcCRC32(name);
	//文字列プール情報を取得
	strPoolInfo* str_info = nullptr;
	{
		auto lock = m_strPoolTable.lockScoped();//共有ロック ※登録中の情報にアクセスする可能性がある
		str_info = m_strPoolTable.at(name_crc);
		if (str_info)
			return str_info;
	}
	//文字列プールからメモリ確保
	const std::size_t name_len = GASHA_ strlen_fast(name);
	char* name_buff = m_strPoolBuff.newArray<char>(name_len + 1);
	GASHA_SIMPLE_ASSERT(name_buff != nullptr, "m_strPoolBuff is not enough memory.");
	if (!name_buff)
		return nullptr;
	//文字列プール情報を登録
	GASHA_ strcpy_fast(name_buff, name);
	strPoolInfo* pooled_str_info = m_strPoolTable.emplace(name_crc, name_crc, name_buff);//内部で排他ロック
	if (!pooled_str_info)//スレッド間で登録が競合した可能性があるので、文字列プール情報を再取得
	{
		//auto lock = m_strPoolTable.lockScoped();//共有ロック ※排他ロックで登録しているので、この時点で他方の処理が完了していないことはない（ロック不要）
		pooled_str_info = m_strPoolTable.at(name_crc);
		if (pooled_str_info)
			return pooled_str_info;
	}
	GASHA_SIMPLE_ASSERT(pooled_str_info != nullptr, "m_strPoolTable is not enough memory.");
	return pooled_str_info;
}

//スレッド情報登録
profiler::threadInfo* profiler::regThread(const GASHA_ threadId& thread_id)
{
	//スレッド名を文字列プールに登録
	GASHA_ crc32_t thread_name_crc = thread_id.nameCrc();
	const strPoolInfo* pooled_thread_name = regStrPool(thread_id.name(), thread_name_crc);
	//スレッド情報を取得
	threadInfo* thread_info = nullptr;
	{
		auto lock = m_threadInfoTable.lockScoped();//共有ロック ※登録中の情報にアクセスする可能性がある
		m_threadInfoTable.at(thread_name_crc);
		if (thread_info)
			return thread_info;
	}
	//スレッド情報を登録
	thread_info = m_threadInfoTable.emplace(thread_name_crc, thread_name_crc, pooled_thread_name->m_str);//内部で排他ロック
	if (!thread_info)//スレッド間で登録が競合した可能性があるので、スレッド情報を再取得
	{
		//auto lock = m_threadInfoTable.lockScoped();//共有ロック ※排他ロックで登録しているので、この時点で他方の処理が完了していないことはない（ロック不要）
		thread_info = m_threadInfoTable.at(thread_name_crc);
		if (thread_info)
			return thread_info;
	}
	GASHA_SIMPLE_ASSERT(thread_info != nullptr, "m_threadInfoTable is not enough memory.");
	if (!thread_info)
		return nullptr;
	//スレッド情報を連結リストに連結
	{
		auto lock = m_threadInfoList.lockScoped();//排他ロック
		m_threadInfoList.push_back(*thread_info);
	}
	return thread_info;
}

//処理時間加算
bool profiler::add(const char* name, const GASHA_ sec_t elapsed_time)
{
	//処理名を文字列プールに登録
	GASHA_ crc32_t name_crc = 0;
	const strPoolInfo* pooled_name = regStrPool(name, name_crc);
	if (!pooled_name)
		return false;
	//スレッド情報を登録
	GASHA_ threadId thread_id;
	threadInfo* thread_info = regThread(thread_id);
	if (!thread_info)
		return false;
	//処理時間を加算
	return thread_info->add(*pooled_name, elapsed_time, m_profileInfoPool);
}

//処理時間集計
bool profiler::sumup(const profileSumup_type type)
{
	//スレッド情報を取得（登録）
	GASHA_ threadId thread_id;
	threadInfo* thread_info = regThread(thread_id);
	if(!thread_info)
		return false;
	//処理時間集計
	return thread_info->sumup(type);
}

//スレッド情報を取得
std::size_t profiler::getThreadInfo(profiler::threadInfo* array, std::size_t max_size)
{
	auto lock = m_threadInfoList.lockSharedScoped();//共有ロック
	std::size_t copy_size = 0;
	GASHA_ forEach(m_threadInfoList, [&copy_size, &array, max_size](threadInfo& thread_info)
		{
			if (copy_size < max_size)
			{
				*(array + copy_size) = thread_info;
				++copy_size;
			}
		}
	);
	return copy_size;
}

//プロファイル情報を取得
std::size_t profiler::getProfileInfo(const GASHA_ crc32_t thread_name_crc, profiler::profileInfo* array, const std::size_t max_size, const profiler::profileOrder_type order)
{
	threadInfo* thread_info = nullptr;
	{
		auto lock = m_threadInfoList.lockSharedScoped();//共有ロック ※登録中の情報にアクセスする可能性がある
		thread_info = m_threadInfoTable.at(thread_name_crc);
		if (!thread_info)
			return 0;
	}
	return thread_info->getProfileInfo(array, max_size, order);
}

//グローバルログレベルマスク初期化（一回限り）
void profiler::initializeOnce()
{
	m_strPoolBuff.clear();//文字列プールバッファ
	m_strPoolTable.clear();//文字列プールテーブル
	m_profileInfoPool.clear();//プロファイル情報プール
	m_threadInfoTable.clear();//スレッド情報テーブル
	m_threadInfoList.clear();//スレッド情報連結リスト
}

//静的フィールドのインスタンス化
std::once_flag profiler::m_initialized;
profiler::strPoolBuff_type profiler::m_strPoolBuff;//文字列プールバッファ
profiler::strPoolTable_type profiler::m_strPoolTable;//文字列プールテーブル
profiler::profileInfoPool_type profiler::m_profileInfoPool;//プロファイル情報プール
profiler::threadInfoTable_type profiler::m_threadInfoTable;//スレッド情報テーブル
profiler::threadInfoLink_type profiler::m_threadInfoList;//スレッド情報連結リスト

#endif//GASHA_PROFILE_IS_AVAILABLE//プロファイル機能無効時はまるごと無効化

//静的変数をインスタンス化
const profiler::explicitInit_type profiler::explicitInit;

GASHA_NAMESPACE_END;//ネームスペース：終了

#ifdef GASHA_PROFILE_IS_AVAILABLE//プロファイル機能無効時はまるごと無効化

//各テンプレートクラスの明示的なインスタンス化
#include <gasha/lf_stack_allocator.cpp.h>//ロックフリースタックアロケータ【関数／実体定義部】
#include <gasha/lf_pool_allocator.cpp.h>//ロックフリープールアロケータ【関数／実体定義部】
#include <gasha/hash_table.cpp.h>//開番地法ハッシュテーブル【関数／実体定義部】
#include <gasha/rb_tree.cpp.h>//赤黒木【関数／実体定義部】
#include <gasha/singly_linked_list.cpp.h>//片方向連結リスト【関数／実体定義部】

//GASHA_INSTANCING_lfStackAllocator();//文字列プールバッファ
//GASHA_INSTANCING_lfPoolAllocator(GASHA_ profiler::STR_POOL_TABLE_SIZE);//文字列プールテーブル
GASHA_INSTANCING_hTable(GASHA_ profiler::strPoolOpe, GASHA_ profiler::STR_POOL_TABLE_SIZE);//文字列プールテーブル型
GASHA_INSTANCING_rbTree(GASHA_ profiler::profileInfoOpe);//プロファイル情報木型
//GASHA_INSTANCING_lfPoolAllocator(GASHA_ profiler::PROFILE_INFO_POOL_SIZE);//プロファイル情報プール型
GASHA_INSTANCING_hTable(GASHA_ profiler::threadInfoOpe, GASHA_ profiler::THREAD_INFO_TABLE_SIZE);//スレッド情報テーブル型
GASHA_INSTANCING_slList(GASHA_ profiler::threadInfoListOpe);//スレッド情報連結リスト型

#endif//GASHA_PROFILE_IS_AVAILABLE//プロファイル機能無効時はまるごと無効化

// End of file
