//--------------------------------------------------------------------------------
// build_settings_diag.cpp
// ビルド設定診断【関数／実体定義部】
//
// Gakimaru's standard library for C++ - GASHA
//   Copyright (c) 2014 Itagaki Mamoru
//   Released under the MIT license.
//     https://github.com/gakimaru/gasha/blob/master/LICENSE
//--------------------------------------------------------------------------------

#include <gasha/build_settings/build_settings_diag.inl>//ビルド設定【インライン関数／テンプレート関数定義部】

GASHA_NAMESPACE_BEGIN;//ネームスペース：開始

//--------------------------------------------------------------------------------
//ビルド設定

//ライブラリビルド時の状態を診断
const diagForLibrary_t diagForLibrary;

//現在のプロジェクトの状態を診断
const diagForProject_t diagForProject;

//----------------------------------------
//ビルド設定が実行環境に適合するか診断
//※特殊化
template<>
bool buildSettingsDiagnosticTest<diagForLibrary_t>(char* message, const std::size_t max_size, std::size_t& size, const diagForLibrary_t mode)
{
	return buildSettingsDiagnosticTest<diagForLibrary_internal_t>(message, max_size, size, diagForLibrary_internal_t());
}
//※ライブラリビルド時の診断用関数を明示的にインスタンス化
template
bool buildSettingsDiagnosticTest<diagForLibrary_internal_t>(char* message, const std::size_t max_size, std::size_t& size, const diagForLibrary_internal_t mode);

GASHA_NAMESPACE_END;//ネームスペース：終了

// End of file
