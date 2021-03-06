﻿//--------------------------------------------------------------------------------
// lock_common.cpp
// ロック共通設定【関数／実体定義部】
//
// Gakimaru's standard library for C++ - GASHA
//   Copyright (c) 2014 Itagaki Mamoru
//   Released under the MIT license.
//     https://github.com/gakimaru/gasha/blob/master/LICENSE
//--------------------------------------------------------------------------------

#include <gasha/lock_common.inl>//ロック共通設定【インライン関数／テンプレート関数定義部】

GASHA_NAMESPACE_BEGIN;//ネームスペース：開始

//----------------------------------------
//ロック制御共通設定

;//ロック／排他ロック取得 ※コンストラクタでロック／排他ロックを取得する
const with_lock_t with_lock;

//共有ロック取得 ※コンストラクタで共有ロックを取得する
const with_lock_shared_t with_lock_shared;

//ロック／排他ロック試行 ※コンストラクタでロック／排他ロック取得を試行する
const try_to_lock_t try_to_lock;

//共有ロック試行 ※コンストラクタで共有ロック取得を試行する
const try_to_lock_shared_t try_to_lock_shared;

//ロック／排他取得済み状態管理の委譲 ※コンストラクタでロック／排他ロック取得済み状態にする（ロックを取得するわけではない）
const adopt_lock_t adopt_lock;

//共有ロック取得済み状態管理の委譲 ※コンストラクタで共有ロック取得済み状態にする（ロックを取得するわけではない）
const adopt_shared_lock_t adopt_shared_lock;

//遅延ロック ※コンストラクタでロックを取得しない
const defer_lock_t defer_lock;

//----------------------------------------
//コンテキストスイッチ

//ゼロスリープでスイッチ
const zeroSleepContextSwitch_tag zeroSleepContextSwitch;

//短いスリープでスイッチ
const shortSleepContextSwitch_tag shortSleepContextSwitch;

//確実なスイッチ
const forceContextSwitch_tag forceContextSwitch;

//イールド
const yieldContextSwitch_tag yieldContextSwitch;

GASHA_NAMESPACE_END;//ネームスペース：終了

// End of file
