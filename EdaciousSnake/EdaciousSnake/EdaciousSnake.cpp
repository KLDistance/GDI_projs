// EdaciousSnake.cpp : Defines the entry point for the application.
//

#include "stdafx.h"
#include "EdaciousSnake.h"
#include <vector>
#include <time.h>
#include <stdio.h>

#define MAX_LOADSTRING 100

//Define the size of the playing panel and the message panel
#define BOUND_SIZE 10
#define SNAKE_SIZE 10
#define GAME_WIDTH 80
#define GAME_HEIGHT 60
#define INFO_WIDTH 30
#define INFO_HEIGHT GAME_HEIGHT
#define MAX_NODE 100 //max length of snake
#define MY_TIMER 1 //timer
#define DEFAULT_INTERVAL 70 //the speed of snake's moving (dimension: ms)
#define PAUSE_ID 1

#define INITLEN 3
#define INITHEAD 3

//Color sets
#define foodColor RGB(255, 236, 139)	//foodColor
#define snakeColor RGB(255, 255, 255)	//SnakeColor
#define backgroundColor RGB(190, 190, 190)	//background color

std::vector<POINT> vSnake;
//initial length of the snake
UCHAR g_ucSnakeLen = INITLEN;
//Pos of the head
UCHAR g_ucSnakeHead = INITHEAD;
//tail pos
UCHAR g_ucSnakeTail = 0;
UINT32 g_uiInterval = DEFAULT_INTERVAL; // moving speed
POINT g_ptDirect = { 1, 0 };//mov pos: x+1 while y retains
POINT g_ptFoodPos;//pos of food
BOOL g_bPause = FALSE;
BOOL g_bState = FALSE;//whether game is over
BOOL g_bWin = FALSE;
BOOL g_bNeedFood = TRUE;//whether there is a need to put out food



// Global Variables:
HINSTANCE hInst;                                // current instance
WCHAR szTitle[MAX_LOADSTRING];                  // The title bar text
WCHAR szWindowClass[MAX_LOADSTRING];            // the main window class name

// Forward declarations of functions included in this code module:
ATOM                MyRegisterClass(HINSTANCE hInstance);
BOOL                InitInstance(HINSTANCE, int);
LRESULT CALLBACK    WndProc(HWND, UINT, WPARAM, LPARAM);
INT_PTR CALLBACK    About(HWND, UINT, WPARAM, LPARAM);

int APIENTRY wWinMain(_In_ HINSTANCE hInstance,
                     _In_opt_ HINSTANCE hPrevInstance,
                     _In_ LPWSTR    lpCmdLine,
                     _In_ int       nCmdShow)
{
    UNREFERENCED_PARAMETER(hPrevInstance);
    UNREFERENCED_PARAMETER(lpCmdLine);

    // TODO: Place code here.

    // Initialize global strings
    LoadStringW(hInstance, IDS_APP_TITLE, szTitle, MAX_LOADSTRING);
    LoadStringW(hInstance, IDC_EDACIOUSSNAKE, szWindowClass, MAX_LOADSTRING);
    MyRegisterClass(hInstance);

    // Perform application initialization:
    if (!InitInstance (hInstance, nCmdShow))
    {
        return FALSE;
    }

    HACCEL hAccelTable = LoadAccelerators(hInstance, MAKEINTRESOURCE(IDC_EDACIOUSSNAKE));

    MSG msg;

    // Main message loop:
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



//
//  FUNCTION: MyRegisterClass()
//
//  PURPOSE: Registers the window class.
//
ATOM MyRegisterClass(HINSTANCE hInstance)
{
    WNDCLASSEXW wcex;

    wcex.cbSize = sizeof(WNDCLASSEX);

    wcex.style          = CS_HREDRAW | CS_VREDRAW;
    wcex.lpfnWndProc    = WndProc;
    wcex.cbClsExtra     = 0;
    wcex.cbWndExtra     = 0;
    wcex.hInstance      = hInstance;
    wcex.hIcon          = LoadIcon(hInstance, MAKEINTRESOURCE(IDI_EDACIOUSSNAKE));
    wcex.hCursor        = LoadCursor(nullptr, IDC_ARROW);
    wcex.hbrBackground  = NULL;
    wcex.lpszMenuName   = MAKEINTRESOURCEW(IDC_EDACIOUSSNAKE);
    wcex.lpszClassName  = szWindowClass;
    wcex.hIconSm        = LoadIcon(wcex.hInstance, MAKEINTRESOURCE(IDI_SMALL));

    return RegisterClassExW(&wcex);
}

//
//   FUNCTION: InitInstance(HINSTANCE, int)
//
//   PURPOSE: Saves instance handle and creates main window
//
//   COMMENTS:
//
//        In this function, we save the instance handle in a global variable and
//        create and display the main program window.
//
BOOL InitInstance(HINSTANCE hInstance, int nCmdShow)
{
   hInst = hInstance; // Store instance handle in our global variable

   HWND hWnd = CreateWindowW(szWindowClass, szTitle, WS_SYSMENU | WS_MINIMIZEBOX,
      CW_USEDEFAULT, 0, CW_USEDEFAULT, 0, nullptr, nullptr, hInstance, nullptr);

   if (!hWnd)
   {
      return FALSE;
   }

   ShowWindow(hWnd, nCmdShow);
   UpdateWindow(hWnd);

   return TRUE;
}

//initial snake
VOID InitSnake()
{
	int i;
	vSnake.clear();
	vSnake.resize(MAX_NODE);
	g_ucSnakeTail = 0;
	g_ucSnakeHead = INITHEAD;
	g_ucSnakeLen = INITLEN;
	g_uiInterval = DEFAULT_INTERVAL;

	for (i = 0; i < g_ucSnakeLen; i++)
	{
		vSnake[i].x = i;
		vSnake[i].y = 1;
	}
}
//Get the pos of the node: where is the last node
POINT &GetSnakeNode(int index)
{
	int i = g_ucSnakeTail + index;
	if (i >= MAX_NODE)
	{
		i -= MAX_NODE;
	}
	return vSnake[i];
}

//Draw the snake
VOID DrawSnake(HDC hdc)
{
	int i;
	POINT ptNode;
	//define the brush and the painting kit
	HBRUSH hBrush = CreateSolidBrush(snakeColor);
	SelectObject(hdc, hBrush);
	for (i = 0; i < g_ucSnakeLen; i++)
	{
		ptNode = GetSnakeNode(i);
		Rectangle(hdc, ptNode.x * SNAKE_SIZE + BOUND_SIZE, ptNode.y * SNAKE_SIZE + BOUND_SIZE, (ptNode.x + 1) * SNAKE_SIZE + BOUND_SIZE,
			(ptNode.y + 1) * SNAKE_SIZE + BOUND_SIZE);
	}
}

//mov the pos of snake
VOID RefreshSnake()
{
	//scrolling form
	POINT ptNewHead;//new head(new pos of the snakeHead)
	POINT ptNode;
	int i;
	ptNewHead.x = GetSnakeNode(g_ucSnakeLen - 1).x + g_ptDirect.x;
	ptNewHead.y = GetSnakeNode(g_ucSnakeLen - 1).y + g_ptDirect.y;

	if (!g_bNeedFood && ptNewHead.x == g_ptFoodPos.x && ptNewHead.y == g_ptFoodPos.y)
	{
		vSnake[g_ucSnakeHead] = ptNewHead;
		g_ucSnakeHead += 1;
		if (g_ucSnakeHead == MAX_NODE)
			g_ucSnakeHead = 0;
		g_ucSnakeLen += 1;
		if (g_ucSnakeLen == MAX_NODE)
		{
			//player wins
			g_bWin = TRUE;
			return;
		}
		g_bNeedFood = TRUE;
		return;
	}

	//Collision test
	if (ptNewHead.x < 0 || ptNewHead.x >= GAME_WIDTH || ptNewHead.y < 0 || ptNewHead.y >= GAME_HEIGHT)
	{
		g_bState = TRUE;
		return;
	}
	//whether collision happens on snake itself
	for (i = 1; i < g_ucSnakeLen; i++)
	{
		ptNode = GetSnakeNode(i);
		if (ptNode.x == ptNewHead.x && ptNode.y == ptNewHead.y)
		{
			g_bState = TRUE;
		}
	}

	vSnake[g_ucSnakeHead].x = ptNewHead.x;
	vSnake[g_ucSnakeHead].y = ptNewHead.y;

	g_ucSnakeHead++;
	if (g_ucSnakeHead == MAX_NODE)
		g_ucSnakeHead = 0;
	g_ucSnakeTail++;
	if (g_ucSnakeTail == MAX_NODE)
		g_ucSnakeTail = 0;
}

VOID DrawFood(HDC hdc)
{
	int x, y; // pos of the food
	int i;
	POINT ptNode;//the food cannot be put on the snake
	HBRUSH hBrush = CreateSolidBrush(foodColor);
	if (!g_bNeedFood)
	{
		SelectObject(hdc, hBrush);
		Ellipse(hdc, BOUND_SIZE + g_ptFoodPos.x*SNAKE_SIZE, BOUND_SIZE + g_ptFoodPos.y*SNAKE_SIZE,
			BOUND_SIZE + (g_ptFoodPos.x + 1)*SNAKE_SIZE, BOUND_SIZE + (g_ptFoodPos.y + 1)*SNAKE_SIZE);
		return;
	}
	srand((unsigned int)time(0));//seed of random
	//acquire the random coordinates
	while (1)
	{
		x = rand() % GAME_WIDTH;
		y = rand() % GAME_HEIGHT;
		for (i = 0; i < g_ucSnakeLen; i++)
		{
			ptNode = GetSnakeNode(i);
			if (ptNode.x == x && ptNode.y == y)
			{
				break;
			}
		}
		//no situation where food is put on the body of the snake
		if (i == g_ucSnakeLen)
			break;
	}
	g_bNeedFood = FALSE;
	g_ptFoodPos.x = x;
	g_ptFoodPos.y = y;
	return;
}

//
//  FUNCTION: WndProc(HWND, UINT, WPARAM, LPARAM)
//
//  PURPOSE:  Processes messages for the main window.
//
//  WM_COMMAND  - process the application menu
//  WM_PAINT    - Paint the main window
//  WM_DESTROY  - post a quit message and return
//
//
LRESULT CALLBACK WndProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam)
{
	RECT rect;
//	int i;
	int nWinX, nWinY, nClientX, nClientY;
	HBRUSH hBrush;
	static HWND hPause;

    switch (message)
    {
	//execute when create the window
	case WM_CREATE:
		GetWindowRect(hWnd, &rect);		//Get the present size
		nWinX = rect.right - rect.left;
		nWinY = rect.bottom - rect.top;
		GetClientRect(hWnd, &rect);		//Client Size
		nClientX = rect.right - rect.left;
		nClientY = rect.bottom - rect.top;

		//Change the size of the window
		MoveWindow(hWnd, 200, 50, (GAME_WIDTH + INFO_WIDTH) * SNAKE_SIZE + BOUND_SIZE * 3 + (nWinX - nClientX), 
			GAME_HEIGHT * SNAKE_SIZE + BOUND_SIZE * 2 + (nWinY - nClientY), TRUE);

		hPause = CreateWindow(TEXT("BUTTON"), TEXT("PAUSE"), 
			WS_CHILD | WS_VISIBLE | BS_PUSHBUTTON, 
			3 * BOUND_SIZE + GAME_WIDTH * SNAKE_SIZE, GAME_HEIGHT * SNAKE_SIZE - 4*BOUND_SIZE, 100, 40, 
			hWnd, (HMENU)PAUSE_ID, hInst, NULL);
		InitSnake();
		SetTimer(hWnd, MY_TIMER, g_uiInterval, NULL);//set up a timer
		break;
	case WM_TIMER:
		//move the snake
		RefreshSnake();
		if (g_bState)
		{
			KillTimer(hWnd, MY_TIMER);
			MessageBox(NULL, TEXT("You Lose !"), TEXT("FAIL"), MB_OK);
//			InitSnake();
			return 0;
		}
		InvalidateRect(hWnd, NULL, TRUE);
		break;
    case WM_COMMAND:
        {
            int wmId = LOWORD(wParam);
            // Parse the menu selections:
            switch (wmId)
            {
			case PAUSE_ID:
				if (g_bPause)
				{
					g_bPause = FALSE;
					SetWindowText(hPause, TEXT("PAUSE"));
					SetTimer(hWnd, MY_TIMER, g_uiInterval, NULL);
				}
				else
				{
					g_bPause = TRUE;
					SetWindowText(hPause, TEXT("CONTINUE"));
					KillTimer(hWnd, MY_TIMER);
				}
				break;
            case IDM_ABOUT:
                DialogBox(hInst, MAKEINTRESOURCE(IDD_ABOUTBOX), hWnd, About);
                break;
            case IDM_EXIT:
                DestroyWindow(hWnd);
                break;
            default:
                return DefWindowProc(hWnd, message, wParam, lParam);
            }
        }
        break;
    case WM_PAINT:
        {
            PAINTSTRUCT ps;
            HDC hdc = BeginPaint(hWnd, &ps);
			HDC memDC = CreateCompatibleDC(NULL);
			HBITMAP hBitMap = CreateCompatibleBitmap(hdc, 850, 650);
            // TODO: Add any drawing code that uses hdc here...
			hBrush = CreateSolidBrush(backgroundColor);
			SelectObject(memDC, hBitMap);
			Rectangle(memDC, BOUND_SIZE, BOUND_SIZE, BOUND_SIZE + GAME_WIDTH*SNAKE_SIZE, BOUND_SIZE + GAME_HEIGHT * SNAKE_SIZE);
			Rectangle(memDC, BOUND_SIZE * 2 + GAME_WIDTH * SNAKE_SIZE, BOUND_SIZE,   
				BOUND_SIZE * 2 + (GAME_WIDTH + INFO_WIDTH)*SNAKE_SIZE, BOUND_SIZE + INFO_HEIGHT * SNAKE_SIZE);
			DrawSnake(memDC);
			DrawFood(memDC);
			BitBlt(hdc, 0, 0, 850, 650, memDC, 0, 0, SRCCOPY);
            EndPaint(hWnd, &ps);
        }
        break;
	case WM_KEYDOWN:
		if (g_bState || g_bPause)
		{
			break;
		}
		switch (wParam)
		{
			//Beware that previous direction (down is not allowed if previous up is scanned)
		case VK_UP:
			if (g_ptDirect.x != 0)
			{
				g_ptDirect.x = 0;
				g_ptDirect.y = -1;
			}
			break;
		case VK_DOWN:
			if (g_ptDirect.x != 0)
			{
				g_ptDirect.x = 0;
				g_ptDirect.y = 1;
			}
			break;
		case VK_LEFT:
			if (g_ptDirect.y != 0)
			{
				g_ptDirect.x = -1;
				g_ptDirect.y = 0;
			}
			break;
		case VK_RIGHT:
			if (g_ptDirect.y != 0)
			{
				g_ptDirect.x = 1;
				g_ptDirect.y = 0;
			}
			break;
		}
		break;
    case WM_DESTROY:
		KillTimer(hWnd, MY_TIMER);
        PostQuitMessage(0);
        break;
    default:
        return DefWindowProc(hWnd, message, wParam, lParam);
    }
    return 0;
}

// Message handler for about box.
INT_PTR CALLBACK About(HWND hDlg, UINT message, WPARAM wParam, LPARAM lParam)
{
    UNREFERENCED_PARAMETER(lParam);
    switch (message)
    {
    case WM_INITDIALOG:
        return (INT_PTR)TRUE;

    case WM_COMMAND:
        if (LOWORD(wParam) == IDOK || LOWORD(wParam) == IDCANCEL)
        {
            EndDialog(hDlg, LOWORD(wParam));
            return (INT_PTR)TRUE;
        }
        break;
    }
    return (INT_PTR)FALSE;
}
