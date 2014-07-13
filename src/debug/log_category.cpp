//--------------------------------------------------------------------------------
// log_category.cpp
// ログカテゴリ【関数／実体定義部】
//
// Gakimaru's researched and standard library for C++ - GASHA
//   Copyright (c) 2014 Itagaki Mamoru
//   Released under the MIT license.
//     https://github.com/gakimaru/gasha/blob/master/LICENSE
//--------------------------------------------------------------------------------

#include <gasha/log_category.inl>//ログカテゴリ【インライン関数／テンプレート関数定義部】

GASHA_NAMESPACE_BEGIN;//ネームスペース：開始

//--------------------------------------------------------------------------------
//ログカテゴリ
//--------------------------------------------------------------------------------

#ifdef GASHA_HAS_DEBUG_LOG//デバッグログ無効時はまるごと無効化

//--------------------
//イテレータ

//インクリメント
void logCategoryContainer::iterator::inc() const
{
	if (m_value == logCategory::INVALID)
		return;
	else if (m_value == logCategory::END)
	{
		m_value = logCategory::INVALID;
		m_logCategory = nullptr;
		m_isEnd = false;
		return;
	}
	while (m_value != logCategory::END)
	{
		++m_value;
		m_logCategory = logCategoryContainer::getInfo(m_value);
		if (m_logCategory)
			return;
	}
	m_logCategory = nullptr;
	m_isEnd = true;
}

//デクリメント
void logCategoryContainer::iterator::dec() const
{
	m_isEnd = false;
	if (m_value == logCategory::INVALID)
		return;
	else if (m_value == logCategory::BEGIN)
	{
		m_value = logCategory::INVALID;
		m_logCategory = nullptr;
		return;
	}
	while (m_value != logCategory::BEGIN)
	{
		--m_value;
		m_logCategory = logCategoryContainer::getInfo(m_value);
		if (m_logCategory)
			return;
	}
	m_logCategory = logCategoryContainer::getInfo(m_value);
	if (!m_logCategory)
		m_value = logCategory::INVALID;
	return;
}

//コンストラクタ
logCategoryContainer::iterator::iterator(const logCategory::category_type value) :
m_value(value),
m_logCategory(logCategoryContainer::getInfo(m_value)),
m_isEnd(value == logCategory::END)
{
	if (!m_logCategory && !m_isEnd)
	{
		++m_value;
		while (m_value < logCategory::END)
		{
			m_logCategory = logCategoryContainer::getInfo(m_value);
			if (m_logCategory)
				return;
			++m_value;
		}
		m_value = logCategory::END;
		m_isEnd = true;
	}
}

//--------------------
//リバースイテレータ

//インクリメント
void logCategoryContainer::reverse_iterator::inc() const
{
	if (m_value == logCategory::INVALID)
		return;
	else if (m_value == logCategory::BEGIN)
	{
		m_value = logCategory::INVALID;
		m_logCategory = nullptr;
		m_isEnd = false;
		return;
	}
	while (m_value != logCategory::BEGIN)
	{
		--m_value;
		m_logCategory = logCategoryContainer::getInfo(m_value - 1);
		if (m_logCategory)
			return;
	}
	m_logCategory = nullptr;
	m_isEnd = true;
}

//デクリメント
void logCategoryContainer::reverse_iterator::dec() const
{
	m_isEnd = false;
	if (m_value == logCategory::INVALID)
		return;
	else if (m_value == logCategory::END)
	{
		m_value = logCategory::INVALID;
		m_logCategory = nullptr;
		return;
	}
	while (m_value != logCategory::END)
	{
		++m_value;
		m_logCategory = logCategoryContainer::getInfo(m_value - 1);
		if (m_logCategory)
			return;
	}
	m_logCategory = logCategoryContainer::getInfo(m_value - 1);
	if (!m_logCategory)
		m_value = logCategory::INVALID;
	return;
}

//コンストラクタ
logCategoryContainer::reverse_iterator::reverse_iterator(const logCategory::category_type value) :
m_value(value),
m_logCategory(logCategoryContainer::getInfo(m_value - 1)),
m_isEnd(value == logCategory::BEGIN)
{
	if (!m_logCategory && !m_isEnd)
	{
		--m_value;
		while (m_value > logCategory::BEGIN)
		{
			m_logCategory = logCategoryContainer::getInfo(m_value - 1);
			if (m_logCategory)
				return;
			--m_value;
		}
		m_value = logCategory::BEGIN;
		m_isEnd = true;
	}
}

//--------------------
//コンテナ（イテレータ用）

//要素を更新
bool logCategoryContainer::regist(const logCategory::info& info)
{
	if (info.m_value < logCategory::MIN || info.m_value > logCategory::MAX || m_isAlreadyPool[info.m_value])
		return false;
	m_pool[info.m_value] = info;
	m_isAlreadyPool[info.m_value] = true;
	return true;
}

//全てのログカテゴリのコンソールを変更
void logCategoryContainer::setAllConsole(IConsole* console)
{
	for (logCategory::category_type value = logCategory::NORMAL_MIN; value <= logCategory::NORMAL_MAX; ++value)
	{
		if (m_isAlreadyPool[value])
		{
			logCategory::info& info = m_pool[value];
			info.m_console = console;
		}
	}
}

//全てのログカテゴリの画面通知用コンソールを変更
void logCategoryContainer::setAllConsoleForNotice(IConsole* console)
{
	for (logCategory::category_type value = logCategory::NORMAL_MIN; value <= logCategory::NORMAL_MAX; ++value)
	{
		if (m_isAlreadyPool[value])
		{
			logCategory::info& info = m_pool[value];
			info.m_consoleForNotice = console;
		}
	}
}

//コンテナ初期化（一回限り）
void logCategoryContainer::initializeOnce()
{
	//静的変数を初期化
	m_isAlreadyPool.reset();
	//要素を初期化
	for (logCategory::category_type value = 0; value < logCategory::NUM; ++value)
	{
		logCategory::info& info = m_pool[value];
		info.m_value = value;
		info.m_name = "(undefined)";
		info.m_console = nullptr;
		info.m_consoleForNotice = nullptr;
	}
	
	typedef consoleColor c;//コンソールカラー

	//既定のログカテゴリを登録（関数オブジェクトで登録）
	#define REG_LOG_CATEGORY(VALUE, CONSOLE, CONSOLE_N) \
		regLogCategory<VALUE>()( \
			#VALUE, \
			CONSOLE, \
			CONSOLE_N \
		)
	#define REG_SPECIAL_LOG_CATEGORY(VALUE) \
		_private::regSpecialLogCategory<VALUE>()( \
			#VALUE \
		)
	REG_LOG_CATEGORY(forAny, nullptr, nullptr);//なんでも（カテゴリなし）
	REG_LOG_CATEGORY(forFileSystem, nullptr, nullptr);//ファイルシステム関係
	REG_LOG_CATEGORY(forResource, nullptr, nullptr);//リソース関係
	REG_LOG_CATEGORY(for3D, nullptr, nullptr);//3Dグラフィックス関係
	REG_LOG_CATEGORY(for2D, nullptr, nullptr);//2Dグラフィックス関係
	REG_LOG_CATEGORY(forSound, nullptr, nullptr);//サウンド関係
	//ログレベル／画面通知レベル変更用
	REG_SPECIAL_LOG_CATEGORY(forEvery);//全部まとめて変更
	//特殊なカテゴリ（プリント時専用）
	REG_SPECIAL_LOG_CATEGORY(forCallPoint);//直近のコールポイントのカテゴリに合わせる（なければforAny扱い）
	REG_SPECIAL_LOG_CATEGORY(forCriticalCallPoint);//直近の重大コールポイントのカテゴリに合わせる（なければforAny扱い）
}

//コンテナの静的変数をインスタンス化
std::once_flag logCategoryContainer::m_initialized;
logCategory::info logCategoryContainer::m_pool[logCategory::POOL_NUM];
std::bitset<logCategory::POOL_NUM> logCategoryContainer::m_isAlreadyPool;

#endif//GASHA_HAS_DEBUG_LOG//デバッグログ無効時はまるごと無効化

GASHA_NAMESPACE_END;//ネームスペース：終了

// End of file
