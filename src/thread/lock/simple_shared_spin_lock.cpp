//--------------------------------------------------------------------------------
// simple_shared_spin_lock.cpp
// 単純共有スピンロック【関数定義部】
//
// Gakimaru's researched and standard library for C++ - GASHA
//   Copyright (c) 2014 Itagaki Mamoru
//   Released under the MIT license.
//     https://github.com/gakimaru/gasha/blob/master/LICENSE
//--------------------------------------------------------------------------------

#include <gasha/simple_shared_spin_lock.h>//単純共有スピンロック【宣言部】
#include <gasha/simple_shared_spin_lock.inl>//単純共有スピンロック【インライン関数／テンプレート関数定義部】

#include <chrono>//C++11 std::chrono

//【VC++】例外を無効化した状態で <thread> をインクルードすると、warning C4530 が発生する
//  warning C4530: C++ 例外処理を使っていますが、アンワインド セマンティクスは有効にはなりません。/EHsc を指定してください。
#pragma warning(disable: 4530)//C4530を抑える

#include <thread>//C++11 std::this_thread

GASHA_NAMESPACE_BEGIN;//ネームスペース：開始

//----------------------------------------
//単純共有スピンロッククラス

//排他ロック（ライトロック）取得
void simpleSharedSpinLock::lock(const int spin_count)
{
	int spin_count_now = spin_count;
	while (1)
	{
		const int lock_counter = m_lockCounter.fetch_sub(SHARED_LOCK_COUNTER_UNLOCKED);//カウンタを更新
		if (lock_counter == SHARED_LOCK_COUNTER_UNLOCKED)
			return;//ロック取得成功
		m_lockCounter.fetch_add(SHARED_LOCK_COUNTER_UNLOCKED);//カウンタを戻してリトライ
		if (spin_count == 1 || (spin_count > 1 && --spin_count_now == 0))
		{
			std::this_thread::sleep_for(std::chrono::milliseconds(0));//コンテキストスイッチ（ゼロスリープ）
			spin_count_now = spin_count;
		}
	}
}

//排他ロック（ライトロック）取得を試行
//※取得に成功した場合、trueが返るので、ロックを解放する必要がある
bool simpleSharedSpinLock::try_lock()
{
	const int lock_counter = m_lockCounter.fetch_sub(SHARED_LOCK_COUNTER_UNLOCKED);//カウンタを更新
	if (lock_counter == SHARED_LOCK_COUNTER_UNLOCKED)
		return true;//ロック取得成功
	m_lockCounter.fetch_add(SHARED_LOCK_COUNTER_UNLOCKED);//カウンタを戻す
	return false;//ロック取得失敗
}

//共有ロック（リードロック）取得
void simpleSharedSpinLock::lock_shared(const int spin_count)
{
	int spin_count_now = spin_count;
	while (1)
	{
		const int lock_counter = m_lockCounter.fetch_sub(1);//カウンタを更新
		if (lock_counter > 0)
			return;//ロック取得成功
		m_lockCounter.fetch_add(1);//カウンタを戻してリトライ
		if (spin_count == 1 || (spin_count > 1 && --spin_count_now == 0))
		{
			std::this_thread::sleep_for(std::chrono::milliseconds(0));//コンテキストスイッチ（ゼロスリープ）
			spin_count_now = spin_count;
		}
	}
}

//共有ロック（リードロック）取得を試行
bool simpleSharedSpinLock::try_lock_shared()
{
	const int lock_counter = m_lockCounter.fetch_sub(1);//カウンタを更新
	if (lock_counter >= 0)
		return true;//ロック取得成功
	m_lockCounter.fetch_add(1);//カウンタを戻す
	return false;//ロック取得失敗
}

GASHA_NAMESPACE_END;//ネームスペース：終了

// End of file
