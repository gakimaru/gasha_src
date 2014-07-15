//--------------------------------------------------------------------------------
// log_attr.cpp
// ログ属性【関数／実体定義部】
//
// Gakimaru's researched and standard library for C++ - GASHA
//   Copyright (c) 2014 Itagaki Mamoru
//   Released under the MIT license.
//     https://github.com/gakimaru/gasha/blob/master/LICENSE
//--------------------------------------------------------------------------------

#include <gasha/log_attr.inl>//ログ属性【インライン関数／テンプレート関数定義部】

GASHA_NAMESPACE_BEGIN;//ネームスペース：開始

//--------------------------------------------------------------------------------
//ログ属性
//--------------------------------------------------------------------------------

#ifdef GASHA_HAS_DEBUG_LOG//デバッグログ無効時はまるごと無効化

//--------------------
//ログ属性

//参照するログ属性を変更
void logAttr::changeRef(const logAttr::ref_type type)
{
	//一旦変更前のTLSを復元する
	if (m_refType == isLocal || m_prevTlsAttr)
	{
		m_tlsAttrRef = m_prevTlsAttr;
		m_prevTlsAttr = nullptr;
	}

	//種別変更
	m_refType = type;

	//TLSの場合
	if (m_refType == isTls)
	{
		if (m_tlsAttrRef)
			m_attrRef = m_tlsAttrRef;
		else
			m_refType = isGlobal;//TLSがなければグローバルに変更
	}

	//グローバルの場合
	if (m_refType == isGlobal)
	{
		m_attrRef = &m_globalAttr;
		if (m_tlsAttrRef)
		{
			m_prevTlsAttr = m_tlsAttrRef;
			m_tlsAttrRef = &m_globalAttr;
		}
	}

	//ローカルの場合、TLSもしくはグローバルからローカルにコピーし、TLSを変更する
	if (m_refType == isLocal)
	{
		m_prevTlsAttr = m_tlsAttrRef;
		attr_type& src = m_prevTlsAttr ? *m_prevTlsAttr : m_globalAttr;
		m_localAttr = src;
		m_tlsAttrRef = &m_localAttr;
		m_attrRef = &m_localAttr;
	}
}


//グローバルログレベルマスク初期化（一回限り）
void logAttr::initializeOnce()
{
	m_globalAttr = DEFAULT_ATTR;
}

//デフォルトコンストラクタ
logAttr::logAttr()
{
#ifdef GASHA_LOG_ATTR_SECURE_INITIALIZE
	std::call_once(m_initialized, initializeOnce);//グローバルログレベルマスク初期化（一回限り）
#endif//GASHA_LOG_ATTR_SECURE_INITIALIZE
	if (m_tlsAttrRef)//TLSログ属性がある場合
	{
		m_refType = isTls;
		m_attrRef = m_tlsAttrRef;
		m_prevTlsAttr = nullptr;
	}
	else
	{
		m_refType = isGlobal;
		m_attrRef = &m_globalAttr;
		m_prevTlsAttr = nullptr;
	}
}

//コンテナの静的変数をインスタンス化
const logAttr::explicitInitialize_t logAttr::explicitInitialize;
std::once_flag logAttr::m_initialized;
logAttr::attr_type logAttr::m_globalAttr;
thread_local logAttr::attr_type* logAttr::m_tlsAttrRef = nullptr;

#endif//GASHA_HAS_DEBUG_LOG//デバッグログ無効時はまるごと無効化

GASHA_NAMESPACE_END;//ネームスペース：終了

// End of file
