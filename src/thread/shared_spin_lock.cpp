//--------------------------------------------------------------------------------
// shared_spin_lock.cpp
// 共有スピンロック
//
// Gakimaru's researched and standard library for C++ - GASHA
//   Copyright (c) 2014 Itagaki Mamoru
//   Released under the MIT license.
//     https://github.com/gakimaru/gasha/blob/master/LICENSE
//--------------------------------------------------------------------------------

#include <gasha/shared_spin_lock.h>//共有スピンロック

//例外を無効化した状態で std::thread をインクルードすると、warning C4530 が発生する
//  warning C4530: C++ 例外処理を使っていますが、アンワインド セマンティクスは有効にはなりません。/EHsc を指定してください。
#pragma warning(disable: 4530)//C4530を抑える

#include <thread>//C++11 std::this_thread::sleep_for
#include <chrono>//C++11 std::chrono::milliseconds

NAMESPACE_GASHA_BEGIN//ネームスペース：開始

//----------------------------------------
//共有スピンロッククラス

//排他ロック（ライトロック）取得
void shared_spin_lock::lock(const int spin_count)
{
	int spin_count_now = spin_count;
	while (1)
	{
		const int lock_counter = m_lockCounter.fetch_sub(SHARED_LOCK_COUNTER_UNLOCKED);//カウンタを更新
		if (lock_counter == SHARED_LOCK_COUNTER_UNLOCKED)
			return;//ロック取得成功
		if (lock_counter > 0)	//他が排他ロックを取得していないので、現在の共有ロックが全て解放されるのを待つ
		{						//※カウンタを更新したままなので、後続の共有ロック／排他ロックは取得できない。
			while (m_lockCounter.load() != 0)//カウンタが0になるのを待つ
			{
				if (spin_count == 1 || spin_count > 1 && --spin_count_now == 0)
				{
					std::this_thread::sleep_for(std::chrono::milliseconds(0));//コンテキストスイッチ（ゼロスリープ）
					spin_count_now = spin_count;
				}
			}
			return;//ロック取得成功
		}
		m_lockCounter.fetch_add(SHARED_LOCK_COUNTER_UNLOCKED);//カウンタを戻してリトライ
		if (spin_count == 1 || spin_count > 1 && --spin_count_now == 0)
		{
			std::this_thread::sleep_for(std::chrono::milliseconds(0));//コンテキストスイッチ（ゼロスリープ）
			spin_count_now = spin_count;
		}
	}
}

//排他ロック（ライトロック）取得を試行
//※取得に成功した場合、trueが返るので、ロックを解放する必要がある
bool shared_spin_lock::try_lock()
{
	const int lock_counter = m_lockCounter.fetch_sub(SHARED_LOCK_COUNTER_UNLOCKED);//カウンタを更新
	if (lock_counter == SHARED_LOCK_COUNTER_UNLOCKED)
		return true;//ロック取得成功
	m_lockCounter.fetch_add(SHARED_LOCK_COUNTER_UNLOCKED);//カウンタを戻す
	return false;//ロック取得失敗
}

//共有ロック（リードロック）取得
void shared_spin_lock::lock_shared(const int spin_count)
{
	int spin_count_now = spin_count;
	while (1)
	{
		const int lock_counter = m_lockCounter.fetch_sub(1);//カウンタを更新
		if (lock_counter > 0)
			return;//ロック取得成功
		m_lockCounter.fetch_add(1);//カウンタを戻してリトライ
		if (spin_count == 1 || spin_count > 1 && --spin_count_now == 0)
		{
			std::this_thread::sleep_for(std::chrono::milliseconds(0));//コンテキストスイッチ（ゼロスリープ）
			spin_count_now = spin_count;
		}
	}
}

//共有ロック（リードロック）取得を試行
bool shared_spin_lock::try_lock_shared()
{
	const int lock_counter = m_lockCounter.fetch_sub(1);//カウンタを更新
	if (lock_counter >= 0)
		return true;//ロック取得成功
	m_lockCounter.fetch_add(1);//カウンタを戻す
	return false;//ロック取得失敗
}

NAMESPACE_GASHA_END//ネームスペース：終了

// End of file
