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

//----------------------------------------
//既定のログレベル定義
DECLARE_LOG_LEVEL(asNormal, true, true, STANDARD, STANDARD, BLACK, iWHITE);//通常メッセージ
DECLARE_LOG_LEVEL(asVerbose, true, false, iBLACK, STANDARD, iBLACK, iWHITE);//冗長メッセージ
DECLARE_LOG_LEVEL(asDetail, true, false, iBLACK, STANDARD, iBLACK, iWHITE);//詳細メッセージ
DECLARE_LOG_LEVEL(asImportant, true, true, iBLUE, STANDARD, iBLUE, iWHITE);//重要メッセージ
DECLARE_LOG_LEVEL(asWarning, true, true, iMAGENTA, STANDARD, BLACK, iMAGENTA);//警告メッセージ
DECLARE_LOG_LEVEL(asCritical, true, true, iRED, STANDARD, iYELLOW, iRED);//重大メッセージ
DECLARE_LOG_LEVEL(asAbsolute, true, false, STANDARD, STANDARD, STANDARD, STANDARD);//絶対メッセージ（ログレベルに関係なく出力したいメッセージ）
DECLARE_SPECIAL_LOG_LEVEL(asSilent);//静寂（絶対メッセ―ジ以外出力しない）
DECLARE_SPECIAL_LOG_LEVEL(asSilentAbsolutely);//絶対静寂（全てのメッセージを出力しない）

//----------------------------------------
//ログレベルクラス

//--------------------
//イテレータ

//インクリメント
void logLevel::iterator::inc() const
{
	if (m_value == container::invalidValue())
		return;
	else if (m_value == container::endValue())
	{
		m_value = container::invalidValue();
		m_ref = nullptr;
		m_isEnd = false;
		return;
	}
	while (m_value != container::endValue())
	{
		++m_value;
		m_ref = container::_at(m_value);
		if (m_ref)
			return;
	}
	m_ref = nullptr;
	m_isEnd = true;
}

//デクリメント
void logLevel::iterator::dec() const
{
	m_isEnd = false;
	if (m_value == container::invalidValue())
		return;
	else if (m_value == container::beginValue())
	{
		m_value = container::invalidValue();
		m_ref = nullptr;
		return;
	}
	while (m_value != container::beginValue())
	{
		--m_value;
		m_ref = container::_at(m_value);
		if (m_ref)
			return;
	}
	m_ref = container::_at(m_value);
	if (!m_ref)
		m_value = container::invalidValue();
	return;
}

//--------------------
//リバースイテレータ

//インクリメント
void logLevel::reverse_iterator::inc() const
{
	if (m_value == container::invalidValue())
		return;
	else if (m_value == container::beginValue())
	{
		m_value = container::invalidValue();
		m_ref = nullptr;
		m_isEnd = false;
		return;
	}
	while (m_value != container::beginValue())
	{
		--m_value;
		m_ref = container::_at(m_value - 1);
		if (m_ref)
			return;
	}
	m_ref = nullptr;
	m_isEnd = true;
}

//デクリメント
void logLevel::reverse_iterator::dec() const
{
	m_isEnd = false;
	if (m_value == container::invalidValue())
		return;
	else if (m_value == container::endValue())
	{
		m_value = container::invalidValue();
		m_ref = nullptr;
		return;
	}
	while (m_value != container::endValue())
	{
		++m_value;
		m_ref = container::_at(m_value - 1);
		if (m_ref)
			return;
	}
	m_ref = container::_at(m_value - 1);
	if (!m_ref)
		m_value = container::invalidValue();
	return;
}

//--------------------
//コンテナ（イテレータ用）

//要素を更新
bool logLevel::container::update(const logLevel::level_type value, const logLevel& obj)
{
	if (value >= MIN && value <= MAX && !container::m_isAlreadyPool[value])
	{
		container::m_poolPtr[value] = obj;
		container::m_isAlreadyPool[value] = true;
		return true;
	}
	return false;
}

//全てのログレベルのコンソールを変更
void logLevel::container::setAllConsole(IConsole* console)
{
	for (level_type value = 0; value < NUM; ++value)
	{
		logLevel& level = m_poolPtr[value];
		level.console() = console;
	}
}

//全てのログレベルの画面通知用コンソールを変更
void logLevel::container::setAllConsoleForNotice(IConsole* console)
{
	for (level_type value = 0; value < NUM; ++value)
	{
		logLevel& level = m_poolPtr[value];
		level.consoleForNotice() = console;
	}
}

//レベルコンテナ初期化（一回限り）
void logLevel::container::initializeOnce()
{
	//初期化済みチェック
	if (m_isInitialized)
		return;
	//静的変数を初期化
	m_isInitialized = true;
	m_isAlreadyPool.reset();
	m_poolPtr = reinterpret_cast<logLevel*>(m_pool);
	//要素を初期化
	for (logLevel::level_type value = 0; value < logLevel::NUM; ++value)
	{
		logLevel(value, "(undefined)", false, false, false, &GASHA_ stdConsole::instance(), &GASHA_ stdConsole::instance(), GASHA_ consoleColor(), GASHA_ consoleColor());
		m_isAlreadyPool[value] = false;
	}
	//既定のログレベルを設定（コンストラクタで要素を登録）
	level_asNormal();//通常メッセージ
	level_asVerbose();//冗長メッセージ
	level_asDetail();//詳細メッセージ
	level_asImportant();//重要メッセージ
	level_asWarning();//警告メッセージ
	level_asCritical();//重大メッセージ
	level_asAbsolute();//絶対メッセージ（ログレベルに関係なく出力したいメッセージ）
	level_asSilent();//静寂（絶対メッセ―ジ以外出力しない）
	level_asSilentAbsolutely();//絶対静寂（全てのメッセージを出力しない）
}

//レベルコンテナの静的変数をインスタンス化
bool logLevel::container::m_isInitialized = false;
logLevel* logLevel::container::m_poolPtr = nullptr;
logLevel::byte logLevel::container::m_pool[sizeof(logLevel) * logLevel::POOL_NUM];
std::bitset<logLevel::POOL_NUM> logLevel::container::m_isAlreadyPool;

//コンテナのコンストラクタで起動時に初期化処理を実行するためのインスタンス
static logLevel::container s_logLevelContainerInitializer;

#endif//GASHA_HAS_DEBUG_LOG//デバッグログ無効時はまるごと無効化

GASHA_NAMESPACE_END;//ネームスペース：終了

// End of file
