//--------------------------------------------------------------------------------
// log_mask.cpp
// ログマスク【関数／実体定義部】
//
// Gakimaru's researched and standard library for C++ - GASHA
//   Copyright (c) 2014 Itagaki Mamoru
//   Released under the MIT license.
//     https://github.com/gakimaru/gasha/blob/master/LICENSE
//--------------------------------------------------------------------------------

#include <gasha/log_mask.inl>//ログマスク【インライン関数／テンプレート関数定義部】

#include <cstring>//memcpy()

GASHA_NAMESPACE_BEGIN;//ネームスペース：開始

//--------------------------------------------------------------------------------
//ログマスク
//--------------------------------------------------------------------------------

#ifdef GASHA_HAS_DEBUG_LOG//デバッグログ無効時はまるごと無効化

//--------------------
//イテレータ

//インクリメント
void logMask::iterator::inc() const
{
	++m_logCategory;
	while (m_logCategory.isExist() && m_logCategory.isSpecial())
		++m_logCategory;
}

//デクリメント
void logMask::iterator::dec() const
{
	--m_logCategory;
	while (m_logCategory.isExist() && m_logCategory.isSpecial())
		--m_logCategory;
}

//ログレベルを変更
void logMask::iterator::changeLevel(const GASHA_ logPurpose purpose, const level_type level)
{
	if (m_logCategory.isExist())
		m_logMask->changeLevel(purpose, level, m_logCategory);
}

//ログレベルを一レベル上に変更
void logMask::iterator::upLevel(const GASHA_ logPurpose purpose)
{
	if (m_logCategory.isExist())
		m_logMask->upLevel(purpose, m_logCategory);
}

//ログレベルを一レベル下に変更
void logMask::iterator::downLevel(const GASHA_ logPurpose purpose)
{
	if (m_logCategory.isExist())
		m_logMask->downLevel(purpose, m_logCategory);
}

//コンストラクタ
logMask::iterator::iterator(const logMask* mask, const category_type category) :
	m_logMask(const_cast<logMask*>(mask)),
	m_logCategory(category)
{
	if (m_logCategory.isExist() && m_logCategory.isSpecial())
		dec();
}
logMask::iterator::iterator(const logMask* mask, const GASHA_ logCategoryContainer::iterator& category) :
	m_logMask(const_cast<logMask*>(mask)),
	m_logCategory(category)
{}

//--------------------
//リバースイテレータ

//インクリメント
void logMask::reverse_iterator::inc() const
{
	++m_logCategory;
	while (m_logCategory.isExist() && m_logCategory.isSpecial())
		++m_logCategory;
}

//デクリメント
void logMask::reverse_iterator::dec() const
{
	--m_logCategory;
	while (m_logCategory.isExist() && m_logCategory.isSpecial())
		--m_logCategory;
}

//ログレベルを変更
void logMask::reverse_iterator::changeLevel(const logPurpose purpose, const level_type level)
{
	if (m_logCategory.isExist())
		m_logMask->changeLevel(purpose, level, m_logCategory);
}

//ログレベルを一レベル上に変更
void logMask::reverse_iterator::upLevel(const logPurpose purpose)
{
	if (m_logCategory.isExist())
		m_logMask->upLevel(purpose, m_logCategory);
}

//ログレベルを一レベル下に変更
void logMask::reverse_iterator::downLevel(const logPurpose purpose)
{
	if (m_logCategory.isExist())
		m_logMask->downLevel(purpose, m_logCategory);
}

//コンストラクタ
logMask::reverse_iterator::reverse_iterator(const logMask* mask, const category_type category) :
	m_logMask(const_cast<logMask*>(mask)),
	m_logCategory(category)
{
	if (m_logCategory.isExist() && m_logCategory.isSpecial())
		inc();
}
logMask::reverse_iterator::reverse_iterator(const logMask* mask, const GASHA_ logCategoryContainer::reverse_iterator& category) :
	m_logMask(const_cast<logMask*>(mask)),
	m_logCategory(category)
{}

//--------------------
//ログマスク

//指定のカテゴリのイテレータを取得
const logMask::iterator logMask::find(const logMask::category_type category) const
{
	if (category < CATEGORY_MIN || category > CATEGORY_MAX)
		return logMask::iterator();
	return logMask::iterator(this, category);
}

//指定のカテゴリのイテレータを取得
logMask::iterator logMask::find(const logMask::category_type category)
{
	const logMask::iterator ite = const_cast<const logMask*>(this)->find(category);
	return *const_cast<logMask::iterator*>(&ite);
}

//ログレベルを変更
void logMask::changeLevel(const GASHA_ logPurpose purpose, const level_type level, const category_type category)
{
	if (category == CATEGORY_FOR_EVERY)
	{
		changeEveryLevel(purpose, level);
		return;
	}
	if (category < CATEGORY_MIN || category > CATEGORY_MAX || level < LEVEL_MIN || level > LEVEL_MAX)
		return;
	level_type& category_level = m_localLogLevelIsEnabled ? m_localLogLevel[purpose][category] : m_globalLogLevel[purpose][category];
	category_level = level;
}

//全ログレベルを変更 ※changeLogLevel に forEvery 指定時と同じ
void logMask::changeEveryLevel(const GASHA_ logPurpose purpose, const level_type level)
{
	if (level < LEVEL_MIN || level > LEVEL_MAX)
		return;
	level_type* category_level = m_localLogLevelIsEnabled ? m_localLogLevel[purpose] : m_globalLogLevel[purpose];
	for (category_type category = CATEGORY_MIN; category <= CATEGORY_MAX; ++category, ++category_level)
		*category_level = level;
}

//ログレベルを一レベル上に変更 ※forEvery は無視する
void logMask::upLevel(const GASHA_ logPurpose purpose, const category_type category)
{
	if (category < CATEGORY_MIN || category > CATEGORY_MAX)
		return;
	level_type& category_level = m_localLogLevelIsEnabled ? m_localLogLevel[purpose][category] : m_globalLogLevel[purpose][category];
	GASHA_ logLevel level_obj(category_level);
	GASHA_ logLevel next_level_obj = level_obj.next();
	if (next_level_obj.isExist())
		category_level = next_level_obj;
}

//ログレベルを一レベル下に変更 ※forEvery は無視する
void logMask::downLevel(const GASHA_ logPurpose purpose, const category_type category)
{
	if (category < CATEGORY_MIN || category > CATEGORY_MAX)
		return;
	level_type& category_level = m_localLogLevelIsEnabled ? m_localLogLevel[purpose][category] : m_globalLogLevel[purpose][category];
	GASHA_ logLevel level_obj(category_level);
	GASHA_ logLevel prev_level_obj = level_obj.prev();
	if (prev_level_obj.isExist())
		category_level = prev_level_obj;
}

//グローバルログマスク初期化（一回限り）
void logMask::initializeOnce()
{
	//要素を初期化
	for (int purpose = 0; purpose < LOG_PURPOSE_NUM; ++purpose)
	{
		level_type default_log_mask = GASHA_ logLevel::fromOutputLevel(purpose == ofLog ? DEFAULT_LOG_MASK_OF_LOG : DEFAULT_LOG_MASK_OF_NOTICE);
		for (category_type category = CATEGORY_MIN; category <= CATEGORY_MAX; ++category)
		{
			level_type& level_mask = m_globalLogLevel[purpose][category];
			level_mask = default_log_mask;
		}
	}
}

//コンテナの静的変数をインスタンス化
const logMask::explicitInitialize_t logMask::explicitInitialize;
std::once_flag logMask::m_initialized;
logMask::level_type logMask::m_globalLogLevel[GASHA_ LOG_PURPOSE_NUM][logMask::CATEGORY_NUM];

#endif//GASHA_HAS_DEBUG_LOG//デバッグログ無効時はまるごと無効化

GASHA_NAMESPACE_END;//ネームスペース：終了

// End of file
