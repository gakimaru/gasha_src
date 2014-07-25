//--------------------------------------------------------------------------------
// unshared_spin_lock.cpp
// 非共有スピンロック【関数／実体定義部】
//
// Gakimaru's standard library for C++ - GASHA
//   Copyright (c) 2014 Itagaki Mamoru
//   Released under the MIT license.
//     https://github.com/gakimaru/gasha/blob/master/LICENSE
//--------------------------------------------------------------------------------

#include <gasha/unshared_spin_lock.inl>//非共有スピンロック【インライン関数／テンプレート関数定義部】

#include <chrono>//C++11 std::chrono

#pragma warning(push)//【VC++】ワーニング設定を退避
#pragma warning(disable: 4530)//【VC++】C4530を抑える
#include <thread>//C++11 std::this_thread
#pragma warning(pop)//【VC++】ワーニング設定を復元

GASHA_NAMESPACE_BEGIN;//ネームスペース：開始

//----------------------------------------
//非共有スピンロッククラス

//排他ロック（ライトロック）取得
void unsharedSpinLock::lock(const int spin_count)
{
	int spin_count_now = spin_count;
	while (true)
	{
		if (!m_lock.test_and_set())
			return;

		//ロックが取得できなければリトライ
		if (spin_count == 1 || (spin_count > 1 && --spin_count_now == 0))
		{
			defaultContextSwitch();
			spin_count_now = spin_count;
		}
	}
}

GASHA_NAMESPACE_END;//ネームスペース：終了

// End of file
