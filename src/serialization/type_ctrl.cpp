//--------------------------------------------------------------------------------
// serialization/type_ctrl.cpp
// シリアライズ/型操作クラス【関数／実体定義部】
//
// Gakimaru's standard library for C++ - GASHA
//   Copyright (c) 2014 Itagaki Mamoru
//   Released under the MIT license.
//     https://github.com/gakimaru/gasha/blob/master/LICENSE
//--------------------------------------------------------------------------------

#include <gasha/serialization/type_ctrl.inl>//シリアライズ/型操作【インライン関数／テンプレート関数定義部】

GASHA_NAMESPACE_BEGIN;//ネームスペース：開始

//--------------------------------------------------------------------------------
//シリアライズ
namespace serialization
{
	//--------------------------------------------------------------------------------
	//シリアライズ/型操作クラス
	//--------------------------------------------------------------------------------

	//--------------------
	//型操作テンプレートクラス：float型に特殊化

	//メモリからコピー
	std::size_t typeCtrl<float>::fromMem(const void* mem, const std::size_t mem_size, void* value_p, const std::size_t value_size)
	{
		if (mem_size == sizeof(double))
		{
			//double型と予測
			double value_from;
			GASHA_ serialization::typeCtrlBase::fromMem(mem, mem_size, &value_from, sizeof(value_from));
			const float value_to = static_cast<float>(value_from);
			GASHA_ serialization::typeCtrlBase::toMem(value_p, value_size, &value_to, sizeof(value_to));
		}
		else if (mem_size == sizeof(float))
		{
			//return GASHA_ serialization::typeCtrlBase::fromMemWithConv(mem, mem_size, value_p, value_size);
			return GASHA_ serialization::typeCtrlBase::fromMem(mem, mem_size, value_p, value_size);//エンディアン調整なしでコピー
		}
		else
		{
			//整数型と予測
			long long value_from;
			GASHA_ serialization::typeCtrlBase::fromMem(mem, mem_size, &value_from, sizeof(value_from));
			const float value_to = static_cast<float>(value_from);
			GASHA_ serialization::typeCtrlBase::toMem(value_p, value_size, &value_to, sizeof(value_to));
		}
		return mem_size;
	}

	//--------------------
	//型操作テンプレートクラス：double型に特殊化

	//メモリからコピー
	std::size_t typeCtrl<double>::fromMem(const void* mem, const std::size_t mem_size, void* value_p, const std::size_t value_size)
	{
		if (mem_size == sizeof(float))
		{
			//float型と予測
			float value_from;
			GASHA_ serialization::typeCtrlBase::fromMem(mem, mem_size, &value_from, sizeof(value_from));
			const double value_to = static_cast<float>(value_from);
			GASHA_ serialization::typeCtrlBase::toMem(value_p, value_size, &value_to, sizeof(value_to));
		}
		else if (mem_size == sizeof(double))
		{
			//return GASHA_ serialization::typeCtrlBase::fromMemWithConv(mem, mem_size, value_p, value_size);
			return GASHA_ serialization::typeCtrlBase::fromMem(mem, mem_size, value_p, value_size);//エンディアン調整なしでコピー
		}
		else
		{
			//整数型と予測
			long long value_from;
			GASHA_ serialization::typeCtrlBase::fromMem(mem, mem_size, &value_from, sizeof(value_from));
			const double value_to = static_cast<double>(value_from);
			GASHA_ serialization::typeCtrlBase::toMem(value_p, value_size, &value_to, sizeof(value_to));
		}
		return mem_size;
	}

	//--------------------
	//型操作テンプレートクラス：文字列型（str_type型）に特殊化

	//文字列へ変換
	std::size_t typeCtrl<GASHA_ serialization::str_type>::toStr(char* str, const std::size_t str_max, const void* value_p, const std::size_t value_size)
	{
		std::size_t used = 0;
		std::size_t remain = str_max;
		char* write_p = str;
		if (remain > 1 + 1)
		{
			*(write_p++) = '\"';
			++used;
			--remain;
		}
		const unsigned char* read_p = reinterpret_cast<const unsigned char*>(value_p);
		for (unsigned int i = 0; i < value_size && remain > 1 + 1; ++i)
		{
			const unsigned char c = *(read_p++);
			if (c == '\0')
				break;
			*(write_p++) = c;
			++used;
			++remain;
		}
		if (remain > 1 + 1)
		{
			*(write_p++) = '\"';
			++used;
			--remain;
		}
		if (remain > 1)
			*(write_p) = '\0';
		return used;
	}

	//明示的なインスタンス化
	template class typeCtrl<int>;
	template class typeCtrl<unsigned int>;
	template class typeCtrl<long>;
	template class typeCtrl<unsigned long>;
	template class typeCtrl<long long>;
	template class typeCtrl<unsigned long long>;
	template class typeCtrl<short>;
	template class typeCtrl<unsigned short>;
	template class typeCtrl<char>;
	template class typeCtrl<unsigned char>;
	template class typeCtrl<float>;
	template class typeCtrl<double>;
	template class typeCtrl<GASHA_ serialization::bin_type>;
	template class typeCtrl<GASHA_ serialization::str_type>;

}//namespace serialization

GASHA_NAMESPACE_END;//ネームスペース：終了

// End of file
