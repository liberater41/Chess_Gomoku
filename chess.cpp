#include <iostream>
#include <graphics.h>
#include <conio.h>
#include <Windows.h>
#include <gdiplus.h>
#include <cmath>
#include <vector>
#include <unordered_set>
#include "chess.h"
#include <chrono>
#include <memory>
#include <algorithm>
#include <ctime> 

using namespace std;
using namespace std::chrono;

const double GRID_SIZE_X = 42; // 网格大小
const double GRID_SIZE_Y = 39.35;
const int WINDOW_WIDTH = 640; // 窗口宽度
const int WINDOW_HEIGHT = 640; // 窗口高度

vector<vector<int>> board(BoardSIZE,vector<int>(BoardSIZE,-1));
unordered_set<pair<int, int>,pair_hash> priority;

duration<double, std::milli> totaltime(0);


int Drawpng(const wchar_t* filename, int x, int y) {
    Gdiplus::Image image(filename);
    if (image.GetLastStatus() != Gdiplus::Ok) {
        return 0;
    }
    int width = getwidth();
    int height = getheight();

    // 获取 EasyX 画布的 HDC
    HDC hdc = GetImageHDC();

    // 创建 GDI+ 的 Graphics 对象，绑定到 EasyX 画布
    Gdiplus::Graphics graphics(hdc);

    // 绘制 PNG 图片到指定位置
    graphics.DrawImage(&image, x, y, width, height);

    return 1;
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


int Five_pieces_score(int x, int y, int dx, int dy, int color) {
    int count = 0;
    int opp_count = 0;
    int opponent = color == 0 ? 1 : 0;
    for (int i = 0; i < 5; i++) {
        if (board[x + i * dx][y + i * dy] == color)count++;
        else if (board[x + i * dx][y + i * dy] == opponent)opp_count++;

    }
    if (count == 0 && opp_count == 0) return 0;
    else if (count != 0 && opp_count != 0) return 0;       
    else if (count > 0)return noblock[count - 1];
    else return opp_noblock[opp_count - 1];
}

bool isblockFour(int x, int y, int color) {
    const int directions[4][2] = { {0, 1}, {1, 0}, {1, 1}, {1, -1} };
    for (int d = 0; d < 4; ++d) {
        int count = 0;
        int dx = directions[d][0];
        int dy = directions[d][1];

        for (int i = -4; i <= 4; ++i) {
            int nx = x + i * dx;
            int ny = y + i * dy;

            if (nx >= 0 && nx < BoardSIZE && ny >= 0 && ny < BoardSIZE) {
                if (board[nx][ny] == color) {
                    ++count;
                }
                else {
                    count = 0;
                }

                if (count == 4) {
                    int preX = x + (i - 4) * dx;
                    int preY = y + (i - 4) * dy;
                    int postX = x + (i + 1) * dx;
                    int postY = y + (i + 1) * dy;

                    bool preEmpty = (preX >= 0 && preX < BoardSIZE && preY >= 0 && preY < BoardSIZE && board[preX][preY] == -1);
                    bool postEmpty = (postX >= 0 && postX < BoardSIZE && postY >= 0 && postY < BoardSIZE && board[postX][postY] == -1);

                    if (preEmpty || postEmpty) {
                        
                        return true;
                    }
                }
            }
        }
    }

    return false;
}

int allline_score(vector<vector<int>>& board, int color) {
    int score = 0;
    for (int i = 0; i < BoardSIZE; i++) {
        for (int j = 0; j < BoardSIZE; j++) {
            if (j + 4 < BoardSIZE) {
                score += Five_pieces_score(j, i, 1, 0, color);
            }
            if (i + 4 < BoardSIZE && j + 4 < BoardSIZE) {
                score+= Five_pieces_score(j, i, 1, 1, color);
            }
            if (i + 4 < BoardSIZE) {
                score += Five_pieces_score(j, i, 0, 1, color);
            }
            if (i + 4 <BoardSIZE && j - 4 >=0) {
                score += Five_pieces_score(j, i, -1, 1, color);
            }
        }
    }
    
    return score;
}

int best_x, best_y,best_score;

bool checkwin(vector<vector<int>>& board,int color){
    for (int i = 0; i < BoardSIZE; i++) {
        for (int j = 0; j < BoardSIZE; j++) {
            if (j + 4 < BoardSIZE&&Five_pieces_score(j, i, 1, 0, color)==FIVE) {
                return true;
            }
            if (i + 4 < BoardSIZE && j + 4 < BoardSIZE&&Five_pieces_score(j, i, 1, 1, color)==FIVE) {
                return true;;
            }
            if (i + 4 < BoardSIZE&&Five_pieces_score(j, i, 0, 1, color)==FIVE) {
                return true;;
            }
            if (i + 4 <BoardSIZE && j - 4 >=0&& Five_pieces_score(j, i, -1, 1, color)==FIVE) {
                return true;;
            }
        }
    }
    return false;
}



class Node {
public:   
    Node* father;
    int score;
    bool isMAX;
    int a;
    int b;
    pair<int,int> loc;
    int sign = 0;
    bool cut=0;
    int four=0;
    Node(Node* father, int score,bool ismax,int a,int b) {
        this->father = father;
        this->score = score;
        this->isMAX = ismax;
        this->a = a;
        this->b = b;
    }
};
vector<int> score_for_odd;
double get_odd(int score) {
    score_for_odd.emplace_back(score);
    sort(score_for_odd.begin(), score_for_odd.end(),greater<int>());
    int s = 0,d=score_for_odd[0] - odd_border;
    for (auto el : score_for_odd) {
        if (el - d <= 0)continue;
        else {
            s += el - d;
        }
    }
    return double(score - d) / s;
}
void change_father_node(Node* father, int score,Node* node) {
    if (father == nullptr)return;
    if (father->father == nullptr) {
        cout << node->loc.second << " " << node->loc.first << "  " << node->score << "   ";
        if (node->cut)cout << "cut" << endl;
        if (score - father->score < -odd_border)return;
        if (score - father->score > odd_border) {
            score_for_odd.emplace_back(score);
            father->score = score;
            father->a = score > father->a ? score : father->a;
            best_x = node->loc.second;
            best_y = node->loc.first;
        }
        else {
            double odd = get_odd(score);
            
            int t = rand() % 1000;
            cout <<"t:"<<t<<" " << "odd:" << odd << endl;
            if (t < odd * 1000) {
                father->score = score;
                father->a = score > father->a ? score : father->a;
                best_x = node->loc.second;
                best_y = node->loc.first;
            }
            else return;
                
        }

    }
    if (father->isMAX == 1 && (score > father->score )) {        
        father->score = score;
        father->a = score>father->a?score: father->a;        
    }
    else if (father->isMAX == 0 && score < father->score) {
        father->score = score;
        father->b = score<father->b?score:father->b;
    }
}

bool is_alpha_beta_cut(Node* node) {
    if (node->father == nullptr)return false;
    if (node->father->father == nullptr) {
        //cout <<" " << node->a << " " << node->b << " ";
        if (node->b < node->a-THREE/2)return true;
        else return false;
    }
    else {
        if (node->isMAX == 1 && node->a > node->b + THREE / 2)return true;
        else if (node->isMAX == 0 && node->b < node->a - THREE / 2)return true;
        else return false;
    }

}

struct pri_point {
    int i0;
    int j0;
    int pri_score;
};

struct Compare {
    bool operator()(const pri_point& a, const pri_point& b) {
        return a.pri_score > b.pri_score;
    }
};

vector<pri_point> create_pri(vector<vector<int>>& board, int color) {
    vector<pri_point> pri;
    for (int i = 0; i < BoardSIZE; i++) {
        for (int j = 0; j < BoardSIZE; j++) {
            if (board[i][j] != -1) {
                continue;
            }
            bool sign = 0;
            for (int i_ = (0 > i - RADIUS ? 0 : i - RADIUS); i_ <= (BoardSIZE - 1 < i + RADIUS ? BoardSIZE - 1 : i + RADIUS); i_++) {
                for (int j_ = (0 > j - RADIUS ? 0 : j - RADIUS); j_ <= (BoardSIZE - 1 < j + RADIUS ? BoardSIZE - 1 : j + RADIUS); j_++) {
                    if (board[i_][j_] != -1) {
                        pri_point temp;
                        temp.i0 = i;
                        temp.j0 = j;
                        board[i][j] = color;
                        temp.pri_score = allline_score(board, color);
                        board[i][j] = -1;
                        pri.emplace_back(temp);
                        sign = 1;
                        break;
                    }
                }
                if (sign)break;
            }
        }
    }
    sort(pri.begin(), pri.end(), Compare());
    return pri;
}

void Botrun(int color, int depth, unique_ptr<Node>& node) {
    bool child_ismax = node->isMAX == 0 ? 1 : 0;
    int opponent = color == 0 ? 1 : 0;
    if (depth == DEPTH+2*node.get()->four) {
        int score=allline_score(board, depth%2==0?color:opponent);
        node->score = score;
        if (node->isMAX == 1)node->a = score>node->a?score:node->a;
        else node->b = score<node->b?score:node->b;
        change_father_node(node->father, score, node.get());
        return;
    }
    else if (checkwin(board, opponent)) {
        int score = allline_score(board, depth % 2 == 0 ? color : opponent);
        node->score = score-depth*THREE*2;
        if (node->isMAX == 1)node->a = score > node->a ? score : node->a;
        else node->b = score < node->b ? score : node->b;
        if(depth==1)change_father_node(node->father, 2*FIVE, node.get());
        else change_father_node(node->father, score, node.get());
        return;

    }    
    vector<pri_point> pri;
    pri = create_pri(board, color);
    if (pri.empty()) {
        int score = 0;
        node->score = score - depth * THREE * 2;
        if (node->isMAX == 1)node->a = score > node->a ? score : node->a;
        else node->b = score < node->b ? score : node->b;
        if (depth == 1)change_father_node(node->father, 2 * FIVE, node.get());
        else change_father_node(node->father, score, node.get());
        return;
    }
    for(auto el:pri)
    {
        int i = el.i0;
        int j = el.j0;
        if (is_alpha_beta_cut(node.get())) {
            node.get()->cut = 1;
            break;
        }            
        board[i][j] = color;
        unique_ptr<Node> childnode = make_unique<Node>(node.get(), (child_ismax == 1 ? -2*FIVE : 2*FIVE), child_ismax, node->a, node->b);
        childnode->loc = { i,j };
        if (depth==0&&isblockFour(i, j, color)) {
            if (depth == 0)cout << "four: " ;
            childnode.get()->four = node.get()->four + 1;
        }
        else {
            childnode.get()->four = node.get()->four;
        }
        Botrun(opponent, depth + 1, childnode);
        board[i][j] = -1;
       
    }

    change_father_node(node->father, node->score, node.get());
        
}

int Bot(vector<vector<int>>& board,int color,int& endsign){
    int intersectX, intersectY;
    best_x = -1;
    best_y = -1;
    best_score = -FIVE;
    //Botrun(board, color,DEPTH,priority);
    unique_ptr<Node> root = make_unique<Node>(nullptr,-2*FIVE,1,-FIVE*2,FIVE*2);
    score_for_odd.clear();
    Botrun(color, 0,root);
    cout << endl << best_x << " " << best_y<<endl;
    board[best_y][best_x] = color;

    GetIntersection(best_x, best_y, intersectX, intersectY);
    Drawchess(intersectX, intersectY, color);
    if (checkwin(board, color)) {
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
            //cout << msg.x << " " << msg.y << "  ";
            if (msg.x > 543 && msg.x < 606 && msg.y>593 && msg.y < 625) {
                cout << "back";
                
                break;
            }

            int intersectX, intersectY,x,y;
            if (msg.x < 15 || msg.x>622 || msg.y < 15 || msg.y>590)continue;
            GetNearestIntersection(msg.x, msg.y, intersectX, intersectY); // 计算交叉点  
            Getcoordinate(x, y, intersectX, intersectY);
            if (board[y][x] != -1)continue;
            
            board[y][x] = color;            
            
            Drawchess(intersectX, intersectY, color);

            if (checkwin(board, order)) {
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
            if (checkwin(board,order)) {
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
    srand(static_cast<unsigned int>(time(0)));
    while (1) {
        initgraph(WINDOW_HEIGHT, WINDOW_WIDTH);
        if (!Drawpng(L"MainMenu.png", 0, 0)) {
            cout << endl << "fail load image";
            return 0;
        }
        int mode;
        while (1) {
            MOUSEMSG msg = GetMouseMsg(); 
            if (msg.uMsg == WM_LBUTTONDOWN) { 
                if (msg.x > 217 && msg.x < 426 && msg.y>382 && msg.y < 434) {
                    closegraph();
                    initgraph(WINDOW_HEIGHT, WINDOW_WIDTH);
                    if (!Drawpng(L"ChessBoard.png", 0, 0)) {
                        cout << endl << "fail load image";
                        return 0;
                    }
                    settextstyle(40, 0, _T("Arial"));
                    outtextxy(280, 250, _T("先手"));
                    outtextxy(280, 300, _T("后手"));
                    int order = 0;
                    while (1) {
                        MOUSEMSG msg = GetMouseMsg();
                        if (msg.uMsg == WM_LBUTTONDOWN) {
                            if (msg.x > 280 && msg.x < 350 && msg.y>251 && msg.y < 290) {
                                order = 0;
                                break;
                            }
                            else if (msg.x > 280 && msg.x < 350 && msg.y>298 && msg.y < 337) {
                                order = 1;
                                break;
                            }
                        }

                    }
                    if (!Drawpng(L"ChessBoard.png", 0, 0)) {
                        cout << endl << "fail load image";
                        return 0;
                    }
                    Player_bot(board, order);
                    closegraph();
                    break;
                }
                if (msg.x > 217 && msg.x < 426 && msg.y>444 && msg.y < 494) {
                    closegraph();
                    initgraph(WINDOW_HEIGHT, WINDOW_WIDTH);
                    if (!Drawpng(L"ChessBoard.png", 0, 0)) {
                        cout << endl << "fail load image";
                        return 0;
                    }
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

