#include <time.h>
#include "framework.h"
#include "GameLogic.h"

ATOM                MyRegisterClass(HINSTANCE hInstance);
ATOM                MyRegisterClassSmall(HINSTANCE hInstance);
BOOL                InitInstance(HINSTANCE, int);
LRESULT CALLBACK    WndProc(HWND, UINT, WPARAM, LPARAM);
LRESULT CALLBACK    WndProcSmall(HWND, UINT, WPARAM, LPARAM);
INT_PTR CALLBACK    About(HWND, UINT, WPARAM, LPARAM);


int APIENTRY wWinMain(_In_ HINSTANCE hInstance,
                     _In_opt_ HINSTANCE hPrevInstance,
                     _In_ LPWSTR    lpCmdLine,
                     _In_ int       nCmdShow)
{
    UNREFERENCED_PARAMETER(hPrevInstance);
    UNREFERENCED_PARAMETER(lpCmdLine);

    initColorsMap();

    LoadStringW(hInstance, IDS_APP_TITLE, szTitle, MAX_LOADSTRING);
    LoadStringW(hInstance, IDC_WINAPILAB, szWindowClass, MAX_LOADSTRING);
    LoadStringW(hInstance, IDS_SMALL_TITLE, szWindowClassSmall, MAX_LOADSTRING);
    MyRegisterClass(hInstance);
    MyRegisterClassSmall(hInstance);

    if (!InitInstance (hInstance, nCmdShow))
    {
        return FALSE;
    }

    HACCEL hAccelTable = LoadAccelerators(hInstance, MAKEINTRESOURCE(IDC_WINAPILAB));

    MSG msg;

    while (GetMessage(&msg, nullptr, 0, 0))
    {
        if (!TranslateAccelerator(msg.hwnd, hAccelTable, &msg))
        {
            TranslateMessage(&msg);
            DispatchMessage(&msg);
        }
    }

    return (int) msg.wParam;
}


ATOM MyRegisterClass(HINSTANCE hInstance)
{
    WNDCLASSEXW wcex;
    wcex.cbSize = sizeof(WNDCLASSEX);

    map<int, tuple<int, int, int>>::iterator it = bcgColours.find(mainWindow);
    if (it == bcgColours.end()) return RegisterClassExW(&wcex);
    tuple<int, int, int> t = it->second;

    wcex.style          = CS_HREDRAW | CS_VREDRAW;
    wcex.lpfnWndProc    = WndProc;
    wcex.cbClsExtra     = 0;
    wcex.cbWndExtra     = 0;
    wcex.hInstance      = hInstance;
    wcex.hIcon          = LoadIcon(hInstance, MAKEINTRESOURCE(IDI_GAME_ICON));
    wcex.hCursor        = LoadCursor(nullptr, IDC_ARROW);
    wcex.hbrBackground  = CreateSolidBrush(RGB(get<0>(t), get<1>(t), get<2>(t)));
    wcex.lpszMenuName   = MAKEINTRESOURCEW(IDC_WINAPILAB);
    wcex.lpszClassName  = szWindowClass;
    wcex.hIconSm        = LoadIcon(wcex.hInstance, MAKEINTRESOURCE(IDI_GAME_ICON));

    return RegisterClassExW(&wcex);
}


ATOM MyRegisterClassSmall(HINSTANCE hInstance)
{
    WNDCLASSEXW wcex;
    wcex.cbSize = sizeof(WNDCLASSEX);

    map<int, tuple<int, int, int>>::iterator it = bcgColours.find(tile);
    if (it == bcgColours.end()) return RegisterClassExW(&wcex);
    tuple<int, int, int> t = it->second;

    wcex.style = CS_HREDRAW | CS_VREDRAW;
    wcex.lpfnWndProc = WndProcSmall;
    wcex.cbClsExtra = 0;
    wcex.cbWndExtra = 0;
    wcex.hInstance = hInstance;
    wcex.hIcon = LoadIcon(hInstance, MAKEINTRESOURCE(IDI_GAME_ICON));
    wcex.hCursor = LoadCursor(nullptr, IDC_ARROW);
    wcex.hbrBackground = CreateSolidBrush(RGB(get<0>(t), get<1>(t), get<2>(t)));
    wcex.lpszMenuName = MAKEINTRESOURCEW(IDC_WINAPILAB);
    wcex.lpszClassName = szWindowClassSmall;
    wcex.hIconSm = LoadIcon(wcex.hInstance, MAKEINTRESOURCE(IDI_GAME_ICON));

    return RegisterClassExW(&wcex);
}

BOOL InitInstance(HINSTANCE hInstance, int nCmdShow)
{
   hInst = hInstance; 

   int width = CHLD_SIZE * N + (N + 1) * PAUSE_SIZE + N * N;
   int heigth = width + CHLD_SIZE / 0.6 + PAUSE_SIZE;
   int startX = GetSystemMetrics(SM_CXSCREEN);
   int startY = GetSystemMetrics(SM_CYSCREEN);

   hWndMain = CreateWindowW(szWindowClass, szTitle, WS_OVERLAPPEDWINDOW ^ WS_THICKFRAME,
      (startX / 2 - width)/2, (startY / 2 - heigth)/2, width, heigth, nullptr, nullptr, hInstance, nullptr);

   CreateSmallWnds(hWndMain);

   hWndCopy = CreateWindowW(szWindowClass, szTitle, WS_VISIBLE,
       (3*startX / 2 - width) / 2, (3*startY / 2 - heigth) / 2, width, heigth, hWndMain, nullptr, hInstance, nullptr);

   CreateSmallWnds(hWndCopy);

   if (!hWndMain | !hWndCopy)
   {
      return FALSE;
   }

   ShowWindow(hWndMain, nCmdShow);
   UpdateWindow(hWndMain);
   srand(time(NULL));
   LoadGame();
   return TRUE;
}

LRESULT CALLBACK WndProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam)
{

    switch (message)
    {
    case WM_MOVE:
    {
        SymmetricMove(hWnd, lParam);
    }
    break;
    case WM_COMMAND:
    {
        int wmId = LOWORD(wParam);
        switch (wmId)
        {
        case IDM_EXIT:
            DestroyWindow(hWnd);
            break;
        case ID_NEW_GAME:
            NewGame();
            break;
        case ID_GOAL_8:
            goal = 8;
            UpdateGoal(goal);
            break;
        case ID_GOAL_16:
        {
            goal = 16;
            UpdateGoal(goal);
        }   break;
        case ID_GOAL_64:
            goal = 64;
            UpdateGoal(goal);
           break;
        case ID_GOAL_2048:
        {
            goal = 2048;
            UpdateGoal(goal);
        }   break;
        default:
            return DefWindowProc(hWnd, message, wParam, lParam);
        }
    }
    break;

    case WM_ACTIVATE:
        endMove(false, false, false);
        break;
    case WM_CHAR:
    {
        if (hWnd == hWndMain)
            int k = 0;

        if (gameEnded != during) break;
        bool moved1 = fillGaps((TCHAR)wParam);
        bool moved2 = joinTiles((TCHAR)wParam);                
        bool moved3 = fillGaps((TCHAR)wParam);

        endMove(moved1, moved2, moved3);
    }
        break;

    case WM_CLOSE:
        SaveGame();
        DestroyWindow(hWnd);
        break;
    case WM_PAINT:
    {
        PAINTSTRUCT ps;
        HDC hdc = BeginPaint(hWnd, &ps);
        ReleaseDC(hWnd, hdc);
        EndPaint(hWnd, &ps);
    }
    break;
    case WM_DESTROY:
        PostQuitMessage(0);
        break;
    default:
        return DefWindowProc(hWnd, message, wParam, lParam);
    }
    return 0;
}


LRESULT CALLBACK WndProcSmall(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam)
{
    switch (message)
    {
    case WM_PAINT:
    {
        PAINTSTRUCT ps;
        HDC hdc = BeginPaint(hWnd, &ps);
        EndPaint(hWnd, &ps);
    }
    break;
    case WM_DESTROY:
        PostQuitMessage(0);
        break;
    default:
        return DefWindowProc(hWnd, message, wParam, lParam);
    }
    return 0;
}


