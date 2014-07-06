//--------------------------------------------------------------------------------
// basic_math.cpp
// 基本算術【関数定義部】
//
// Gakimaru's researched and standard library for C++ - GASHA
//   Copyright (c) 2014 Itagaki Mamoru
//   Released under the MIT license.
//     https://github.com/gakimaru/gasha/blob/master/LICENSE
//--------------------------------------------------------------------------------

#include <gasha/basic_math.inl>//基本算術【インライン関数／テンプレート関数定義部】

#ifdef ENABLE_BUILTIN_POPCNT
#ifdef GASHA_IS_VC
#include <intrin.h>//__popcnt()
#endif//GASHA_IS_VC
#ifdef GASHA_IS_GCC
//#include "icint.h"//__builtin_popcount()//インクルード不要
#endif//GASHA_IS_GCC
#endif//ENABLE_BUILTIN_POPCNT

#ifdef ENABLE_SSE_POPCNT
#include <nmmintrin.h>//SSE4.2
#endif//ENABLE_SSE_POPCNT

GASHA_NAMESPACE_BEGIN;//ネームスペース：開始

//--------------------------------------------------------------------------------
//素数計算
//--------------------------------------------------------------------------------

//----------------------------------------
//【ランタイム版】
//----------------------------------------

//----------------------------------------
//【ランタイム版】素数判定
bool isPrime(const unsigned int n)
{
	if (n < 2)//2未満は素数ではない
		return false;
	else if (n == 2)//2は素数
		return true;
	else if ((n & 1) == 0)//偶数は素数ではない
		return false;
	for (unsigned int div = 3; div <= n / div; div += 2)//div = 3～n/div の範囲で割り切れる値があるか判定
	{
		if (n % div == 0)//割り切れる値がみつかったら素数ではない
			return false;
	}
	return true;//素数と判定
}

//----------------------------------------
//【ランタイム版】指定の値より小さい最初の素数を算出
unsigned int makePrimeLT(const unsigned int n)
{
	if (n <= 2)//2より小さい素数はない
		return 0;
	else if (n == 3)//3の次に小さい素数は2
		return 2;
	for (unsigned int nn = n - ((n & 1) == 0 ? 1 : 2);; nn -= 2)//素数がみつかるまでループ ※偶数は判定しない
	{
		if (isPrime(nn))//素数判定
			return nn;
	}
	return 0;//dummy
}

//----------------------------------------
//【ランタイム版】指定の値より大きい最初の素数を算出
unsigned int makePrimeGT(const unsigned int n)
{
	if (n < 2)//2未満の値より大きい最初の素数は2
		return 2;
	for (unsigned int nn = n + ((n & 1) == 0 ? 1 : 2);; nn += 2)//素数がみつかるまでループ ※偶数は判定しない
	{
		if (isPrime(nn))//素数判定
			return nn;
	}
	return 0;//dummy
}

//--------------------------------------------------------------------------------
//ビット計算
//--------------------------------------------------------------------------------

//----------------------------------------
//【ランタイム版】ビット数を数える
//※独自実装版
int countBits_custom(const unsigned int value)
{
#if 0
	//最適化していない処理
	int bits = 0;
	for (int i = 0; i < 32; ++i)
	{
		if (value & (1 << i))
			++bits;
	}
	return bits;
#elif 0
	//少しだけ最適化した処理（シフト演算）
	unsigned int _value = value;
	int bits = 0;
	while (_value)
	{
		++bits;
		_value >>= 1;
	}
	return bits;
#elif 0
	//少しだけ最適化した処理（論理和）
	unsigned int _value = value;
	int bits = 0;
	while (_value)
	{
		++bits;
		_value &= (_value - 1);
	}
	return bits;
#elif 0
	//最適化した処理
	unsigned int bits = static_cast<unsigned int>(value);
	bits = ((bits >>  1) & 0x55555555) + (bits & 0x55555555);//1ビットずつ16個の加算
	bits = ((bits >>  2) & 0x33333333) + (bits & 0x33333333);//2ビットずつ8個の加算
	bits = ((bits >>  4) & 0x0f0f0f0f) + (bits & 0x0f0f0f0f);//4ビットずつ4個の加算
	bits = ((bits >>  8) & 0x00ff00ff) + (bits & 0x00ff00ff);//8ビットずつ2個の加算
	bits = ((bits >> 16) & 0x0000ffff) + (bits & 0x0000ffff);//最終加算
	return static_cast<int>(bits);
#else
	//更に最適化した処理
	//※乗算を使用しているが、実際に計測するとこちらの方が速い
	unsigned int bits = static_cast<unsigned int>(value);
	bits = bits - ((bits >> 1) & 0x55555555);//2ビットずつ16個の減算 ※1ビットずつ16個の加算と同じ結果になる（マスク回数削減）
	bits = ((bits >> 2) & 0x33333333) + (bits & 0x33333333);//2ビットずつ8個の加算 ※加算の繰り上がりが隣の値に及ぶのでマスクして計算
	bits = (((bits >> 4) + bits) & 0x0f0f0f0f);//4ビットずつ4個の加算 ※加算の繰り上がりが隣の値まで及ばないのでマスクなしで計算し、最後にマスク（マスク回数削減）
	bits *= 0x01010101;//8ビットずつ4個の値をまとめて加算
	                   //※上位から8ビットごとに [① ② ③ ④]とすると、乗算の結果、[[①+②+③+④] [②+③+④] [③+④] ④] という値ができ上がる
	bits >>= 24;//上位8ビットを取り出す
	return static_cast<int>(bits);
#endif
}
#ifdef ENABLE_BUILTIN_POPCNT
//※ビルトイン処理版
int countBits_builtin(const unsigned int value)
{
#ifdef GASHA_IS_VC
	return __popcnt(value);
#endif//GASHA_IS_VC
#ifdef GASHA_IS_GCC
	return __builtin_popcount(value);
#endif//GASHA_IS_GCC
}
#endif//ENABLE_BUILTIN_POPCNT
#ifdef ENABLE_SSE_POPCNT
//※SSE命令版
int countBits_sse(const unsigned int value)
{
	return  _mm_popcnt_u32(value);
}
#endif//ENABLE_SSE_POPCNT

//----------------------------------------
//【ランタイム版】MSB(Most Significant Bit)を算出
int calcMSB(const unsigned int value)
{
#if 0
	//最適化していない処理
	for (int i = 31; i >= 0; --i)
	{
		if (value & (1 << i))
			return i;
	}
	return -1;
#elif 0
	//計算量が多すぎて失敗した最適化
	unsigned int bits = static_cast<unsigned int>(value);
	bits |= (bits << 16);//下位16ビットのビット数を倍にする
	bits |= (bits <<  8);//下位24ビットのビット数を倍にする
	bits |= (bits <<  4);//下位28ビットのビット数を倍にする
	bits |= (bits <<  2);//下位30ビットのビット数を倍にする
	bits |= (bits <<  1);//下位31ビットのビット数を倍にする
	int bit = countBits2(static_cast<int>(bits));//ビット数を計測
	--bit;//-1～31の値に調整
	const int sign = bit >> 31;//符号（-1か0）を取得
	bit = 31 - bit;//ビット番号反転（0～32の値になる）
	bit |= sign;//符号を合成して -1～31の値に調整
	return bit;
#elif 0
	//最適化した処理（ビット数を半分ずつ判定しながら計上）
	int bit = 0;
	unsigned int _value = static_cast<unsigned int>(value);
	if (_value & 0xffff0000){ bit += 16; _value >>= 16; }//上位16ビット分のビット位置を計上
	if (_value & 0x0000ff00){ bit +=  8; _value >>=  8; }//上位24ビット分のビット位置を計上
	if (_value & 0x000000f0){ bit +=  4; _value >>=  4; }//上位28ビット分のビット位置を計上
	if (_value & 0x0000000c){ bit +=  2; _value >>=  2; }//上位30ビット分のビット位置を計上
	if (_value & 0x00000002){ ++bit;     _value >>=  1; }//上位31ビット分のビット位置を計上
	if (_value & 0x00000001){ ++bit;                    }//全ビット分のビット位置を計上
	--bit;//-1～31の値に調整
	return bit;
#elif 0
	//最適化した処理（ビット数を半分ずつ計上、判定文なし）
	int bit = 0;
	unsigned int _value = static_cast<unsigned int>(value);
	int shift;
	shift = ((_value & 0xffff0000) != 0) << 4; bit += shift; _value >>= shift;//上位16ビット分のビット位置を計上
	shift = ((_value & 0x0000ff00) != 0) << 3; bit += shift; _value >>= shift;//上位24ビット分のビット位置を計上
	shift = ((_value & 0x000000f0) != 0) << 2; bit += shift; _value >>= shift;//上位28ビット分のビット位置を計上
	shift = ((_value & 0x0000000c) != 0) << 1; bit += shift; _value >>= shift;//上位30ビット分のビット位置を計上
	shift = ((_value & 0x00000002) != 0) << 0; bit += shift; _value >>= shift;//上位31ビット分のビット位置を計上
	shift = ((_value & 0x00000001) != 0) << 0; bit += shift;                  //全ビット分のビット位置を計上
	--bit;//-1～31の値に調整
	return bit;
#else
	//ベタにビット数を半分ずつ判定
	//※結局これが最速
	if (value & 0xffff0000){
		if (value & 0xff000000){
			if (value & 0xf0000000){
				if (value & 0xc0000000){
					if (value & 0x80000000){
						return 31;
					}else{//if(value & 0x40000000){
						return 30;
					}
				}else{//if(value & 0x30000000){
					if (value & 0x20000000){
						return 29;
					}else{//if(value & 0x10000000){
						return 28;
					}
				}
			}else{//if(value & 0x0f000000){
				if (value & 0x0c000000){
					if (value & 0x08000000){
						return 27;
					}else{//if(value & 0x04000000){
						return 26;
					}
				}else{//if(value & 0x03000000){
					if (value & 0x02000000){
						return 25;
					}else{//if(value & 0x01000000){
						return 24;
					}
				}
			}
		}else{//if(value & 0x00ff0000){
			if (value & 0x00f00000){
				if (value & 0x00c00000){
					if (value & 0x00800000){
						return 23;
					}else{//if(value & 0x00400000){
						return 22;
					}
				}else{//if(value & 0x00300000){
					if (value & 0x00200000){
						return 21;
					}else{//if(value & 0x00100000){
						return 20;
					}
				}
			}else{//if(value & 0x000f0000){
				if (value & 0x000c0000){
					if (value & 0x00080000){
						return 19;
					}else{//if(value & 0x00040000){
						return 18;
					}
				}else{//if(value & 0x00030000){
					if (value & 0x00020000){
						return 17;
					}else{//if(value & 0x00010000){
						return 16;
					}
				}
			}
		}
	}else if(value & 0x0000ffff){
		if (value & 0x0000ff00){
			if (value & 0x0000f000){
				if (value & 0x0000c000){
					if (value & 0x00008000){
						return 15;
					}else{//if(value & 0x00004000){
						return 14;
					}
				}else{//if(value & 0x00003000){
					if (value & 0x00002000){
						return 13;
					}else{//if(value & 0x00001000){
						return 12;
					}
				}
			}else{//if(value & 0x00000f00){
				if (value & 0x00000c00){
					if (value & 0x00000800){
						return 11;
					}else{//if(value & 0x00000400){
						return 10;
					}
				}else{//if(value & 0x00000300){
					if (value & 0x00000200){
						return 9;
					}else{//if(value & 0x00000100){
						return 8;
					}
				}
			}
		}else{//if(value & 0x000000ff){
			if (value & 0x000000f0){
				if (value & 0x000000c0){
					if (value & 0x00000080){
						return 7;
					}else{//if(value & 0x00000040){
						return 6;
					}
				}else{//if(value & 0x00000030){
					if (value & 0x00000020){
						return 5;
					}else{//if(value & 0x00000010){
						return 4;
					}
				}
			}else{//if(value & 0x0000000f){
				if (value & 0x0000000c){
					if (value & 0x00000008){
						return 3;
					}else{//if(value & 0x00000004){
						return 2;
					}
				}else{//if(value & 0x00000003){
					if (value & 0x00000002){
						return 1;
					}else{//if(value & 0x00000001){
						return 0;
					}
				}
			}
		}
	}else{//if(value == 0){
		return -1;
	}
	return -1;
#endif
}

//----------------------------------------
//【ランタイム版】LSB(Least Significant Bit)を算出
int calcLSB(const unsigned int value)
{
#if 0
	//最適化していない処理
	for (int i = 0; i < 32; ++i)
	{
		if (value & (1 << i))
			return i;
	}
	return -1;
#elif 0
	//計算量が多すぎて失敗した最適化
	unsigned int bits = static_cast<unsigned int>(value);
	bits |= (bits << 16);//下位16ビットのビット数を倍にする
	bits |= (bits <<  8);//下位24ビットのビット数を倍にする
	bits |= (bits <<  4);//下位28ビットのビット数を倍にする
	bits |= (bits <<  2);//下位30ビットのビット数を倍にする
	bits |= (bits <<  1);//下位31ビットのビット数を倍にする
	int bit = countBits2(static_cast<int>(bits));//ビット数を計測
	--bit;//-1～31の値に調整
	const int sign = bit >> 31;//符号（-1か0）を取得
	bit = 31 - bit;//ビット番号反転（0～32の値になる）
	bit |= sign;//符号を合成して -1～31の値に調整
	return bit;
#elif 0
	//最適化した処理（ビット数を半分ずつ判定しながら計上）
	int bit = 0;
	unsigned int _value = static_cast<unsigned int>(value);
	if (_value & 0x0000ffff){ bit -= 16; _value <<= 16; }//下位16ビット分のビット位置を計上
	if (_value & 0x00ff0000){ bit -=  8; _value <<=  8; }//下位24ビット分のビット位置を計上
	if (_value & 0x0f000000){ bit -=  4; _value <<=  4; }//下位28ビット分のビット位置を計上
	if (_value & 0x30000000){ bit -=  2; _value <<=  2; }//下位30ビット分のビット位置を計上
	if (_value & 0x40000000){ --bit;     _value <<=  1; }//下位31ビット分のビット位置を計上
	if (_value & 0x80000000){ --bit;                    }//全ビット分のビット位置を計上
	int sign = (bit ^ 0x80000000);//符号ビットを取得
	sign >>= 31;//符号を-1か0に拡張
	bit += 32;//ビット番号を調整（0～32の値になる）
	bit |= sign;//符号を合成して -1～31の値に調整
	return bit;
#elif 0
	//最適化した処理（ビット数を半分ずつ計上、判定文なし）
	int bit = 0;
	unsigned int _value = static_cast<unsigned int>(value);
	if (_value & 0x0000ffff){ bit -= 16; _value <<= 16; }//下位16ビット分のビット位置を計上
	if (_value & 0x00ff0000){ bit -=  8; _value <<=  8; }//下位24ビット分のビット位置を計上
	if (_value & 0x0f000000){ bit -=  4; _value <<=  4; }//下位28ビット分のビット位置を計上
	if (_value & 0x30000000){ bit -=  2; _value <<=  2; }//下位30ビット分のビット位置を計上
	if (_value & 0x40000000){ --bit;     _value <<=  1; }//下位31ビット分のビット位置を計上
	if (_value & 0x80000000){ --bit;                    }//全ビット分のビット位置を計上
	int sign = (bit ^ 0x80000000);//符号ビットを取得
	sign >>= 31;//符号を-1か0に拡張
	bit += 32;//ビット番号を調整（0～32の値になる）
	bit |= sign;//符号を合成して -1～31の値に調整
	return bit;
#else
	//ベタにビット数を半分ずつ判定
	//※結局これが最速
	if (value & 0x0000ffff){
		if (value & 0x000000ff){
			if (value & 0x0000000f){
				if (value & 0x00000003){
					if (value & 0x00000001){
						return 0;
					}
					else{//if(value & 0x00000002){
						return 1;
					}
				}else{//if(value & 0x0000000c){
					if (value & 0x00000004){
						return 2;
					}else{//if(value & 0x00000008){
						return 3;
					}
				}
			}else{//if(value & 0x000000f0){
				if (value & 0x00000030){
					if (value & 0x00000010){
						return 4;
					}else{//if(value & 0x00000020){
						return 5;
					}
				}else{//if(value & 0x000000c0){
					if (value & 0x00000040){
						return 6;
					}else{//if(value & 0x00000080){
						return 7;
					}
				}
			}
		}else{//if(value & 0x0000ff00){
			if (value & 0x00000f00){
				if (value & 0x00000300){
					if (value & 0x00000100){
						return 8;
					}else{//if(value & 0x00000200){
						return 9;
					}
				}else{//if(value & 0x00000c00){
					if (value & 0x00000400){
						return 10;
					}else{//if(value & 0x00000800){
						return 11;
					}
				}
			}else{//if(value & 0x0000f000){
				if (value & 0x00003000){
					if (value & 0x00001000){
						return 12;
					}else{//if(value & 0x00002000){
						return 13;
					}
				}else{//if(value & 0x0000c000){
					if (value & 0x00004000){
						return 14;
					}else{//if(value & 0x00008000){
						return 15;
					}
				}
			}
		}
	}else if(value & 0xffff0000){
		if (value & 0x00ff0000){
			if (value & 0x000f0000){
				if (value & 0x00030000){
					if (value & 0x00010000){
						return 16;
					}else{//if(value & 0x00020000){
						return 17;
					}
				}else{//if(value & 0x000c0000){
					if (value & 0x00040000){
						return 18;
					}else{//if(value & 0x00080000){
						return 19;
					}
				}
			}else{//if(value & 0x00f00000){
				if (value & 0x00300000){
					if (value & 0x00100000){
						return 20;
					}else{//if(value & 0x00200000){
						return 21;
					}
				}else{//if(value & 0x00c00000){
					if (value & 0x00400000){
						return 22;
					}else{//if(value & 0x00800000){
						return 23;
					}
				}
			}
		}else{//if(value & 0xff000000){
			if (value & 0x0f000000){
				if (value & 0x03000000){
					if (value & 0x01000000){
						return 24;
					}else{//if(value & 0x02000000){
						return 25;
					}
				}else{//if(value & 0x0c000000){
					if (value & 0x04000000){
						return 26;
					}else{//if(value & 0x08000000){
						return 27;
					}
				}
			}else{//if(value & 0xf0000000){
				if (value & 0x30000000){
					if (value & 0x10000000){
						return 28;
					}else{//if(value & 0x20000000){
						return 29;
					}
				}else{//if(value & 0xc0000000){
					if (value & 0x40000000){
						return 30;
					}else{//if(value & 0x80000000)[
						return 31;
					}
				}
			}
		}
	//}else{//if (value == 0x0){
	//	return -1;
	}
	return -1;
#endif
}

GASHA_NAMESPACE_END;//ネームスペース：終了

// End of file
