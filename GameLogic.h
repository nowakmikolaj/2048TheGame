#pragma once
#pragma comment(lib, "Msimg32.lib")
#include "framework.h"
#include "resource.h"
#include <wingdi.h>
#include <map>
#include <tuple>
#include <fstream>
#include <iostream>
using namespace std;

#define MAX_LOADSTRING 100
#define N 4
#define CHLD_SIZE 60            // Tile size
#define PAUSE_SIZE 10           


enum        result { lose = -1, during, win };
enum        verticalMoves { Down = -1, NopeV, Up };
enum        horizontalMoves { Left = -1, NopeH, Right };
enum        ColorID { tile, mainWindow, winner = 5, loser };

// Global Variables:
HINSTANCE   hInst;                                   // current instance
WCHAR       szTitle[MAX_LOADSTRING];                 // The title bar text
WCHAR       szWindowClass[MAX_LOADSTRING];           // the main window class name
WCHAR       szWindowClassSmall[MAX_LOADSTRING];      
HWND        smallWindowsMain[N * N + 1];
HWND        smallWindowsCopy[N * N + 1];
HWND        hWndMain;
HWND        hWndCopy;
map<int, tuple<int, int, int>> bcgColours;

// State of the game parameters
int         board[N][N];            
int         currMax;
int         score;
int         goal;
result      gameEnded;

// Functions
void		LoadGame();
void		SaveGame();
void		NewGame();
void		UpdateBoard();
void		UpdateGoal(int);
void		WinnerLoser(HWND, ColorID);
void		endMove(bool, bool, bool);
HFONT		MyFont(HDC);
void		PrintInt(HWND, int);
void		ColourWnd(HWND, int);
void		CreateSmallWnds(HWND);
void		SymmetricMove(HWND, LPARAM);
bool		fillGaps(TCHAR);
bool		joinTiles(TCHAR);
void		initColorsMap();
int			GetRandom();
void		BoardInsert();
bool		IsMoveable(int, int, int, int);
bool		BoardFull();
bool		isGameOver();



void LoadGame()
{
    ifstream ifs;
    ifs.open("Debug\\2048theGame.ini");
    if (!ifs)
    {
        NewGame();
        return;
    }

    char t[MAX_LOADSTRING];
    char ogr = '=';

    ifs.getline(t, MAX_LOADSTRING, ogr);
    int k;
    ifs >> k;
    gameEnded = (result)k;

    ifs.getline(t, MAX_LOADSTRING, ogr);
    ifs >> score;

    ifs.getline(t, MAX_LOADSTRING, ogr);
    ifs >> goal;
    UpdateGoal(goal);


    ifs.getline(t, MAX_LOADSTRING, ogr);
    for (int i = 0; i < N; i++)
        for (int j = 0; j < N; j++) {
            ifs >> board[i][j];
            ifs.get();
        }

    UpdateBoard();

    if (gameEnded == win) {
        WinnerLoser(hWndCopy, winner);
        WinnerLoser(hWndMain, winner);
    }
    else if (gameEnded == lose)
    {
        WinnerLoser(hWndCopy, loser);
        WinnerLoser(hWndMain, loser);
    }
}

void SaveGame()
{
    ofstream ofs("Debug\\2048theGame.ini");
    if (!ofs) return;

    ofs << "[2048theGame]" << endl;
    ofs << "STATUS=" << gameEnded << endl;
    ofs << "SCORE=" << score << endl;
    ofs << "GOAL=" << goal << endl;
    ofs << "BOARD=";
    for (int i = 0; i < N; i++)
        for (int j = 0; j < N; j++)
        {
            ofs << board[i][j] << ";";
        }
    ofs.close();
}

void NewGame()
{
    currMax = score = 0;
    gameEnded = win;
    for (int i = 0; i < N; i++)
        for (int j = 0; j < N; j++)
            board[i][j] = 0;

    BoardInsert();
    UpdateBoard();
    gameEnded = during;
}

void UpdateBoard() 
{
    if (gameEnded != during)
    {
        ColourWnd(hWndMain, mainWindow);
        ColourWnd(hWndCopy, mainWindow);
    }
    ColourWnd(smallWindowsMain[N * N], tile);
    PrintInt(smallWindowsMain[N * N], score);
    ColourWnd(smallWindowsCopy[N * N], tile);
    PrintInt(smallWindowsCopy[N * N], score);

    for (int i = 0; i < N; i++)
        for (int j = 0; j < N; j++)
        {
            ColourWnd(smallWindowsMain[i * N + j], 0);
            ColourWnd(smallWindowsCopy[i * N + j], 0);

            if (board[i][j])
            {
                ColourWnd(smallWindowsMain[i * N + j], board[i][j]);
                PrintInt(smallWindowsMain[i * N + j], board[i][j]);
                ColourWnd(smallWindowsCopy[i * N + j], board[i][j]);
                PrintInt(smallWindowsCopy[i * N + j], board[i][j]);

                if (board[i][j] == goal)
                    currMax = goal;
                else if (board[i][j] > currMax)
                    currMax = board[i][j];
            }
            else
            {
                ColourWnd(smallWindowsMain[i * N + j], 0);
                ColourWnd(smallWindowsCopy[i * N + j], 0);
            }
        }
}

void UpdateGoal(int goal)
{
    switch (goal)
    {
    case 8:
        CheckMenuItem(GetMenu(hWndMain), ID_GOAL_8, MF_CHECKED);
        CheckMenuItem(GetMenu(hWndMain), ID_GOAL_16, MF_UNCHECKED);
        CheckMenuItem(GetMenu(hWndMain), ID_GOAL_64, MF_UNCHECKED);
        CheckMenuItem(GetMenu(hWndMain), ID_GOAL_2048, MF_UNCHECKED);
        CheckMenuItem(GetMenu(hWndCopy), ID_GOAL_8, MF_CHECKED);
        CheckMenuItem(GetMenu(hWndCopy), ID_GOAL_16, MF_UNCHECKED);
        CheckMenuItem(GetMenu(hWndCopy), ID_GOAL_64, MF_UNCHECKED);
        CheckMenuItem(GetMenu(hWndCopy), ID_GOAL_2048, MF_UNCHECKED);
        break;
    case 16:
        CheckMenuItem(GetMenu(hWndMain), ID_GOAL_8, MF_UNCHECKED);
        CheckMenuItem(GetMenu(hWndMain), ID_GOAL_16, MF_CHECKED);
        CheckMenuItem(GetMenu(hWndMain), ID_GOAL_64, MF_UNCHECKED);
        CheckMenuItem(GetMenu(hWndMain), ID_GOAL_2048, MF_UNCHECKED);
        CheckMenuItem(GetMenu(hWndCopy), ID_GOAL_8, MF_UNCHECKED);
        CheckMenuItem(GetMenu(hWndCopy), ID_GOAL_16, MF_CHECKED);
        CheckMenuItem(GetMenu(hWndCopy), ID_GOAL_64, MF_UNCHECKED);
        CheckMenuItem(GetMenu(hWndCopy), ID_GOAL_2048, MF_UNCHECKED);
        break;
    case 64:
        CheckMenuItem(GetMenu(hWndMain), ID_GOAL_8, MF_UNCHECKED);
        CheckMenuItem(GetMenu(hWndMain), ID_GOAL_16, MF_UNCHECKED);
        CheckMenuItem(GetMenu(hWndMain), ID_GOAL_64, MF_CHECKED);
        CheckMenuItem(GetMenu(hWndMain), ID_GOAL_2048, MF_UNCHECKED);
        CheckMenuItem(GetMenu(hWndCopy), ID_GOAL_8, MF_UNCHECKED);
        CheckMenuItem(GetMenu(hWndCopy), ID_GOAL_16, MF_UNCHECKED);
        CheckMenuItem(GetMenu(hWndCopy), ID_GOAL_64, MF_CHECKED);
        CheckMenuItem(GetMenu(hWndCopy), ID_GOAL_2048, MF_UNCHECKED);
        break;
    case 2048:
        CheckMenuItem(GetMenu(hWndMain), ID_GOAL_8, MF_UNCHECKED);
        CheckMenuItem(GetMenu(hWndMain), ID_GOAL_16, MF_UNCHECKED);
        CheckMenuItem(GetMenu(hWndMain), ID_GOAL_64, MF_UNCHECKED);
        CheckMenuItem(GetMenu(hWndMain), ID_GOAL_2048, MF_CHECKED);
        CheckMenuItem(GetMenu(hWndCopy), ID_GOAL_8, MF_UNCHECKED);
        CheckMenuItem(GetMenu(hWndCopy), ID_GOAL_16, MF_UNCHECKED);
        CheckMenuItem(GetMenu(hWndCopy), ID_GOAL_64, MF_UNCHECKED);
        CheckMenuItem(GetMenu(hWndCopy), ID_GOAL_2048, MF_CHECKED);
        break;
    default:break;
    }
}

void WinnerLoser(HWND hwnd, ColorID res)
{
    gameEnded = res == winner ? win : lose;
    map<int, tuple<int, int, int>>::iterator it = bcgColours.find(res);
    if (it == bcgColours.end()) return;
    tuple<int, int, int> t = it->second;

    BLENDFUNCTION bf = { AC_SRC_OVER, 0, 136, 0 };
    RECT rc;
    GetClientRect(hwnd, &rc);
    int w = rc.right - rc.left;
    int h = rc.bottom - rc.top;
    HDC hdc = GetDC(hwnd);
    HDC temp = CreateCompatibleDC(hdc);
    HBITMAP bmap = CreateCompatibleBitmap(hdc, w, h);
    HBITMAP oldmap = (HBITMAP)SelectObject(temp, bmap);
    HBRUSH brush = CreateSolidBrush(RGB(get<0>(t), get<1>(t), get<2>(t)));
    FillRect(temp, &rc, brush);

    AlphaBlend(hdc, rc.left, rc.top, w, h, temp, rc.left, rc.top, w, h, bf);
    SelectObject(hdc, oldmap);
    DeleteObject(bmap);
    DeleteObject(brush);
    DeleteDC(temp);

    // pisanie tekstu
    const int s_size = 11;
    wchar_t s[s_size];
    swprintf_s(s, s_size, res == ColorID::winner ? L"WIN!" : L"GAME OVER");

    HFONT font = MyFont(hdc);
    HFONT oldFont = (HFONT)SelectObject(hdc, font);
    SetTextColor(hdc, RGB(255, 255, 255));
    SetBkMode(hdc, TRANSPARENT);
    DrawText(hdc, s, (int)_tcslen(s), &rc, DT_CENTER | DT_VCENTER | DT_SINGLELINE);
    ReleaseDC(hwnd, hdc);
    DeleteObject(font);
    DeleteDC(hdc);
}

void endMove(bool m1, bool m2, bool m3)
{
    if ((m1 || m2 || m3) && !BoardFull())
        BoardInsert();

    UpdateBoard();
    if (currMax == goal)
    {
        WinnerLoser(hWndCopy, ColorID::winner);
        WinnerLoser(hWndMain, ColorID::winner);
        return;
    }
    else if (BoardFull())
    {
        if (isGameOver())
        {
            WinnerLoser(hWndMain, ColorID::loser);
            WinnerLoser(hWndCopy, ColorID::loser);
        }
    }

}

HFONT MyFont(HDC hdc)
{
    return CreateFont(
        -MulDiv(24, GetDeviceCaps(hdc, LOGPIXELSY), 100),
        0,
        0,
        0,
        FW_BOLD,
        false,
        false,
        false,
        EASTEUROPE_CHARSET,
        OUT_DEFAULT_PRECIS,
        CLIP_DEFAULT_PRECIS,
        DEFAULT_QUALITY,
        DEFAULT_PITCH | FF_SWISS,
        _T("Arial"));
}

void PrintInt(HWND hWnd, int k)
{

    const int s_size = 10;
    wchar_t s[s_size];
    swprintf_s(s, s_size, L"%d", k);

    HDC hdc = GetDC(hWnd);
    HFONT font = MyFont(hdc);
    HFONT oldFont = (HFONT)SelectObject(hdc, font);
    RECT rc;
    GetClientRect(hWnd, &rc);
    SetTextColor(hdc, RGB(255, 255, 255));
    SetBkMode(hdc, TRANSPARENT);
    DrawText(hdc, s, (int)_tcslen(s), &rc, DT_CENTER | DT_VCENTER | DT_SINGLELINE);

    SelectObject(hdc, oldFont);
    DeleteObject(font);
    ReleaseDC(hWnd, hdc);
}

void ColourWnd(HWND hWnd, int k)
{
    map<int, tuple<int, int, int>>::iterator it = bcgColours.find(k);
    if (it == bcgColours.end()) return;
    tuple<int, int, int> t = it->second;

    RECT rc;
    GetClientRect(hWnd, &rc);
    HDC hdc = GetDC(hWnd);
    HBRUSH brush = CreateSolidBrush(RGB(get<0>(t), get<1>(t), get<2>(t)));
    HBRUSH old = (HBRUSH)SelectObject(hdc, brush);
    FillRect(hdc, &rc, brush);
    SelectObject(hdc, old);
    DeleteObject(brush);
    ReleaseDC(hWnd, hdc);
}

void CreateSmallWnds(HWND hWnd)
{
    RECT rc, rcWnd;
    GetWindowRect(hWnd, &rcWnd);
    int w = rcWnd.right - rcWnd.left;
    int h = rcWnd.bottom - rcWnd.top;
    GetClientRect(hWnd, &rc);
    HWND* tab = nullptr;
    if (hWnd == hWndMain)
    {
        tab = smallWindowsMain;
    }
    else if (hWnd == hWndCopy)
    {
        tab = smallWindowsCopy;
    }

    int ellipse = 13;
    for (int j = 0; j < N; j++)
        for (int i = 0; i < N; i++)
        {

            tab[j * N + i] = CreateWindowW(szWindowClassSmall, nullptr, WS_CHILD | WS_VISIBLE,
                i * CHLD_SIZE + (i + 1) * PAUSE_SIZE, (j + 1) * CHLD_SIZE + (j + 2) * PAUSE_SIZE, CHLD_SIZE, CHLD_SIZE, hWnd, (HMENU)(j * N + i), hInst, nullptr);


            HRGN hrgn = CreateRoundRectRgn(0, 0, CHLD_SIZE, CHLD_SIZE, ellipse, ellipse);
            SetWindowRgn(tab[j * N + i], hrgn, TRUE);
            UpdateWindow(tab[j * N + i]);
            DeleteObject(hrgn);
        }

    tab[N * N] = CreateWindowW(szWindowClassSmall, nullptr, WS_CHILD | WS_VISIBLE,
        PAUSE_SIZE, PAUSE_SIZE, CHLD_SIZE * 4 + 3 * PAUSE_SIZE, CHLD_SIZE, hWnd, nullptr, hInst, nullptr);

    HRGN hrgn = CreateRoundRectRgn(0, 0, CHLD_SIZE * 4 + 3 * PAUSE_SIZE, CHLD_SIZE, ellipse, ellipse);
    SetWindowRgn(tab[N * N], hrgn, TRUE);
    UpdateWindow(tab[N * N]);
    DeleteObject(hrgn);
}

void SymmetricMove(HWND hWnd, LPARAM lParam)
{
    int screenX = GetSystemMetrics(SM_CXSCREEN);
    int screenY = GetSystemMetrics(SM_CYSCREEN);

    RECT rc;
    GetWindowRect(hWnd, &rc);
    int wndWidth = rc.right - rc.left;
    int wndHeight = rc.bottom - rc.top;
    int x = (int)(short)LOWORD(lParam);
    int y = (int)(short)HIWORD(lParam);
    int x_ = screenX - wndWidth - x;
    int y_ = screenY - wndHeight - y;

    if (hWndMain == hWnd)
    {
        MoveWindow(hWndCopy, x_, y_, wndWidth, wndHeight, TRUE);
    }
    else if (hWndCopy == hWnd)
    {
        MoveWindow(hWndMain, x_, y_, wndWidth, wndHeight, TRUE);
        RECT rc2;
        GetClientRect(hWnd, &rc2);

        SetWindowLong(hWnd, GWL_EXSTYLE, GetWindowLong(hWnd, GWL_EXSTYLE) | WS_EX_LAYERED);
        int transparent = 100;
        if ((x_ + rc2.right >= rc.left && rc.left + rc2.right >= x_) && (y_ + rc2.bottom + 50 >= rc.top && rc.top + rc2.bottom + 50 >= y_))
        {
            transparent = 50;
        }
        SetLayeredWindowAttributes(hWnd, 0, (255 * transparent) / 100, LWA_ALPHA);
        UpdateWindow(hWnd);
    }
}

bool fillGaps(TCHAR c)
{
    int n = 0;  // start row
    int m = 0;  // start column
    int k = 1;  // step row
    int l = 1;  // step col
    int p = 1;
    int next_i, next_j;
    int* it = &next_i;
    verticalMoves vert;
    horizontalMoves hor;

    switch (c)
    {
    case 'w':
    {
        vert = Up;
        hor = NopeH;
    }
    break;
    case 's':
    {
        vert = Down;
        hor = NopeH;
        n = 3;
        k = p = -1;
    }
    break;
    case 'a':
    {
        vert = NopeV;
        hor = Right;
        it = &next_j;
    }
    break;
    case 'd':
    {
        vert = NopeV;
        hor = Left;
        m = 3;
        l = p = -1;
        it = &next_j;
    }
    break;
    default: return false;
    }

    bool moved = false;
    for (int i = n; i >= 0 && i < N; i += k)
        for (int j = m; j >= 0 && j < N; j += l)
        {
            if (board[i][j]) continue;
            next_i = i + vert;
            next_j = j + hor;

            for (; *it >= 0 && *it < N; *it += p)
            {
                if (board[next_i][next_j])
                {
                    board[i][j] = board[next_i][next_j];
                    board[next_i][next_j] = 0;
                    moved = true;
                    break;
                }
            }
        }

    return moved;
}

bool joinTiles(TCHAR c)
{
    int n = 0;  // start row
    int m = 0;  // start column
    int k = 1;  // step row
    int l = 1;  // step col
    int p = 1;
    int next_i, next_j;
    verticalMoves vert;
    horizontalMoves hor;

    switch (c)
    {
    case 'w':
    {
        vert = Up;
        hor = NopeH;
    }
    break;
    case 's':
    {
        vert = Down;
        hor = NopeH;
        n = 3;
        k = p = -1;
    }
    break;
    case 'a':
    {
        vert = NopeV;
        hor = Right;
    }
    break;
    case 'd':
    {
        vert = NopeV;
        hor = Left;
        m = 3;
        l = p = -1;
    }
    break;
    default: return false;
    }

    bool moved = false;
    for (int i = n; i >= 0 && i < N; i += k)
        for (int j = m; j >= 0 && j < N; j += l)
        {
            next_i = i + vert;
            next_j = j + hor;

            if (IsMoveable(i, j, next_i, next_j))
            {
                board[i][j] += board[next_i][next_j];
                board[next_i][next_j] = 0;
                moved = true;
                score += board[i][j];
            }
        }

    return moved;
}

void initColorsMap()
{
    bcgColours.insert(make_pair(tile, make_tuple(204, 192, 174)));
    bcgColours.insert(make_pair(mainWindow, make_tuple(250, 247, 238)));
    bcgColours.insert(make_pair(pow(2, 1), make_tuple(238, 228, 198)));
    bcgColours.insert(make_pair(pow(2, 2), make_tuple(239, 225, 218)));
    bcgColours.insert(make_pair(pow(2, 3), make_tuple(243, 179, 124)));
    bcgColours.insert(make_pair(pow(2, 4), make_tuple(246, 153, 100)));
    bcgColours.insert(make_pair(pow(2, 5), make_tuple(246, 125, 98)));
    bcgColours.insert(make_pair(pow(2, 6), make_tuple(247, 93, 60)));
    bcgColours.insert(make_pair(pow(2, 7), make_tuple(237, 206, 116)));
    bcgColours.insert(make_pair(pow(2, 8), make_tuple(239, 204, 98)));
    bcgColours.insert(make_pair(pow(2, 9), make_tuple(243, 201, 85)));
    bcgColours.insert(make_pair(pow(2, 10), make_tuple(238, 200, 72)));
    bcgColours.insert(make_pair(pow(2, 11), make_tuple(239, 192, 47)));
    bcgColours.insert(make_pair(ColorID::winner, make_tuple(0, 255, 0)));
    bcgColours.insert(make_pair(ColorID::loser, make_tuple(255, 0, 0)));
}

int GetRandom()
{
    int id;
    do {
        id = rand() % (N * N);
    } while (board[id / N][id % N]);

    return id;
}

void BoardInsert()
{
    int id = GetRandom();
    board[id / N][id % N] = 2;
}

bool IsMoveable(int i, int j, int next_i, int next_j)
{

    return (next_i >= 0 && next_i < N&&
        next_j >= 0 && next_j < N&&
        board[i][j] &&
        board[i][j] == board[next_i][next_j]);
}

bool BoardFull()
{
    for (int i = 0; i < N; i++)
        for (int j = 0; j < N; j++)
            if (board[i][j] == 0) return false;

    return true;
}

bool isGameOver()
{
    for (int i = 0; i < N - 1; i++)
        for (int j = 0; j < N - 1; j++)
        {
            int item = board[i][j];
            if (item == board[i + 1][j] || item == board[i][j + 1])
                return false;
        }

    if (board[N - 2][N - 1] == board[N - 1][N - 1] || board[N - 1][N - 2] == board[N - 1][N - 1])
        return false;

    return true;
}