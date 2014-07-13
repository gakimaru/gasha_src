//--------------------------------------------------------------------------------
// log_level.cpp
// ログレベル【関数／実体定義部】
//
// Gakimaru's researched and standard library for C++ - GASHA
//   Copyright (c) 2014 Itagaki Mamoru
//   Released under the MIT license.
//     https://github.com/gakimaru/gasha/blob/master/LICENSE
//--------------------------------------------------------------------------------

#include <gasha/log_level.inl>//ログレベル【インライン関数／テンプレート関数定義部】

GASHA_NAMESPACE_BEGIN;//ネームスペース：開始

//--------------------------------------------------------------------------------
//ログレベル
//--------------------------------------------------------------------------------

#ifdef GASHA_HAS_DEBUG_LOG//デバッグログ無効時はまるごと無効化

//--------------------
//ログレベル

//前のレベルを取得
logLevel logLevel::prev() const
{
	level_type prev_value = END;
	if (m_info)
	{
		const level_type value = m_info->m_value;
		if (value >= NORMAL_MIN && value <= NORMAL_MAX)
		{
			prev_value = fromOutputLevel(toOutputLevel(value) - 1);
			if (prev_value < NORMAL_MIN || prev_value > NORMAL_MAX)
				prev_value = END;
		}
	}
	logLevelContainer con;
	return con.at(prev_value);
}

//次のレベルを取得
logLevel logLevel::next() const
{
	level_type next_value = END;
	if (m_info)
	{
		const level_type value = m_info->m_value;
		if (value >= NORMAL_MIN && value <= NORMAL_MAX)
		{
			next_value = fromOutputLevel(toOutputLevel(value) + 1);
			if (next_value < NORMAL_MIN || next_value > NORMAL_MAX)
				next_value = END;
		}
	}
	logLevelContainer con;
	return con.at(next_value);
}

//--------------------
//イテレータ

//インクリメント
void logLevelContainer::iterator::inc() const
{
	if (m_value == logLevel::INVALID)
		return;
	else if (m_value == logLevel::END)
	{
		m_value = logLevel::INVALID;
		m_logLevel = nullptr;
		m_isEnd = false;
		return;
	}
	while (m_value != logLevel::END)
	{
		++m_value;
		m_logLevel = logLevelContainer::getInfo(m_value);
		if (m_logLevel)
			return;
	}
	m_logLevel = nullptr;
	m_isEnd = true;
}

//デクリメント
void logLevelContainer::iterator::dec() const
{
	m_isEnd = false;
	if (m_value == logLevel::INVALID)
		return;
	else if (m_value == logLevel::BEGIN)
	{
		m_value = logLevel::INVALID;
		m_logLevel = nullptr;
		return;
	}
	while (m_value != logLevel::BEGIN)
	{
		--m_value;
		m_logLevel = logLevelContainer::getInfo(m_value);
		if (m_logLevel)
			return;
	}
	m_logLevel = logLevelContainer::getInfo(m_value);
	if (!m_logLevel)
		m_value = logLevel::INVALID;
	return;
}

//--------------------
//リバースイテレータ

//インクリメント
void logLevelContainer::reverse_iterator::inc() const
{
	if (m_value == logLevel::INVALID)
		return;
	else if (m_value == logLevel::BEGIN)
	{
		m_value = logLevel::INVALID;
		m_logLevel = nullptr;
		m_isEnd = false;
		return;
	}
	while (m_value != logLevel::BEGIN)
	{
		--m_value;
		m_logLevel = logLevelContainer::getInfo(m_value - 1);
		if (m_logLevel)
			return;
	}
	m_logLevel = nullptr;
	m_isEnd = true;
}

//デクリメント
void logLevelContainer::reverse_iterator::dec() const
{
	m_isEnd = false;
	if (m_value == logLevel::INVALID)
		return;
	else if (m_value == logLevel::END)
	{
		m_value = logLevel::INVALID;
		m_logLevel = nullptr;
		return;
	}
	while (m_value != logLevel::END)
	{
		++m_value;
		m_logLevel = logLevelContainer::getInfo(m_value - 1);
		if (m_logLevel)
			return;
	}
	m_logLevel = logLevelContainer::getInfo(m_value - 1);
	if (!m_logLevel)
		m_value = logLevel::INVALID;
	return;
}

//--------------------
//コンテナ（イテレータ用）

//要素を更新
bool logLevelContainer::regist(const logLevel::info& info)
{
	if (info.m_value < logLevel::MIN || info.m_value > logLevel::MAX || m_isAlreadyPool[info.m_value])
		return false;
	m_pool[info.m_value] = info;
	m_isAlreadyPool[info.m_value] = true;
	return true;
}

//全てのログレベルのコンソールを変更
void logLevelContainer::setAllConsole(IConsole* console)
{
	for (logLevel::level_type value = logLevel::NORMAL_MIN; value <= logLevel::NORMAL_MAX; ++value)
	{
		if (m_isAlreadyPool[value])
		{
			logLevel::info& info = m_pool[value];
			info.m_console = console;
		}
	}
}

//全てのログレベルの画面通知用コンソールを変更
void logLevelContainer::setAllConsoleForNotice(IConsole* console)
{
	for (logLevel::level_type value = logLevel::NORMAL_MIN; value <= logLevel::NORMAL_MAX; ++value)
	{
		if (m_isAlreadyPool[value])
		{
			logLevel::info& info = m_pool[value];
			info.m_consoleForNotice = console;
		}
	}
}

//コンテナ初期化（一回限り）
void logLevelContainer::initializeOnce()
{
	//静的変数を初期化
	m_isAlreadyPool.reset();
	//要素を初期化
	for (logLevel::level_type value = 0; value < logLevel::NUM; ++value)
	{
		logLevel::info& info = m_pool[value];
		info.m_value = value;
		info.m_name = "(undefined)";
		info.m_console = nullptr;
		info.m_consoleForNotice = nullptr;
		info.m_color.reset();
		info.m_colorForNotice.reset();
	}
	
	IConsole& console = GASHA_ stdConsole::instance();//標準コンソール
	IConsole& console_n = GASHA_ stdConsoleForNotice::instance();//画面通知用標準コンソール
	typedef consoleColor c;//コンソールカラー

	//既定のログレベルを設定（コンストラクタで要素を登録）
	#define REG_LOG_LEVEL(VALUE, CONSOLE, CONSOLE_N, FORE, BACK, FORE_N, BACK_N) \
		regLogLevel<VALUE>()( \
			#VALUE, \
			CONSOLE, \
			CONSOLE_N, \
			consoleColor(consoleColor::FORE, consoleColor::BACK), \
			consoleColor(consoleColor::FORE_N, consoleColor::BACK_N) \
		);
	#define REG_SPECIAL_LOG_LEVEL(VALUE) \
		_private::regSpecialLogLevel<VALUE>()( \
			#VALUE \
		);
	REG_LOG_LEVEL(asNormal, &console, nullptr, STANDARD, STANDARD, BLACK, iWHITE);//通常メッセージ
	REG_LOG_LEVEL(asVerbose, &console, nullptr, iBLACK, STANDARD, iBLACK, iWHITE);//冗長メッセージ
	REG_LOG_LEVEL(asDetail, &console, nullptr, iBLACK, STANDARD, iBLACK, iWHITE);//詳細メッセージ
	REG_LOG_LEVEL(asImportant, &console, &console_n, iBLUE, BLACK, iBLUE, iWHITE);//重要メッセージ
	REG_LOG_LEVEL(asWarning, &console, &console_n, iMAGENTA, STANDARD, BLACK, iMAGENTA);//警告メッセージ
	REG_LOG_LEVEL(asCritical, &console, &console_n, iRED, STANDARD, iYELLOW, iRED);//重大メッセージ
	REG_LOG_LEVEL(asAbsolute, &console, nullptr, STANDARD, STANDARD, STANDARD, STANDARD);//絶対メッセージ（ログレベルに関係なく出力したいメッセージ）
	REG_SPECIAL_LOG_LEVEL(asSilent);//静寂（絶対メッセ―ジ以外出力しない）
	REG_SPECIAL_LOG_LEVEL(asSilentAbsolutely);//絶対静寂（全てのメッセージを出力しない）
}

//コンテナの静的変数をインスタンス化
std::once_flag logLevelContainer::m_initialized;
logLevel::info logLevelContainer::m_pool[logLevel::POOL_NUM];
std::bitset<logLevel::POOL_NUM> logLevelContainer::m_isAlreadyPool;

#endif//GASHA_HAS_DEBUG_LOG//デバッグログ無効時はまるごと無効化

GASHA_NAMESPACE_END;//ネームスペース：終了

// End of file
