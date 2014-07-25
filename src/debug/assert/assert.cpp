//--------------------------------------------------------------------------------
// assert.cpp
// アサーション／ブレークポイント／ウォッチポイント【関数／実体定義部】
//
// Gakimaru's standard library for C++ - GASHA
//   Copyright (c) 2014 Itagaki Mamoru
//   Released under the MIT license.
//     https://github.com/gakimaru/gasha/blob/master/LICENSE
//--------------------------------------------------------------------------------

#include <gasha/assert.inl>//アサーション／ブレークポイント／ウォッチポイント【インライン関数／テンプレート関数定義部】

GASHA_NAMESPACE_BEGIN;//ネームスペース：開始

//--------------------------------------------------------------------------------
//アサーション／ブレークポイント／ウォッチポイント
//--------------------------------------------------------------------------------

#ifdef GASHA_ASSERTION_IS_ENABLED//アサーション無効時はまるごと無効化

//--------------------
//アサーション／ブレークポイント／ウォッチポイント

//TLS静的フィールド
thread_local GASHA_ iDebugPause* breakPoint::m_tlsDebugPause = nullptr;//TLSデバッグポーズ処理オブジェクト

#endif//GASHA_ASSERTION_IS_ENABLED//アサーション無効時はまるごと無効化

GASHA_NAMESPACE_END;//ネームスペース：終了

// End of file
