//--------------------------------------------------------------------------------
// lw_spin_lock.cpp
// サイズ軽量スピンロック
//
// Gakimaru's researched and standard library for C++ - GASHA
//   Copyright (c) 2014 Itagaki Mamoru
//   Released under the MIT license.
//     https://github.com/gakimaru/gasha/blob/master/LICENSE
//--------------------------------------------------------------------------------

#include <gasha/lw_spin_lock.h>//サイズ軽量スピンロック

//例外を無効化した状態で std::thread をインクルードすると、warning C4530 が発生する
//  warning C4530: C++ 例外処理を使っていますが、アンワインド セマンティクスは有効にはなりません。/EHsc を指定してください。
#pragma warning(disable: 4530)//C4530を抑える

#include <thread>//C++11 std::this_thread::sleep_for
#include <chrono>//C++11 std::chrono::milliseconds

NAMESPACE_GASHA_BEGIN//ネームスペース：開始

//----------------------------------------
//サイズ軽量スピンロッククラス

//ロック取得
void lw_spin_lock::lock(const int spin_count)
{
	int spin_count_now = spin_count;
	while (true)
	{
		bool prev = false;
		if (!m_lock.compare_exchange_weak(prev, true))
			return;
		if (spin_count == 1 || spin_count > 1 && --spin_count_now == 0)
		{
			std::this_thread::sleep_for(std::chrono::milliseconds(0));//コンテキストスイッチ（ゼロスリープ）
			spin_count_now = spin_count;
		}
	}
}

NAMESPACE_GASHA_END//ネームスペース：終了

// End of file
