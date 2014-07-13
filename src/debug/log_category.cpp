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

//----------------------------------------
//既定のログカテゴリ定義
#define DECLARE_LOG_CATEGORY(value, for_log, for_notice, fore_color, back_color, fore_color_for_notice, back_color_for_notice) struct category_##value : public regLogCategory<value, for_log, for_notice>{ category_##value () : regLogCategory<value, for_log, for_notice>(#value, GASHA_ consoleColor(GASHA_ consoleColor::fore_color, GASHA_ consoleColor::back_color), GASHA_ consoleColor(GASHA_ consoleColor::fore_color_for_notice, GASHA_ consoleColor::back_color_for_notice)){} }
DECLARE_LOG_CATEGORY(asNormal, true, false, STANDARD, STANDARD, BLACK, iWHITE);//通常メッセージ
DECLARE_LOG_CATEGORY(asVerbose, true, false, iBLACK, STANDARD, iBLACK, iWHITE);//冗長メッセージ
DECLARE_LOG_CATEGORY(asDetail, true, false, iBLACK, STANDARD, iBLACK, iWHITE);//詳細メッセージ
DECLARE_LOG_CATEGORY(asImportant, true, true, iBLUE, STANDARD, iBLUE, iWHITE);//重要メッセージ
DECLARE_LOG_CATEGORY(asWarning, true, true, iMAGENTA, STANDARD, BLACK, iMAGENTA);//警告メッセージ
DECLARE_LOG_CATEGORY(asCritical, true, true, iRED, STANDARD, iYELLOW, iRED);//重大メッセージ
DECLARE_LOG_CATEGORY(asAbsolute, true, false, STANDARD, STANDARD, STANDARD, STANDARD);//絶対メッセージ（ログカテゴリに関係なく出力したいメッセージ）

//----------------------------------------
//特殊ログカテゴリ定義
#define DECLARE_SPECIAL_LOG_CATEGORY(value) struct category_##value : public _private::regSpecialLogCategory<value>{ category_##value () : regSpecialLogCategory<value>(#value){} }
DECLARE_SPECIAL_LOG_CATEGORY(asSilent);//静寂（絶対メッセ―ジ以外出力しない）
DECLARE_SPECIAL_LOG_CATEGORY(asSilentAbsolutely);//絶対静寂（全てのメッセージを出力しない）

//----------------------------------------
//ログカテゴリクラス

//登録済みか？
bool logCategory::isRegistered() const
{
	container con;
	logCategory* obj = con.at(m_value);
	if (!obj)
		return false;
	return obj->m_name == m_name;
}

//--------------------
//イテレータ

//インクリメント
void logCategory::iterator::inc() const
{
	if (m_value == INVALID)
		return;
	else if (m_value == END)
	{
		m_value = INVALID;
		m_ref = nullptr;
		m_isEnd = false;
		return;
	}
	while (m_value != END)
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
void logCategory::iterator::dec() const
{
	m_isEnd = false;
	if (m_value == INVALID)
		return;
	else if (m_value == BEGIN)
	{
		m_value = INVALID;
		m_ref = nullptr;
		return;
	}
	while (m_value != BEGIN)
	{
		--m_value;
		m_ref = container::_at(m_value);
		if (m_ref)
			return;
	}
	m_ref = container::_at(m_value);
	if (!m_ref)
		m_value = INVALID;
	return;
}

//--------------------
//リバースイテレータ

//インクリメント
void logCategory::reverse_iterator::inc() const
{
	if (m_value == INVALID)
		return;
	else if (m_value == BEGIN)
	{
		m_value = INVALID;
		m_ref = nullptr;
		m_isEnd = false;
		return;
	}
	while (m_value != BEGIN)
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
void logCategory::reverse_iterator::dec() const
{
	m_isEnd = false;
	if (m_value == INVALID)
		return;
	else if (m_value == END)
	{
		m_value = INVALID;
		m_ref = nullptr;
		return;
	}
	while (m_value != END)
	{
		++m_value;
		m_ref = container::_at(m_value - 1);
		if (m_ref)
			return;
	}
	m_ref = container::_at(m_value - 1);
	if (!m_ref)
		m_value = INVALID;
	return;
}

//--------------------
//コンテナ（イテレータ用）

//要素を更新
bool logCategory::container::update(const logCategory::category_type value, const logCategory& obj)
{
	if (value >= MIN && value <= MAX && !container::m_isAlreadyPool[value])
	{
		container::m_poolPtr[value] = obj;
		container::m_isAlreadyPool[value] = true;
		return true;
	}
	return false;
}

//全てのログカテゴリのコンソールを変更
void logCategory::container::setAllConsole(IConsole* console)
{
	for (category_type value = NORMAL_MIN; value <= NORMAL_MAX; ++value)
	{
		logCategory* category = at(value);
		if (category)
			category->console() = console;
	}
}

//全てのログカテゴリの画面通知用コンソールを変更
void logCategory::container::setAllConsoleForNotice(IConsole* console)
{
	for (category_type value = NORMAL_MIN; value <= NORMAL_MAX; ++value)
	{
		logCategory* category = at(value);
		if (category)
			category->consoleForNotice() = console;
	}
}

//コンテナ初期化（一回限り）
void logCategory::container::initializeOnce()
{
	//静的変数を初期化
	m_isAlreadyPool.reset();
	m_poolPtr = reinterpret_cast<logCategory*>(m_pool);
	//要素を初期化
	for (logCategory::category_type value = 0; value < logCategory::NUM; ++value)
	{
		logCategory(value, "(undefined)", nullptr, nullptr, GASHA_ consoleColor(), GASHA_ consoleColor());
		m_isAlreadyPool[value] = false;
	}
	//既定のログカテゴリを設定（コンストラクタで要素を登録）
	category_asNormal();//通常メッセージ
	category_asVerbose();//冗長メッセージ
	category_asDetail();//詳細メッセージ
	category_asImportant();//重要メッセージ
	category_asWarning();//警告メッセージ
	category_asCritical();//重大メッセージ
	category_asAbsolute();//絶対メッセージ（ログカテゴリに関係なく出力したいメッセージ）
	category_asSilent();//静寂（絶対メッセ―ジ以外出力しない）
	category_asSilentAbsolutely();//絶対静寂（全てのメッセージを出力しない）
}

//コンテナの静的変数をインスタンス化
std::once_flag logCategory::container::m_initialized;
logCategory* logCategory::container::m_poolPtr = nullptr;
logCategory::byte logCategory::container::m_pool[sizeof(logCategory)* logCategory::POOL_NUM];
std::bitset<logCategory::POOL_NUM> logCategory::container::m_isAlreadyPool;

#endif//GASHA_HAS_DEBUG_LOG//デバッグログ無効時はまるごと無効化

GASHA_NAMESPACE_END;//ネームスペース：終了

// End of file
