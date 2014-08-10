//--------------------------------------------------------------------------------
// unit_test.cpp
// ユニットテスト【関数／実体定義部】
//
// Gakimaru's standard library for C++ - GASHA
//   Copyright (c) 2014 Itagaki Mamoru
//   Released under the MIT license.
//     https://github.com/gakimaru/gasha/blob/master/LICENSE
//--------------------------------------------------------------------------------

#include <gasha/unit_test.inl>//ユニットテスト【インライン関数／テンプレート関数定義部】

#ifdef GASHA_UNIT_TEST_ENABLED//ユニットテスト無効化時はまるごと無効化

#include <gasha/console_color.h>//コンソールカラー
#include <gasha/std_console.h>//標準コンソール
#include <gasha/fast_string.h>//高速文字列処理
#include <gasha/utility.h>//汎用ユーティリティ

#include <utility>//std::forward

//【VC++】ユニットテスト登録よりも先に、確実に初期化を先に行わせるための設定
#ifdef GASHA_IS_VC
#pragma warning(push)
#pragma warning(disable : 4073)
#pragma init_seg(lib)
#pragma warning(pop)
#endif//GASHA_IS_VC

GASHA_NAMESPACE_BEGIN;//ネームスペース：開始

//--------------------------------------------------------------------------------
//ユニットテスト
//--------------------------------------------------------------------------------

namespace ut
{
	//ユニットテスト情報
	container::funcInfo g_registeredInfo[UNIT_TEST_REGIST_NUM_MAX];//登録情報
	std::size_t g_registeredNum = 0;//登録件数
	std::size_t g_lastPassedTotal = 0;//成功件数
	std::size_t g_lastMissedTotal = 0;//失敗件数
	std::size_t g_minStackRemain = 0;//最小スタック残量
	std::size_t g_minStrRemain = 0;//最小文字列登録数残量
	std::size_t g_minStrBuffRemain = 0;//最小文字列バッファ残量

	//ユニットテスト比較演算子用文字列
	const char* g_opeEnumStr[OPE_NUM] =
	{
		"??",//unknown
		"==",//EQ
		"!=",//NE
		">",//GT
		">=",//GE
		"<",//LT
		"<="//LE
	};
	
	//ユニットテスト比較演算子指定用タグ
	const unknownOpe_tag unknownOpe;
	const EQ_tag EQ;
	const NE_tag NE;
	const GT_tag GT;
	const GE_tag GE;
	const LT_tag LT;
	const LE_tag LE;

	//出力先コンソール
	iConsole* g_console = &GASHA_ stdOutConsole::instance();

	//スタックアロケータ
	//※実行時のワークバッファ
	GASHA_ stackAllocator_withBuff<UNIT_TEST_ALLOCATOR_SIZE> g_allocator GASHA_INIT_PRIORITY_ATTRIBUTE(101);
	GASHA_ scopedStackAllocator<GASHA_ stackAllocator<>>* s_scopeAllocator = nullptr;

	//文字列プール
	strPool<UNIT_TEST_ALLOCATOR_SIZE, UNIT_TEST_STR_POOL_TABLE_SIZE> g_strPool GASHA_INIT_PRIORITY_ATTRIBUTE(101);

	//出力用ワークバッファ
	char g_workBuff[UNIT_TEST_WORK_BUFF_SIZE];

	//コンソールカラーセット
	static void consoleColor_begin()
	{
		if (!g_console)
			return;
		g_console->begin();
	}
	static void consoleColor_reset()
	{
		if (!g_console)
			return;
		g_console->resetColor();
	}
	static void consoleColor_normal()
	{
		if (!g_console)
			return;
		g_console->changeColor(GASHA_ consoleColor(GASHA_ consoleColor::iWHITE));
	}
	static void consoleColor_ok()
	{
		if (!g_console)
			return;
		g_console->changeColor(GASHA_ consoleColor(GASHA_ consoleColor::iWHITE, GASHA_ consoleColor::iBLUE));
	}
	static void consoleColor_ng()
	{
		if (!g_console)
			return;
		g_console->changeColor(GASHA_ consoleColor(GASHA_ consoleColor::iWHITE, GASHA_ consoleColor::iRED));
	}
	static void consoleColor_unknown()
	{
		if (!g_console)
			return;
		g_console->changeColor(GASHA_ consoleColor(GASHA_ consoleColor::iRED, GASHA_ consoleColor::iYELLOW));
	}
	static void consoleColor_expr()
	{
		if (!g_console)
			return;
		g_console->changeColor(GASHA_ consoleColor(GASHA_ consoleColor::iGREEN));
	}
	static void consoleColor_ope()
	{
		if (!g_console)
			return;
		g_console->changeColor(GASHA_ consoleColor(GASHA_ consoleColor::iYELLOW));
	}
	static void consoleColor_expect()
	{
		if (!g_console)
			return;
		g_console->changeColor(GASHA_ consoleColor(GASHA_ consoleColor::iGREEN));
	}
	static void consoleColor_value()
	{
		if (!g_console)
			return;
		g_console->changeColor(GASHA_ consoleColor(GASHA_ consoleColor::iGREEN));
	}
	static void consoleColor_elapsedTime()
	{
		if (!g_console)
			return;
		g_console->changeColor(GASHA_ consoleColor(GASHA_ consoleColor::iYELLOW));
	}
	static void consoleColor_exception()
	{
		if (!g_console)
			return;
		g_console->changeColor(GASHA_ consoleColor(GASHA_ consoleColor::iMAGENTA));
	}
	static void consoleColor_end()
	{
		if (!g_console)
			return;
		g_console->end();
	}
	
	//----------------------------------------
	//ユニットテスト結果基底クラス
	
	//アクセッサ
	void resultBase::setException(std::exception const& e)
	{
		m_hasException = true; m_exceptionStr = e.what();
	}
	void resultBase::setExprStr(const char* expr)
	{
		if (expr)
		{
			//m_exprStr = g_strPool.regist(expr);
			m_exprStr = expr;
			m_hasExprStr = true;
		}
	}
	void resultBase::setValueStr(const char* value)
	{
		if (value)
		{
			m_valuieStr = g_strPool.regist(value);
			m_hasValueStr = true;
		}
	}
	void resultBase::setOpeStr(const char* ope)
	{
		if (ope)
		{
			//m_opeStr = g_strPool.regist(ope);
			m_opeStr = ope;
			m_hasOpeStr = true;
		}
	}
	const char* resultBase::setOpeStrFromId(const opeEnum ope)
	{
		setOpeStr(g_opeEnumStr[ope]);
		return opeStr();
	}
	void resultBase::setExpectStr(const char* expect)
	{
		if (expect)
		{
			//m_expectStr = g_strPool.regist(expect);
			m_expectStr = expect;
			m_hasExpectStr = true;
		}
	}
	
	//----------------------------------------
	//ユニットテストメインクラス
	
	//ユニットテスト登録
	bool container::add(testFunc func, const char* module_name, const int group_id, const attr_type attr)
	{
		GASHA_SIMPLE_ASSERT(g_registeredNum < UNIT_TEST_REGIST_NUM_MAX, "Entry of unit-test is overed.");
		if (g_registeredNum >= UNIT_TEST_REGIST_NUM_MAX)
			return false;
		funcInfo* info = &g_registeredInfo[g_registeredNum++];
		info->m_func = func;
		info->m_moduleName = module_name;
		info->m_groupId = group_id;
		info->m_attr = attr;
		info->passed = 0;
		info->missed = 0;
		info->elapsed_time = 0.;
		return true;
	}

	//アクセッサ
	std::size_t container::size()
	{
		return g_registeredNum;
	}
	const container::funcInfo* container::head()
	{
		return g_registeredInfo;
	}
	const container::funcInfo* container::at(const int index)
	{
		return index >= 0 && index < static_cast<int>(g_registeredNum) ? &g_registeredInfo[index] : nullptr;
	}
	const container::funcInfo* container::at(const char* module_name)
	{
		funcInfo* info = g_registeredInfo;
		for (std::size_t i = 0; i < g_registeredNum; ++i, ++info)
		{
			if (GASHA_ strcmp_fast(module_name, info->m_moduleName) == 0)
			{
				return  &g_registeredInfo[i];
			}
		}
		return nullptr;
	}
	int container::lastPassedCount()
	{
		return static_cast<int>(g_lastPassedTotal);
	}
	int container::lastMissedCount()
	{
		return static_cast<int>(g_lastMissedTotal);
	}
	
	//前回の実行結果をリセット
	void container::resetLastResult()
	{
		g_lastPassedTotal = 0;
		g_lastMissedTotal = 0;
		funcInfo* info = g_registeredInfo;
		for (std::size_t i = 0; i < g_registeredNum; ++i, ++info)
		{
			info->passed = 0;
			info->missed = 0;
			info->elapsed_time = 0.;
		}
	}
	
	//ユニットテスト結果登録
	result<int>* container::makeResultWithoutValueAndExpect(const char* expr_str)
	{
		result<int>* obj = s_scopeAllocator->template newObj<result<int>>();
		obj->setExprStr(expr_str);
		obj->setResult(false);
		obj->setHasResult(false);
		return obj;
	}
	
	//ユニットテスト実行
	int container::runByModuleAndGroup(const char* target_module_name, const int target_group_id, const attr_type target_attr)
	{
		g_strPool.clear();
		std::size_t passed_total = 0;
		std::size_t missed_total = 0;
		GASHA_ sec_t elapsed_time_total_sec = 0.;
		outputBegin(target_module_name, target_group_id, target_attr);
		funcInfo* info = g_registeredInfo;
		for (std::size_t i = 0; i < g_registeredNum; ++i, ++info)
		{
			if ((target_module_name == nullptr || (target_module_name != nullptr && info->m_moduleName != nullptr && GASHA_ strcmp_fast(target_module_name, info->m_moduleName) == 0)) &&
				(target_group_id == 0 || (target_group_id != 0 && target_group_id == info->m_groupId)) &&
				(target_attr == ATTR_ANY || (target_attr != ATTR_ANY && (target_attr & info->m_attr) != ATTR_NONE)))
			{
				auto scoped_allocator = g_allocator.scopedAllocator();
				s_scopeAllocator = &scoped_allocator;
				
				std::size_t passed = 0;
				std::size_t missed = 0;
				GASHA_ elapsedTime elapsed_time;
				info->m_func(passed, missed);
				const GASHA_ sec_t elapsed_time_sec = elapsed_time.now();
				elapsed_time_total_sec += elapsed_time_sec;
				passed_total += passed;
				missed_total += missed;
				info->passed = passed;
				info->missed = missed;
				info->elapsed_time = elapsed_time_sec;
				
				g_minStackRemain = GASHA_ minIf(g_minStackRemain, g_allocator.remain());
				g_minStrRemain = GASHA_ minIf(g_minStrRemain, g_strPool.remain());
				g_minStrBuffRemain = GASHA_ minIf(g_minStrBuffRemain, g_strPool.buffRemain());
			}
		}
		g_lastPassedTotal = passed_total;
		g_lastMissedTotal = missed_total;
		outputEnd(target_module_name, target_group_id, target_attr, passed_total, missed_total, elapsed_time_total_sec);
		return static_cast<int>(missed_total);
	}

	//ユニットテスト結果表示
	void container::setConsole(GASHA_ iConsole& console)
	{
		g_console = &console;
	}
	void container::outputBegin(const char* target_module_name, const int target_group_id, const GASHA_ ut::attr_type target_attr)
	{
		consoleColor_begin();
		consoleColor_reset();
		consoleColor_normal();
		put("\n");
		put("Start unit test: ");
		if (target_module_name)
		{
			printf("[Target module=\"%s\"]", target_module_name);
		}
		if (target_group_id != 0)
		{
			printf("[Target group=%d]", target_group_id);
		}
		if (!target_module_name && target_group_id == 0)
		{
			put("[Target=All]");
		}
		if (target_attr != ATTR_ANY)
		{
			printf("[Attr=0x%08x]", target_attr);
		}
		else
		{
			put("[Attr=ANY]");
		}
		put("\n");
		put("============================================================\n");
		consoleColor_end();
	}
	void container::outputEnd(const char* target_module_name, const int target_group_id, const GASHA_ ut::attr_type target_attr, const std::size_t passed_total, const std::size_t missed_total, const GASHA_ sec_t elapsed_time_total)
	{
		consoleColor_begin();
		consoleColor_reset();
		consoleColor_normal();
		put("\n");
		put("============================================================\n");
		printf("Finish unit test: Total [test=%d, passed=", static_cast<int>(passed_total + missed_total));
		if (passed_total > 0)
		{
			consoleColor_ok();
			printf("%d", static_cast<int>(passed_total));
		}
		else
		{
			consoleColor_normal();
			printf("%d", static_cast<int>(passed_total));
		}
		consoleColor_normal();
		put(", missed=");
		if (missed_total > 0)
		{
			consoleColor_ng();
			printf("%d", static_cast<int>(missed_total));
		}
		else
		{
			consoleColor_normal();
			printf("%d", static_cast<int>(missed_total));
		}
		consoleColor_normal();
		put("] ");
		consoleColor_elapsedTime();
		printf("%.9lf sec", elapsed_time_total);
		consoleColor_normal();
		put(" -----\n");
		printf("(registeredNum=%d, minStackRemain=%d, minStrRemain=%d, minStrBuffRemain=%d)", static_cast<int>(g_registeredNum), static_cast<int>(g_minStackRemain), static_cast<int>(g_minStrRemain), static_cast<int>(g_minStrBuffRemain));
		put("\n");
		consoleColor_end();
	}
	void container::outputModuleBegin(const char* module_name, const int group_id, const GASHA_ ut::attr_type attr)
	{
		consoleColor_begin();
		consoleColor_reset();
		consoleColor_normal();
		put("\n");
		printf("----- Start unit test module: \"%s\" (Group=%d,Attr=0x%08x) -----\n", module_name, group_id, attr);
		consoleColor_end();
	}
	void container::outputModuleEnd(const char* module_name, const int group_id, const GASHA_ ut::attr_type attr, const std::size_t passed, const std::size_t missed, const GASHA_ sec_t elapsed_time)
	{
		consoleColor_begin();
		consoleColor_reset();
		consoleColor_normal();
		printf("----- Finish unit test module: [test=%d, passed=", passed + missed);
		if (passed > 0)
		{
			consoleColor_ok();
			printf("%d", static_cast<int>(passed));
		}
		else
		{
			consoleColor_normal();
			printf("%d", static_cast<int>(passed));
		}
		consoleColor_normal();
		put(", missed=");
		if (missed > 0)
		{
			consoleColor_ng();
			printf("%d", static_cast<int>(missed));
		}
		else
		{
			consoleColor_normal();
			printf("%d", static_cast<int>(missed));
		}
		consoleColor_normal();
		put("]");
		consoleColor_normal();
		put(" ");
		consoleColor_elapsedTime();
		printf("%.9lf sec", elapsed_time);
		consoleColor_normal();
		put(" -----\n");
		consoleColor_end();
	}
	void container::outputResult(const bool is_child, std::size_t* passed, std::size_t* missed, const GASHA_ sec_t elapsed_time_sec, resultBase* result_obj)
	{
		consoleColor_begin();
		consoleColor_reset();
		consoleColor_normal();
		bool is_count_passed = false;
		bool is_count_missed = false;
		if (result_obj->hasOpeStr() && result_obj->hasExpectStr())
		{
			if (result_obj->hasResult())
			{
				if (result_obj->result())
				{
					is_count_passed = true;
					consoleColor_normal();
					put(" ");
					consoleColor_ok();
					put("[OK]");
					consoleColor_normal();
					put(" ");
				}
				else
				{
					is_count_missed = true;
					consoleColor_ng();
					put("*[NG!]");
				}
			}
			else
			{
				is_count_missed = true;
				consoleColor_unknown();
				put("*[??]");
				consoleColor_normal();
				put(" ");
			}
			consoleColor_normal();
			put(" <-- ");
		}
		else
		{
			consoleColor_normal();
			put("      ");
			put("     ");
		}
		if (is_child)
		{
			consoleColor_normal();
			put("    ");
		}
		if (result_obj->hasExprStr())
		{
			consoleColor_expr();
			printf("%s", result_obj->exprStr());
		}
		if (result_obj->hasOpeStr())
		{
			consoleColor_normal();
			put(" ");
			consoleColor_ope();
			printf("%s", result_obj->opeStr());
			consoleColor_normal();
			put(" ");
		}
		if (result_obj->hasExpectStr())
		{
			consoleColor_expect();
			printf("%s", result_obj->expectStr());
		}
		if (result_obj->hasValueStr())
		{
			consoleColor_normal();
			put(" (ret=");
			consoleColor_value();
			printf("%s", result_obj->valueStr());
			consoleColor_normal();
			put(")");
		}
		if (elapsed_time_sec != 0.)
		{
			consoleColor_normal();
			put(" ");
			consoleColor_elapsedTime();
			printf("%.9lf sec", elapsed_time_sec);
		}
		if (result_obj->hasException())
		{
			is_count_missed = true;
			consoleColor_normal();
			put(" ");
			consoleColor_exception();
			put("<EXCEPTION!!");
			const char* msg = result_obj->exceptionStr();
			if (msg)
			{
				printf(":%s", msg);
			}
			put(">");
		}
		consoleColor_reset();
		put("\n");
		consoleColor_end();

		if (is_count_passed && passed)
		{
			++(*passed);
		}
		if (is_count_missed && missed)
		{
			++(*missed);
		}
	}
}//namespace ut

GASHA_NAMESPACE_END;//ネームスペース：終了

#include <gasha/str_pool.cpp.h>//文字列プール【関数／実体定義部】
GASHA_INSTANCING_strPool(GASHA_ ut::UNIT_TEST_STR_POOL_SIZE, GASHA_ ut::UNIT_TEST_STR_POOL_TABLE_SIZE);

#else//GASHA_UNIT_TEST_ENABLED

GASHA_NAMESPACE_BEGIN;//ネームスペース：開始

//【VC++】LNK4221回避用のダミー関数
namespace _private{
	void unit_test_dummy(){}
}//namespace _private

GASHA_NAMESPACE_END;//ネームスペース：終了

#endif//GASHA_UNIT_TEST_ENABLED//ユニットテスト無効化時はまるごと無効化

// End of file
