//--------------------------------------------------------------------------------
// type_traits.cpp
// 型特性ユーティリティ【関数／実体定義部】
//
// Gakimaru's standard library for C++ - GASHA
//   Copyright (c) 2014 Itagaki Mamoru
//   Released under the MIT license.
//     https://github.com/gakimaru/gasha/blob/master/LICENSE
//--------------------------------------------------------------------------------

#include <gasha/type_traits.inl>//型特性ユーティリティ【インライン関数／テンプレート関数定義部】

#include <gasha/string.h>//文字列処理：sprintf()

GASHA_NAMESPACE_BEGIN;//ネームスペース：開始

//--------------------------------------------------------------------------------
//文字列化
//--------------------------------------------------------------------------------

//値を文字列に変換
//※char型に特殊化
template<>
const char* toNumStr<char>(char* buff, const char value)
{
	GASHA_ spprintf(buff, "%d", static_cast<int>(value));
	return buff;
}
template<>
const char* toHexStr<char>(char* buff, const char value)
{
	GASHA_ spprintf(buff, "0x%02x", static_cast<unsigned int>(value)& 0xff);
	return buff;
}
template<>
const char* toByteStr<char>(char* buff, const char value)
{
	buff[0] = value;
	buff[1] = '\0';
	return buff;
}
//※unsinged char型に特殊化
template<>
const char* toNumStr<unsigned char>(char* buff, const unsigned char value)
{
	GASHA_ spprintf(buff, "%u", static_cast<unsigned int>(value));
	return buff;
}
template<>
const char* toHexStr<unsigned char>(char* buff, const unsigned char value)
{
	GASHA_ spprintf(buff, "0x%02x", static_cast<unsigned int>(value));
	return buff;
}
template<>
const char* toByteStr<unsigned char>(char* buff, const unsigned char value)
{
	buff[0] = value;
	buff[1] = '\0';
	return buff;
}
//※short型に特殊化
template<>
const char* toNumStr<short>(char* buff, const short value)
{
	GASHA_ spprintf(buff, "%d", static_cast<int>(value));
	return buff;
}
template<>
const char* toHexStr<short>(char* buff, const short value)
{
	GASHA_ spprintf(buff, "0x%04x", static_cast<unsigned int>(value)& 0xffff);
	return buff;
}
template<>
const char* toByteStr<short>(char* buff, const short value)
{
	unionTypes uni_value(value);
	std::size_t i = 0;
	for (; i < sizeof(value); ++i)
		buff[i] = uni_value.m_char[i];
	buff[i] = '\0';
	return buff;
}
//※unsinged short型に特殊化
template<>
const char* toNumStr<unsigned short>(char* buff, const unsigned short value)
{
	GASHA_ spprintf(buff, "%u", static_cast<int>(value));
	return buff;
}
template<>
const char* toHexStr<unsigned short>(char* buff, const unsigned short value)
{
	GASHA_ spprintf(buff, "0x%04x", static_cast<unsigned int>(value));
	return buff;
}
template<>
const char* toByteStr<unsigned short>(char* buff, const unsigned short value)
{
	unionTypes uni_value(value);
	std::size_t i = 0;
	for (; i < sizeof(value); ++i)
		buff[i] = uni_value.m_char[i];
	buff[i] = '\0';
	return buff;
}
//※int型に特殊化
template<>
const char* toNumStr<int>(char* buff, const int value)
{
	GASHA_ spprintf(buff, "%d", value);
	return buff;
}
template<>
const char* toHexStr<int>(char* buff, const int value)
{
	GASHA_ spprintf(buff, "0x%08x", value);
	return buff;
}
template<>
const char* toByteStr<int>(char* buff, const int value)
{
	unionTypes uni_value(value);
	std::size_t i = 0;
	for (; i < sizeof(value); ++i)
		buff[i] = uni_value.m_char[i];
	buff[i] = '\0';
	return buff;
}
//※unsinged int型に特殊化
template<>
const char* toNumStr<unsigned int>(char* buff, const unsigned int value)
{
	GASHA_ spprintf(buff, "%u", value);
	return buff;
}
template<>
const char* toHexStr<unsigned int>(char* buff, const unsigned int value)
{
	GASHA_ spprintf(buff, "0x%08x", value);
	return buff;
}
template<>
const char* toByteStr<unsigned int>(char* buff, const unsigned int value)
{
	unionTypes uni_value(value);
	std::size_t i = 0;
	for (; i < sizeof(value); ++i)
		buff[i] = uni_value.m_char[i];
	buff[i] = '\0';
	return buff;
}
//※long型に特殊化
template<>
const char* toNumStr<long>(char* buff, const long value)
{
	GASHA_ spprintf(buff, "%ld", value);
	return buff;
}
template<>
const char* toHexStr<long>(char* buff, const long value)
{
	GASHA_ spprintf(buff, "0x%08lx", value);
	return buff;
}
template<>
const char* toByteStr<long>(char* buff, const long value)
{
	unionTypes uni_value(value);
	std::size_t i = 0;
	for (; i < sizeof(value); ++i)
		buff[i] = uni_value.m_char[i];
	buff[i] = '\0';
	return buff;
}
//※unsinged long型に特殊化
template<>
const char* toNumStr<unsigned long>(char* buff, const unsigned long value)
{
	GASHA_ spprintf(buff, "%lu", value);
	return buff;
}
template<>
const char* toHexStr<unsigned long>(char* buff, const unsigned long value)
{
	GASHA_ spprintf(buff, "0x%08lx", value);
	return buff;
}
template<>
const char* toByteStr<unsigned long>(char* buff, const unsigned long value)
{
	unionTypes uni_value(value);
	std::size_t i = 0;
	for (; i < sizeof(value); ++i)
		buff[i] = uni_value.m_char[i];
	buff[i] = '\0';
	return buff;
}
//※long long型に特殊化
template<>
const char* toNumStr<long long>(char* buff, const long long value)
{
	GASHA_ spprintf(buff, "%lld", value);
	return buff;
}
template<>
const char* toHexStr<long long>(char* buff, const long long value)
{
	GASHA_ spprintf(buff, "0x%016llx", value);
	return buff;
}
template<>
const char* toByteStr<long long>(char* buff, const long long value)
{
	unionTypes uni_value(value);
	std::size_t i = 0;
	for (; i < sizeof(value); ++i)
		buff[i] = uni_value.m_char[i];
	buff[i] = '\0';
	return buff;
}
//※unsinged long long型に特殊化
template<>
const char* toNumStr<unsigned long long>(char* buff, const unsigned long long value)
{
	GASHA_ spprintf(buff, "%llu", value);
	return buff;
}
template<>
const char* toHexStr<unsigned long long>(char* buff, const unsigned long long value)
{
	GASHA_ spprintf(buff, "0x%016llx", value);
	return buff;
}
template<>
const char* toByteStr<unsigned long long>(char* buff, const unsigned long long value)
{
	unionTypes uni_value(value);
	std::size_t i = 0;
	for (; i < sizeof(value); ++i)
		buff[i] = uni_value.m_char[i];
	buff[i] = '\0';
	return buff;
}
//※float型に特殊化
template<>
const char* toNumStr<float>(char* buff, const float value)
{
	GASHA_ spprintf(buff, "%f", value);
	return buff;
}
template<>
const char* toHexStr<float>(char* buff, const float value)
{
	unionTypes uni_value(value);
	GASHA_ spprintf(buff, "0x%08x", uni_value.m_uint[0]);
	return buff;
}
template<>
const char* toByteStr<float>(char* buff, const float value)
{
	unionTypes uni_value(value);
	std::size_t i = 0;
	for (; i < sizeof(value); ++i)
		buff[i] = uni_value.m_char[i];
	buff[i] = '\0';
	return buff;
}
//※double型に特殊化
template<>
const char* toNumStr<double>(char* buff, const double value)
{
	GASHA_ spprintf(buff, "%lf", value);
	return buff;
}
template<>
const char* toHexStr<double>(char* buff, const double value)
{
	unionTypes uni_value(value);
	GASHA_ spprintf(buff, "0x%016llx", uni_value.m_ullong[0]);
	return buff;
}
template<>
const char* toByteStr<double>(char* buff, const double value)
{
	unionTypes uni_value(value);
	std::size_t i = 0;
	for (; i < sizeof(value); ++i)
		buff[i] = uni_value.m_char[i];
	buff[i] = '\0';
	return buff;
}

GASHA_NAMESPACE_END;//ネームスペース：終了

// End of file
