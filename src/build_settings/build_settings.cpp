//--------------------------------------------------------------------------------
// build_settings.cpp
// ビルド設定【関数／実体定義部】
//
// Gakimaru's researched and standard library for C++ - GASHA
//   Copyright (c) 2014 Itagaki Mamoru
//   Released under the MIT license.
//     https://github.com/gakimaru/gasha/blob/master/LICENSE
//--------------------------------------------------------------------------------

#include <gasha/build_settings/build_settings.inl>//ビルド設定【インライン関数／テンプレート関数定義部】

GASHA_NAMESPACE_BEGIN;//ネームスペース：開始

//--------------------------------------------------------------------------------
//ビルド設定

//ライブラリビルド時の状態を診断
const diagnosisModeForLibrary_t diagnosisModeForLibrary;

//現在の状態を診断
const diagnosisModeForCurrent_t diagnosisModeForCurrent;

//----------------------------------------
//ビルド設定が実行環境に適合するか診断
//※特殊化
template<>
bool diagnoseBuildSettings<diagnosisModeForLibrary_t>(char* message, std::size_t& size, const diagnosisModeForLibrary_t mode)
{
	return diagnoseBuildSettings<diagnosisModeForLibrary_internal_t>(message, size, diagnosisModeForLibrary_internal_t());
}
//※ライブラリビルド時の診断用関数を明示的にインスタンス化
template
bool diagnoseBuildSettings<diagnosisModeForLibrary_internal_t>(char* message, std::size_t& size, const diagnosisModeForLibrary_internal_t mode);

GASHA_NAMESPACE_END;//ネームスペース：終了

// End of file
