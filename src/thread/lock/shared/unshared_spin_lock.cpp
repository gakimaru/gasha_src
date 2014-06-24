//--------------------------------------------------------------------------------
// unshared_spin_lock.cpp
// 非共有スピンロック
//
// Gakimaru's researched and standard library for C++ - GASHA
//   Copyright (c) 2014 Itagaki Mamoru
//   Released under the MIT license.
//     https://github.com/gakimaru/gasha/blob/master/LICENSE
//--------------------------------------------------------------------------------

#include <gasha/unshared_spin_lock.h>//非共有スピンロック

//例外を無効化した状態で <thread> をインクルードすると、warning C4530 が発生する
//  warning C4530: C++ 例外処理を使っていますが、アンワインド セマンティクスは有効にはなりません。/EHsc を指定してください。
#pragma warning(disable: 4530)//C4530を抑える

#include <thread>//C++11 std::this_thread
#include <chrono>//C++11 std::chrono

NAMESPACE_GASHA_BEGIN;//ネームスペース：開始

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
		if (spin_count == 1 || (spin_count > 1 && --spin_count_now == 0))
		{
			std::this_thread::sleep_for(std::chrono::milliseconds(0));//コンテキストスイッチ（ゼロスリープ）
			spin_count_now = spin_count;
		}
	}
}

NAMESPACE_GASHA_END;//ネームスペース：終了

// End of file
