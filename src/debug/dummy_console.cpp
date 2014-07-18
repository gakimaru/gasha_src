//--------------------------------------------------------------------------------
// dummy_console.cpp
// ダミーコンソール【関数／実体定義部】
//
// Gakimaru's researched and standard library for C++ - GASHA
//   Copyright (c) 2014 Itagaki Mamoru
//   Released under the MIT license.
//     https://github.com/gakimaru/gasha/blob/master/LICENSE
//--------------------------------------------------------------------------------

#include <gasha/dummy_console.inl>//ダミーコンソール【インライン関数／テンプレート関数定義部】

GASHA_NAMESPACE_BEGIN;//ネームスペース：開始

//--------------------------------------------------------------------------------
//ダミーコンソール
//--------------------------------------------------------------------------------

#ifdef GASHA_LOG_IS_ENABLED//デバッグログ無効時はまるごと無効化

//----------------------------------------
//ダミーコンソールクラス

//出力開始
void dummyConsole::begin()
{
	//何もしない
}

//出力終了
void dummyConsole::end()
{
	//何もしない
}

//出力
void dummyConsole::put(const char* str)
{
	//何もしない
}

//改行出力
void dummyConsole::putCr()
{
	//何もしない
}

//カラー変更
void dummyConsole::changeColor(GASHA_ consoleColor&& color)
{
	//何もしない
}

//カラーリセット
void dummyConsole::resetColor()
{
	//何もしない
}

//出力先が同じか判定
bool dummyConsole::isSame(const IConsole* rhs) const
{
	const dummyConsole* _rhs = dynamic_cast<const dummyConsole*>(rhs);
	if (!_rhs)
		return false;
	return true;
}

//デストラクタ
dummyConsole::~dummyConsole()
{}

#endif//GASHA_LOG_IS_ENABLED//デバッグログ無効時はまるごと無効化

GASHA_NAMESPACE_END;//ネームスペース：終了

// End of file
