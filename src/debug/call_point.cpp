//--------------------------------------------------------------------------------
// call_point.cpp
// コールポイント【関数／実体定義部】
//
// Gakimaru's researched and standard library for C++ - GASHA
//   Copyright (c) 2014 Itagaki Mamoru
//   Released under the MIT license.
//     https://github.com/gakimaru/gasha/blob/master/LICENSE
//--------------------------------------------------------------------------------

#include <gasha/call_point.inl>//コールポイント【インライン関数／テンプレート関数定義部】

#include <gasha/string.inl>//文字列処理：spprintf
#include <gasha/thread_id.inl>//スレッドID

GASHA_NAMESPACE_BEGIN;//ネームスペース：開始

//--------------------------------------------------------------------------------
//コールポイント
//--------------------------------------------------------------------------------

#ifdef GASHA_CALLPOINT_IS_ENABLED//デバッグログ無効時はまるごと無効化

//--------------------
//コールポイント

//直近の重大コールポイントを検索
const criticalCallPoint* callPoint::findCritical() const
{
	callPoint* cp = m_callStackTop;
	while (cp)
	{
		if (cp->m_type == isCritical)
			return static_cast<criticalCallPoint*>(cp);
		cp = cp->m_prevCallStack;
	}
	return nullptr;
}

//プロファイリングする
void callPoint::profiling()
{
	//...
}

//デバッグ情報作成
std::size_t callPoint::debugInfo(char* message, const std::size_t max_size) const
{
	if (!message)
		return 0;
	std::size_t message_len = 0;
	std::size_t ret = 0;
	ret = GASHA_ spprintf(message, max_size, message_len, "----- callPoint stack info -----\n");
	GASHA_ threadId thread_id;
	ret = GASHA_ spprintf(message, max_size, message_len, "Thread: \"%s\"(ID=%08x)\n", thread_id.thisName(), thread_id.thisId());
	
	//コールポイント数の集計と出力
	const callPoint* cp = m_callStackTop;
	int num_of_cp = 0;
	while (cp)
	{
		cp = cp->m_prevCallStack;
		++num_of_cp;
	}
	ret = GASHA_ spprintf(message, max_size, message_len, "Num of point: %d\n", num_of_cp);
	if (num_of_cp > 0)
		ret = GASHA_ spprintf(message, max_size, message_len, "------------------------------------\n");

	//コールポイントスタックの出力
	cp = m_callStackTop;
	int depth = 0;
	while (cp)
	{
		//インデント表示関数
		auto print_indent = [&]()
		{
			for (int indent = 0; indent < depth; ++indent)
				ret = GASHA_ spprintf(message, max_size, message_len, "  ");
		};
		
		//コールポイント名取得
		const char* name = cp->name();
		if (!name)
			name = "(unknown)";
		
		//種別
		const char* type_str = "";
		if (cp->type() == isCritical)
			type_str = "<CRITICAL>";

		//自動プロファイル設定
		const char* auto_profiling_str = "";
		if (cp->autoProfiling() == useAutoProfiling)
			auto_profiling_str = "<Auto-Profiling>";

		//カテゴリ名取得
		logCategory category(cp->category());
		const char* category_name = category.name();
		if (!category_name)
			category_name = "(unknown)";

		//ファイル名取得
		const char* file_name = cp->fileName();
		if (!file_name)
			file_name = "(no file name)";

		//関数名取得
		const char* func_name = cp->funcName();
		if (!func_name)
			func_name = "(no func name)";

		//開始時間
		char begin_time_str[32];
		GASHA_ timeToStr(begin_time_str, cp->beginTime(), timeStr_HHMMSS_MICRO);

		//文字列作成
		print_indent();
		ret = GASHA_ spprintf(message, max_size, message_len, "[%s]\"%s\"(addr=%p)%s%s\n", category_name, name, cp, type_str, auto_profiling_str);
		print_indent();
		ret = GASHA_ spprintf(message, max_size, message_len, "        src: %s\n", file_name);
		print_indent();
		ret = GASHA_ spprintf(message, max_size, message_len, "        func:%s\n", func_name);

		//次のコールポイント
		cp = cp->m_prevCallStack;
		++depth;
	}
	
	//終了
	ret = GASHA_ spprintf(message, max_size, message_len, "------------------------------------");//最終行改行なし
	return ret == 0 ? 0 : message_len;
}

//静的変数をインスタンス化
thread_local callPoint* callPoint::m_callStackTop;//コールスタックの先頭

#endif//GASHA_CALLPOINT_IS_ENABLED//デバッグログ無効時はまるごと無効化

GASHA_NAMESPACE_END;//ネームスペース：終了

// End of file
