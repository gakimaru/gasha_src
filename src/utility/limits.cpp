//--------------------------------------------------------------------------------
//【テンプレートライブラリ】
// limits.inl
// 限界値【関数／実体定義部】
//
// Gakimaru's standard library for C++ - GASHA
//   Copyright (c) 2014 Itagaki Mamoru
//   Released under the MIT license.
//     https://github.com/gakimaru/gasha/blob/master/LICENSE
//--------------------------------------------------------------------------------

#include <gasha/limits.inl>//限界値【インライン関数／テンプレート関数定義部】

GASHA_NAMESPACE_BEGIN;//ネームスペース：開始

//型の限界値情報：float型
const float numeric_limits<float>::MIN = std::numeric_limits<float>::min();
const float numeric_limits<float>::MAX = std::numeric_limits<float>::max();
const float numeric_limits<float>::ZERO = 0.f;

//型の限界値情報：double型
const double numeric_limits<double>::MIN = std::numeric_limits<double>::min();
const double numeric_limits<double>::MAX = std::numeric_limits<double>::max();
const double numeric_limits<double>::ZERO = 0.f;

GASHA_NAMESPACE_END;//ネームスペース：終了

// End of file
