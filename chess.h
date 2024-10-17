#pragma once

#include <functional>
#include <vector>
#define BoardSIZE 15
#define DEPTH 5
#define RADIUS 1

using namespace std;
enum
{
	ONE = 1,
	TWO = 30,
	THREE = 100,
	FOUR = 10000,
	FIVE = 100000,
	OPP_ONE = -1,
	OPP_TWO = -30,
	OPP_THREE = -100,
	OPP_FOUR = -10000,
	OPP_FIVE = -100000,
};

struct pair_hash {
	template <class T1, class T2>
	size_t operator() (const pair<T1, T2>& pair) const {
		// 结合两个整数的哈希值
		return hash<T1>()(pair.first) ^ hash<T2>()(pair.second);
	}
};

vector<int> noblock = { ONE,TWO,THREE,FOUR,FIVE };
vector<int> opp_noblock = { OPP_ONE,OPP_TWO,OPP_THREE,OPP_FOUR,OPP_FIVE };


//int get_score(int x, int y, char color)
//{
//    auto start = high_resolution_clock::now();
//    int count = 0, block = 0, i, j, s = 0;
//    int opponent = color == 0 ? 1 : 0;
//
//    count = 0; block = 0;
//    for (i = y + 1;; i++)
//    {
//        if (i < BoardSIZE && board[x][i] == color)
//        {
//            count++;
//        }
//        else if (i < BoardSIZE && board[x][i] == opponent)
//        {
//            block++;
//            break;
//        }
//        else
//            break;
//    }
//    for (i = y - 1;; i--)
//    {
//        if (board[x][i] == color && i >= 0)
//        {
//            count++;
//        }
//        else if (board[x][i] == opponent && i >= 0)
//        {
//            block++;
//            break;
//        }
//        else
//            break;
//    }
//    s += Count_and_block_to_score(count, block);//横向
//
//    count = 0; block = 0;
//    for (i = x + 1;; i++)
//    {
//        if (board[i][y] == color && i < BoardSIZE)
//        {
//            count++;
//        }
//        else if (board[i][y] == opponent && i < BoardSIZE)
//        {
//            block++;
//            break;
//        }
//        else
//            break;
//    }
//    for (i = x - 1;; i--)
//    {
//        if (board[i][y] == color && i >= 0)
//        {
//            count++;
//        }
//        else if (board[i][y] == opponent && i >= 0)
//        {
//            block++;
//            break;
//        }
//        else
//            break;
//    }
//    s += Count_and_block_to_score(count, block);//纵向
//
//    count = 0; block = 0;
//    for (i = x + 1, j = y + 1;; i++, j++)
//    {
//        if (board[i][j] == color && i < BoardSIZE && j < BoardSIZE)
//        {
//            count++;
//        }
//        else if (board[i][j] == opponent && i < BoardSIZE && j < BoardSIZE)
//        {
//            block++;
//            break;
//        }
//        else
//            break;
//    }
//    for (i = x - 1, j = y - 1;; i--, j--)
//    {
//        if (board[i][j] == color && i >= 0 && j >= 0)
//        {
//            count++;
//        }
//        else if (board[i][j] == opponent && i >= 0 && j >= 0)
//        {
//            block++;
//            break;
//        }
//        else
//            break;
//    }
//    s += Count_and_block_to_score(count, block);//捺向
//
//    count = 0; block = 0;
//    for (i = x + 1, j = y - 1;; i++, j--)
//    {
//        if (board[i][j] == color && i < BoardSIZE && j >= 0)
//        {
//            count++;
//        }
//        else if (board[i][j] == opponent && i < BoardSIZE && j >= 0)
//        {
//            block++;
//            break;
//        }
//        else
//            break;
//    }
//    for (i = x - 1, j = y + 1;; i--, j++)
//    {
//        if (board[i][j] == color && i >= 0 && j < BoardSIZE)
//        {
//            count++;
//        }
//        else if (board[i][j] == opponent && i >= 0 && j < BoardSIZE)
//        {
//            block++;
//            break;
//        }
//        else
//            break;
//    }
//    s += Count_and_block_to_score(count, block);//撇向      
//    auto end = high_resolution_clock::now();
//    totaltime += end - start;
//    return s;
//
//}