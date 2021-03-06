﻿//--------------------------------------------------------------------------------
// log_level.cpp
// ログレベル【関数／実体定義部】
//
// Gakimaru's standard library for C++ - GASHA
//   Copyright (c) 2014 Itagaki Mamoru
//   Released under the MIT license.
//     https://github.com/gakimaru/gasha/blob/master/LICENSE
//--------------------------------------------------------------------------------

#include <gasha/log_level.inl>//ログレベル【インライン関数／テンプレート関数定義部】

GASHA_NAMESPACE_BEGIN;//ネームスペース：開始

//--------------------------------------------------------------------------------
//ログレベル
//--------------------------------------------------------------------------------

#ifdef GASHA_LOG_IS_ENABLED//デバッグログ無効時はまるごと無効化

//--------------------
//ログレベル

//前のレベルを取得
logLevel logLevel::prev() const
{
	logLevelContainer con;
	if (m_info)
	{
		const level_type value = m_info->m_value;
		if (value >= NORMAL_MIN && value <= NORMAL_MAX)
		{
			level_type next_value = fromOutputLevel(toOutputLevel(value) - 1);
			while (next_value >= NORMAL_MIN && next_value <= NORMAL_MAX)
			{
				logLevel::info* info = con.getInfo(next_value);
				if (info)
					return info;
				--next_value;
			}
		}
	}
	return con.at(END);
}

//次のレベルを取得
logLevel logLevel::next() const
{
	logLevelContainer con;
	if (m_info)
	{
		const level_type value = m_info->m_value;
		if (value >= NORMAL_MIN && value <= NORMAL_MAX)
		{
			level_type next_value = fromOutputLevel(toOutputLevel(value) + 1);
			while (next_value >= NORMAL_MIN && next_value <= NORMAL_MAX)
			{
				logLevel::info* info = con.getInfo(next_value);
				if (info)
					return info;
				++next_value;
			}
		}
	}
	return con.at(END);
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
}

//コンストラクタ
logLevelContainer::iterator::iterator(const logLevel::level_type value) :
m_value(value),
m_logLevel(logLevelContainer::getInfo(m_value)),
m_isEnd(value == logLevel::END)
{
	if (!m_logLevel && !m_isEnd)
	{
		++m_value;
		while (m_value < logLevel::END)
		{
			m_logLevel = logLevelContainer::getInfo(m_value);
			if (m_logLevel)
				return;
			++m_value;
		}
		m_value = logLevel::END;
		m_isEnd = true;
	}
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
}

//コンストラクタ
logLevelContainer::reverse_iterator::reverse_iterator(const logLevel::level_type value) :
m_value(value),
m_logLevel(logLevelContainer::getInfo(m_value - 1)),
m_isEnd(value == logLevel::BEGIN)
{
	if (!m_logLevel && !m_isEnd)
	{
		--m_value;
		while (m_value > logLevel::BEGIN)
		{
			m_logLevel = logLevelContainer::getInfo(m_value - 1);
			if (m_logLevel)
				return;
			--m_value;
		}
		m_value = logLevel::BEGIN;
		m_isEnd = true;
	}
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
void logLevelContainer::replaceEachConsole(const logLevel::purpose_type purpose, iConsole* new_console)
{
	for (logLevel::level_type value = logLevel::NORMAL_MIN; value <= logLevel::NORMAL_MAX; ++value)
	{
		if (m_isAlreadyPool[value])
		{
			logLevel::info& info = m_pool[value];
			info.m_consoles[purpose] = new_console;
		}
	}
}
//※置き換え元のコンソールを指定する場合
void logLevelContainer::replaceEachConsole(const logLevel::purpose_type purpose, iConsole* src_console, iConsole* new_console)
{
	for (logLevel::level_type value = logLevel::NORMAL_MIN; value <= logLevel::NORMAL_MAX; ++value)
	{
		if (m_isAlreadyPool[value])
		{
			logLevel::info& info = m_pool[value];
			iConsole*& curr_console = info.m_consoles[purpose];
			if ((!curr_console && !src_console) ||
				(curr_console && src_console && *curr_console == *src_console))
				curr_console = new_console;
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
		for (logLevel::purpose_type purpose = 0; purpose < logLevel::PURPOSE_NUM; ++purpose)
		{
			info.m_consoles[purpose] = nullptr;
			info.m_colors[purpose].reset();
		}
	}
	
	//既定のログレベルを登録
	GASHA_ registDefaultLogLevel();
}

//静的変数をインスタンス化
std::once_flag logLevelContainer::m_initialized;
logLevel::info logLevelContainer::m_pool[logLevel::POOL_NUM];
std::bitset<logLevel::POOL_NUM> logLevelContainer::m_isAlreadyPool;

#else//GASHA_LOG_IS_ENABLED

//【VC++】LNK4221回避用のダミー関数
namespace _private{
	void log_level_dummy(){}
}//namespace _private

#endif//GASHA_LOG_IS_ENABLED//デバッグログ無効時はまるごと無効化

//静的変数をインスタンス化
const logLevelContainer::explicitInit_tag logLevelContainer::explicitInit;

GASHA_NAMESPACE_END;//ネームスペース：終了

// End of file
