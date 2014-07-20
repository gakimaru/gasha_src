//--------------------------------------------------------------------------------
// std_console.cpp
// 標準コンソール【関数／実体定義部】
//
// Gakimaru's researched and standard library for C++ - GASHA
//   Copyright (c) 2014 Itagaki Mamoru
//   Released under the MIT license.
//     https://github.com/gakimaru/gasha/blob/master/LICENSE
//--------------------------------------------------------------------------------

#include <gasha/std_console.inl>//標準コンソール【インライン関数／テンプレート関数定義部】

#include <cstdio>//std::fptuc()

GASHA_NAMESPACE_BEGIN;//ネームスペース：開始

//--------------------------------------------------------------------------------
//標準コンソール
//--------------------------------------------------------------------------------

#ifdef GASHA_LOG_IS_ENABLED//デバッグログ無効時はまるごと無効化

//----------------------------------------
//画面通知用標準コンソール

#ifdef GASHA_USE_WINDOWS_CONSOLE

//出力開始
void stdNoticeConsole::begin()
{
	m_idAlreadyCr = false;
	winConsole::begin();
}

//出力終了
void stdNoticeConsole::end()
{
	winConsole::resetColor();
	winConsole::putCr();
	winConsole::end();
	m_idAlreadyCr = false;
}

//出力
void stdNoticeConsole::put(const char* str)
{
	if (m_idAlreadyCr)
		return;
	const char* p = str;
	while(true)
	{
		const char c = *p;
		if (c == '\0')
			break;
		else if(c == '\r' || c == '\n')
		{
			m_idAlreadyCr = true;
			break;
		}
		std::fputc(c, m_handle);
		++p;
	}
}

//改行出力
void stdNoticeConsole::putCr()
{
	winConsole::resetColor();
	m_idAlreadyCr = true;
}

//カラー変更
void stdNoticeConsole::changeColor(GASHA_ consoleColor&& color)
{
	if (m_idAlreadyCr)
		return;
	winConsole::changeColor(std::move(color));
}

#else//GASHA_USE_WINDOWS_CONSOLE

//出力開始
void stdNoticeConsole::begin()
{
	m_idAlreadyCr = false;
	ttyConsole::begin();
}

//出力終了
void stdNoticeConsole::end()
{
	ttyConsole::resetColor();
	ttyConsole::putCr();
	ttyConsole::end();
	m_idAlreadyCr = false;
}

//出力
void stdNoticeConsole::put(const char* str)
{
	if(m_idAlreadyCr)
		return;
	const char* p = str;
	while(true)
	{
		const char c = *p;
		if (c == '\0')
			break;
		else if(c == '\r' || c == '\n')
		{
			m_idAlreadyCr = true;
			break;
		}
		std::fputc(c, m_handle);
		++p;
	}
}

//改行出力
void stdNoticeConsole::putCr()
{
	ttyConsole::resetColor();
	m_idAlreadyCr = true;
}

//カラー変更
void stdNoticeConsole::changeColor(GASHA_ consoleColor&& color)
{
	if (m_idAlreadyCr)
		return;
	ttyConsole::changeColor(std::move(color));
}

#endif//GASHA_USE_WINDOWS_CONSOLE

#endif//GASHA_LOG_IS_ENABLED//デバッグログ無効時はまるごと無効化

//----------------------------------------
//標準出力コンソール
stdOutConsole stdOutConsole::m_instance;

//----------------------------------------
//標準エラーコンソール
stdErrConsole stdErrConsole::m_instance;

//----------------------------------------
//画面通知用標準コンソール
stdNoticeConsole stdNoticeConsole::m_instance;

//----------------------------------------
//標準メモリコンソール
stdMemConsole stdMemConsole::m_instance;

//----------------------------------------
//標準ダミーコンソール
stdDummyConsole stdDummyConsole::m_instance;

GASHA_NAMESPACE_END;//ネームスペース：終了

#ifdef GASHA_LOG_IS_ENABLED//デバッグログ無効時はまるごと無効化

//----------------------------------------
//標準メモリコンソールの明示的なインスタンス化
#include <gasha/mem_console.cpp.h>//メモリコンソール【関数／実体定義部】

//明示的なインスタンス化
GASHA_INSTANCING_memConsole(GASHA_STD_MEM_CONSOLE_BUFF_SIZE);

#endif//GASHA_LOG_IS_ENABLED//デバッグログ無効時はまるごと無効化

// End of file
