//--------------------------------------------------------------------------------
// chrono.cpp
// 時間処理系ユーティリティ【関数／実体定義部】
//
// Gakimaru's researched and standard library for C++ - GASHA
//   Copyright (c) 2014 Itagaki Mamoru
//   Released under the MIT license.
//     https://github.com/gakimaru/gasha/blob/master/LICENSE
//--------------------------------------------------------------------------------

#include <gasha/chrono.inl>//時間処理系ユーティリティ【インライン関数／テンプレート関数定義部】

#include <gasha/string.h>//文字列処理：spprintf()

#include <cstddef>//std::size_t
#include <cstdint>//C++11 std::uint32_t

GASHA_NAMESPACE_BEGIN;//ネームスペース：開始

//--------------------------------------------------------------------------------
//時間処理
//--------------------------------------------------------------------------------

//----------------------------------------
//経過時間を文字列化
std::size_t timeToStr(char* buff, const time_type time, const timeStrFormat_type format_type)
{
	const double _time = static_cast<double>(time);
	const std::uint32_t total_sec = static_cast<std::uint32_t>(_time);
	const double dec = _time - static_cast<double>(total_sec);
	const std::uint32_t total_minute = total_sec / 60;
	const std::uint32_t total_hour = total_minute / 60;
	const std::uint32_t sec = total_sec % 60;
	const std::uint32_t minute = total_minute % 60;

	std::size_t size = 0;
	switch (format_type)
	{
	case timeStr_S://"秒"（例："0"）
		{
			GASHA_ spprintf(buff, size, "%d", total_sec);
		}
		break;
	case timeStr_S_MILLI://"秒.ミリ秒"（例："0.000"）
		{
			const std::uint32_t dec_i = static_cast<std::uint32_t>(dec * 1000.);
			GASHA_ spprintf(buff, size, "%d.%03d", total_sec, dec_i);
		}
		break;
	case timeStr_S_MICRO://"秒.マイクロ秒"（例："0.000000"）
		{
			const std::uint32_t dec_i = static_cast<std::uint32_t>(dec * 1000000.);
			GASHA_ spprintf(buff, size, "%d.%06d", total_sec, dec_i);
		}
		break;
	case timeStr_S_NANO://"秒.ナノ秒"（例："0.000000000"）
		{
			const std::uint32_t dec_i = static_cast<std::uint32_t>(dec * 1000000000.);
			GASHA_ spprintf(buff, size, "%d.%09d", total_sec, dec_i);
		}
		break;
	case timeStr_MMSS://"分:秒"（例："00:00"）
		{
			GASHA_ spprintf(buff, size, "%02d:%02d", total_minute, sec);
		}
		break;
	case timeStr_MMSS_MILLI://"分:秒.ミリ秒"（例："00:00.000"）
		{
			const std::uint32_t dec_i = static_cast<std::uint32_t>(dec * 1000.);
			GASHA_ spprintf(buff, size, "%02d:%02d.%03d", total_minute, sec, dec_i);
		}
		break;
	case timeStr_MMSS_MICRO://"分:秒.マイクロ秒"（例："00:00.000000"）
		{
			const std::uint32_t dec_i = static_cast<std::uint32_t>(dec * 1000000.);
			GASHA_ spprintf(buff, size, "%02d:%02d.%06d", total_minute, sec, dec_i);
		}
		break;
	case timeStr_MMSS_NANO://"分:秒.ナノ秒"（例："00:00.000000000"）
		{
			const std::uint32_t dec_i = static_cast<std::uint32_t>(dec * 1000000000.);
			GASHA_ spprintf(buff, size, "%02d:%02d.%09d", total_minute, sec, dec_i);
		}
		break;
	case timeStr_HHMMSS://"時:分:秒"（例："00:00:00"）
		{
			GASHA_ spprintf(buff, size, "%02d:%02d:%02d", total_hour, minute, sec);
		}
		break;
	case timeStr_HHMMSS_MILLI://"時:分:秒.ミリ秒"（例："00:00:00.000"）
		{
			const std::uint32_t dec_i = static_cast<std::uint32_t>(dec * 1000.);
			GASHA_ spprintf(buff, size, "%02d:%02d:%02d.%03d", total_hour, minute, sec, dec_i);
		}
		break;
	case timeStr_HHMMSS_MICRO://"時:分:秒.マイクロ秒"（例："00:00:00.000000"）
		{
			const std::uint32_t dec_i = static_cast<std::uint32_t>(dec * 1000000.);
			GASHA_ spprintf(buff, size, "%02d:%02d:%02d.%06d", total_hour, minute, sec, dec_i);
		}
		break;
	case timeStr_HHMMSS_NANO://"時:分:秒.ナノ秒"（例："00:00:00.000000000"）
		{
			const std::uint32_t dec_i = static_cast<std::uint32_t>(dec * 1000000000.);
			GASHA_ spprintf(buff, size, "%02d:%02d:%02d.%09d", total_hour, minute, sec, dec_i);
		}
		break;
	}
	return size;
}

//----------------------------------------
//システム経過時間
//※インスタンス化
elapsedTime g_elapsedTime;

GASHA_NAMESPACE_END;//ネームスペース：終了

// End of file
