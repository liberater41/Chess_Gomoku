#include <iostream>
#include <graphics.h>
#include <conio.h>
#include <Windows.h>
#include <gdiplus.h>
#include <cmath>
#include <vector>
#include <unordered_set>
#include "chess.h"

using namespace std;

const double GRID_SIZE_X = 42; // 网格大小
const double GRID_SIZE_Y = 39.35;
const int WINDOW_WIDTH = 640; // 窗口宽度
const int WINDOW_HEIGHT = 640; // 窗口高度

vector<vector<int>> board(BoardSIZE,vector<int>(BoardSIZE,-1));
unordered_set<pair<int, int>,pair_hash> priority;



void Drawpng(const wchar_t* filename, int x, int y) {
    Gdiplus::Image image(filename);

    int width = getwidth();
    int height = getheight();

    // 获取 EasyX 画布的 HDC
    HDC hdc = GetImageHDC();

    // 创建 GDI+ 的 Graphics 对象，绑定到 EasyX 画布
    Gdiplus::Graphics graphics(hdc);

    // 绘制 PNG 图片到指定位置
    graphics.DrawImage(&image, x, y, width, height);
}

// 初始化 GDI+
void InitGDIPlus(ULONG_PTR* gdiplusToken) {
    Gdiplus::GdiplusStartupInput gdiplusStartupInput;
    Gdiplus::GdiplusStartup(gdiplusToken, &gdiplusStartupInput, NULL);
}

// 关闭 GDI+
void ShutdownGDIPlus(ULONG_PTR gdiplusToken) {
    Gdiplus::GdiplusShutdown(gdiplusToken);
}

void GetNearestIntersection(int mouseX, int mouseY, int& intersectX, int& intersectY) {
    intersectX = 25+round((mouseX-25) / GRID_SIZE_X) * GRID_SIZE_X; // 最近的横坐标
    intersectY = 25 + round((mouseY - 25) / GRID_SIZE_Y) * GRID_SIZE_Y; // 最近的纵坐标
}

void GetIntersection(int x, int y, int& intersectX, int& intersectY) {
    intersectX = 25 + x * GRID_SIZE_X;
    intersectY = 25 + y * GRID_SIZE_Y;
}

void Getcoordinate(int& x, int& y, int intersectX, int intersectY) {
    x = round((intersectX - 25) / GRID_SIZE_X);
    y = round((intersectY - 25) / GRID_SIZE_Y);
}

void Drawchess(int X, int Y, bool color) {
    if (color == 0) { setfillcolor(BLACK); }
    else {setfillcolor(WHITE);}
    
    fillcircle(X, Y, 18);
}



int Count_and_block_to_score(int count, int block) {
    if (count >= 4)return FIVE;
    if (block == 2)return 0;
    vector<int> oneblock = { BLOCKED_ONE,BLOCKED_TWO,BLOCKED_THREE,BLOCKED_FOUR };
    vector<int> noblock = { ONE,TWO,THREE,FOUR};
    if (block == 1)return oneblock[count];
    if (block == 0)return noblock[count];
    
}

int Get_count_and_block_to_score(vector<vector<int>>& board,int x,int y,int dx,int dy,int color){
    int count = 0, block = 0;
    int opponent = color == 0 ? 1 : 0;
    for (int i = 1;; i++) {
        int nx = x + i * dx;
        int ny = y + i * dy;
        if (nx >= 0 && nx < BoardSIZE && ny >= 0 && ny < BoardSIZE) {
            if (board[nx][ny] == color)count++;
            else if (board[nx][ny] == opponent) {
                block++;
                break;
            }
            else {
                break;
            }
        }
        else {
            block++;
            break;
        }
    }
    for (int i = 1;; i++) {
        int nx = x - i * dx;
        int ny = y - i * dy;
        if (nx >= 0 && nx < BoardSIZE && ny >= 0 && ny < BoardSIZE) {
            if (board[nx][ny] == color)count++;
            else if (board[nx][ny] == opponent) {
                block++;
                break;
            }
            else {
                break;
            }
        }
        else {
            block++;
            break;
        }
    }
    return Count_and_block_to_score(count, block);
}

int allline_score(vector<vector<int>>& board, int x, int y, int color) {
    int score = 0;
    score += Get_count_and_block_to_score(board, x, y, 0, 1, color);
    score += Get_count_and_block_to_score(board, x, y, 1, 0, color);
    score += Get_count_and_block_to_score(board, x, y, 1, 1, color);
    score += Get_count_and_block_to_score(board, x, y, 1, -1, color);
    return score;
}

int best_x, best_y,best_score;

bool checkwin(vector<vector<int>>& board,int x,int y,int color){
    if (allline_score(board, x, y, color) >= FIVE)return true;
    return false;
}

int Botrun1(vector<vector<int>>& board, int color,int depth,unordered_set<pair<int,int>,pair_hash>& prio) {
    if (depth == 0)return 0;
    int tempscore=-FIVE,tempx=-1,tempy=-1;
    for (pair<int, int> el : prio) {
        int x = el.first, y = el.second;
        int score=0;
        int opponent = color == 0 ? 1 : 0;
        unordered_set<pair<int, int>,pair_hash> deepset=prio;
        for(int i=x-1;i<=x+1;i++)
            for (int j = y - 1; j <= y + 1; j++) {
                if (i >= 0 && i < BoardSIZE && j >= 0 && j < BoardSIZE&&board[i][j]==-1) {
                    deepset.insert({i,j});
                }
        }
        deepset.erase({x,y});
        score += allline_score(board, x, y, color);
        if (score == FIVE)return FIVE;
        board[x][y] = color;
        score -= Botrun1(board, opponent, depth - 1, deepset);
        board[x][y] = -1;
        if (depth == DEPTH) {
            cout << "(" << x << "," << y << ")" << " " << score << "  ";

        }
        if (score > tempscore) {
            tempscore = score;
            tempx = x;
            tempy = y;
        }
       
    }
    if (depth == DEPTH) {
        best_score = tempscore;
        best_x = tempx;
        best_y = tempy;
    }
    return tempscore;

}

int Botrun(int color, int depth) {
    if (depth == 0)return 0;
    int xy, x1, y1, score = -FIVE, best = -FIVE, best_score;
    int opponent = color == 0 ? 1 : 0;
    for (xy = 0; xy < BoardSIZE * BoardSIZE; xy++) {
        x1 = xy % BoardSIZE;
        y1 = xy / BoardSIZE;
        if (board[x1][y1] != -1)continue;
        int sign = 0;
        for (int i = x1 - 1; i <= x1 + 1; i++) {
            for (int j = y1 - 1; j <= y1 + 1; j++) {
                if (i >= 0 && i < BoardSIZE && j >= 0 && j < BoardSIZE && board[i][j] != -1) {
                    score = allline_score(board, x1, y1, color);
                    if (score == FIVE)return score;

                    board[x1][y1] = color;
                    score -= Botrun(opponent, depth - 1);
                    board[x1][y1] = -1;
                    if(depth==DEPTH)cout << "(" << x1 << "," << y1 << ")" << " " << score << "  ";
                    
                    sign = 1;
                    break;
                }
            }
            if (sign)break;
        }

        if (score > best) {
            best = score;
            if (depth == DEPTH) {
                best_x = x1;
                best_y = y1;
            }
        }

    }
    return best;
}

int Bot(vector<vector<int>>& board,int color,int& endsign){
    int intersectX, intersectY;
    best_x = -1;
    best_y = -1;
    best_score = -FIVE;
    //Botrun(board, color,DEPTH,priority);
    Botrun(color, DEPTH);
    cout << best_x << " " << best_y;
    board[best_x][best_y] = color;
    for (int i = best_x - 1; i <= best_x + 1; i++)
        for (int j = best_y - 1; j <= best_y + 1; j++) {
            if (i >= 0 && i < BoardSIZE && j >= 0 && j < BoardSIZE && board[i][j] == -1) {
                priority.insert({ i,j });
            }
        }
    priority.erase({ best_x,best_y });
    GetIntersection(best_x, best_y, intersectX, intersectY);
    Drawchess(intersectX, intersectY, color);
    if (checkwin(board, best_x, best_y, color)) {
        cout << "bot win\n";
        settextstyle(40, 0, _T("Arial"));
        outtextxy(280, 600, _T("电脑获胜"));
        endsign = 1;
        return 1;
    }
    return 0;

}

void Player_bot(vector<vector<int>>& board,int order ){
    for (int i = 0; i < board.size(); ++i) {
        for (int j = 0; j < board[i].size(); ++j) {
            board[i][j] = -1;
        }
    }

    int endsign = 0;
    int color = order;
    int opponent = color == 0 ? 1 : 0;
    if (order == 1) {
        int x = BoardSIZE / 2, y = BoardSIZE / 2, intersectX, intersectY;
        board[x][y] = opponent;
        GetIntersection(x, y, intersectX, intersectY);
        Drawchess(intersectX, intersectY, 0);
    }

    while (true) {
        int sign = 0;
        MOUSEMSG msg = GetMouseMsg(); 
        if (msg.uMsg == WM_LBUTTONDOWN) { 
            if (msg.x > 543 && msg.x < 606 && msg.y>593 && msg.y < 625) {
                cout << "back";
                
                break;
            }

            int intersectX, intersectY,x,y;
            if (msg.x < 15 || msg.x>622 || msg.y < 15 || msg.y>590)continue;
            GetNearestIntersection(msg.x, msg.y, intersectX, intersectY); // 计算交叉点  
            Getcoordinate(x, y, intersectX, intersectY);
            if (board[x][y] != -1)continue;
            cout << x << " " << y <<" " << intersectX << " " << intersectY << "  ";
            board[x][y] = color;
            for (int i = x - 1; i <= x + 1; i++)
                for (int j = y - 1; j <= y + 1; j++) {
                    if (i >= 0 && i < BoardSIZE && j >= 0 && j < BoardSIZE && board[i][j] == -1) {
                        priority.insert({ i,j });
                    }
                }
            priority.erase({ x,y });
            Drawchess(intersectX, intersectY, color);
            if (checkwin(board, x, y, order)) {
                cout << "player" << order << " win\n";
                settextstyle(40, 0, _T("Arial"));
                outtextxy(280, 600, _T("你获胜"));
                endsign = 1;
                break;
            }
            sign = 1;
        }


        if (sign) {
            if (Bot(board, opponent,endsign))break;
            sign = 0;
        }
        

    }
    if (endsign) {
        while (1) {
            MOUSEMSG msg = GetMouseMsg();
            if (msg.uMsg == WM_LBUTTONDOWN) {
                if (msg.x > 543 && msg.x < 606 && msg.y>593 && msg.y < 625) {
                    cout << "back\n";
                    break;
                }
            }
        }
    }
}

void Player_to_player(vector<vector<int>>& board) {
    for (int i = 0; i < board.size(); ++i) {
        for (int j = 0; j < board[i].size(); ++j) {
            board[i][j] = -1;
        }
    }
    
    int order = 0,endsign=0;
    while (1) {
        MOUSEMSG msg = GetMouseMsg();
        if (msg.uMsg == WM_LBUTTONDOWN) {
            if (msg.x > 543 && msg.x < 606 && msg.y>593 && msg.y < 625) {
                cout << "back\n";                
                break;
            }

            int intersectX, intersectY,x,y;
            if (msg.x < 15 || msg.x>622 || msg.y < 15 || msg.y>590)continue;
            GetNearestIntersection(msg.x, msg.y, intersectX, intersectY); // 计算交叉点                                
            Getcoordinate(x, y, intersectX, intersectY);
            if (board[x][y] != -1)continue;
            board[x][y] = order;
            Drawchess(intersectX, intersectY, (order == 0) ? 0 : 1);
            if (checkwin(board, x, y, order)) {
                cout << "player" << order << " win\n";
                settextstyle(40, 0, _T("Arial"));
                outtextxy(280, 600, (order == 0 ? _T("黑子获胜") : _T("白子获胜")));
                endsign = 1;
                break;
            }
            order = ((order == 0) ? 1 : 0);
        }

    }
    if (endsign) {
        while (1) {
            MOUSEMSG msg = GetMouseMsg();
            if (msg.uMsg == WM_LBUTTONDOWN) {
                if (msg.x > 543 && msg.x < 606 && msg.y>593 && msg.y < 625) {
                    cout << "back\n";
                    break;
                }
            }
        }
    }

}

int main() {
    ULONG_PTR gdiplusToken;
    InitGDIPlus(&gdiplusToken);  
    while (1) {
        initgraph(WINDOW_HEIGHT, WINDOW_WIDTH);
        Drawpng(L"MainMenu.png", 0, 0);
        int mode;
        while (1) {
            MOUSEMSG msg = GetMouseMsg(); 
            if (msg.uMsg == WM_LBUTTONDOWN) { 
                if (msg.x > 217 && msg.x < 426 && msg.y>382 && msg.y < 434) {
                    closegraph();
                    initgraph(WINDOW_HEIGHT, WINDOW_WIDTH);
                    Drawpng(L"ChessBoard.png", 0, 0);
                    int order = 0;
                    Player_bot(board, order);
                    closegraph();
                    break;
                }
                if (msg.x > 217 && msg.x < 426 && msg.y>444 && msg.y < 494) {
                    closegraph();
                    initgraph(WINDOW_HEIGHT, WINDOW_WIDTH);
                    Drawpng(L"ChessBoard.png", 0, 0);
                    Player_to_player(board);
                    closegraph();
                    break;

                }
            }                
        }       
    }
    
    
    

	_getch();
	closegraph();
    ShutdownGDIPlus(gdiplusToken);
	return 0;
}

