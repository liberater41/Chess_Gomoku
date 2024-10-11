#pragma once

#include <functional>

#define BoardSIZE 15
#define DEPTH 5

using namespace std;
enum
{
	ONE = 10,
	TWO = 100,
	THREE = 1000,
	FOUR = 100000,
	FIVE = 1000000,
	BLOCKED_ONE = 5,
	BLOCKED_TWO = 50,
	BLOCKED_THREE = 500,
	BLOCKED_FOUR = 50000,
};

struct pair_hash {
	template <class T1, class T2>
	size_t operator() (const pair<T1, T2>& pair) const {
		// 结合两个整数的哈希值
		return hash<T1>()(pair.first) ^ hash<T2>()(pair.second);
	}
};
