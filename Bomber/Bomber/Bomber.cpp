// Bomber.cpp : Defines the entry point for the application.
//

#include "stdafx.h"
#include "Bomber.h"
#include "cLogic.h"

#pragma comment(lib, "Msimg32.lib")		//declare the static lib that includes transparent func

#define MAX_LOADSTRING 1000

#define StoneColor RGB(50, 50, 50)	//color of the stone
#define BrickColor RGB(205, 170, 125)	//color of the brick
#define BackgroundColor RGB(200, 255, 200) //color of the background
#define HeroTransparentColor RGB(248, 248, 248)	//color that cancelled by transparent func which exerts on hero.bmp
#define HeroFailTransparentColor RGB(35, 233, 40)//color when fails
#define BombTransparentColor RGB(255, 255, 255)	//color that cancelled by transparent func which exerts on bomb.bmp
#define FireTransparentColor RGB(255, 255, 255)	//color that cancelled by transparent func which exerts on explosion.bmp
#define MonsterTransparentColor RGB(255, 255, 255)	//color that cancelled by transparent func which exerts on enemy1.bmp or else

#define TimeIntervalOfTyping 20	//define the type listening interval


// Global Variables:
HINSTANCE hInst;                                // current instance
WCHAR szTitle[MAX_LOADSTRING];                  // The title bar text
WCHAR szWindowClass[MAX_LOADSTRING];            // the main window class name

cLogic g_Logic; //data from logic processing

void DrawMap(HWND hWnd);//draw the map
void DrawStoneAndBrick(HDC hdc);//draw stone and bricks
void DrawMan(HDC hdc);//draw man
void DrawMonster(HDC hdc);//draw monster
void DrawBomb(HDC hdc);	//draw bombs
void DrawFire(HDC hdc);	//draw fire flash
void DrawDoor(HDC hdc);//draw door

void OnKeyDown(HWND hWnd, WPARAM wParam);	//on keyboard down msg
void OnKeyUp(HWND hWnd, WPARAM wParam);		//on keyboard up msg

DWORD g_dwClickTime = 1;	//time that dir boardkey is typed

					  // Forward declarations of functions included in this code module:
ATOM                MyRegisterClass(HINSTANCE hInstance);
BOOL                InitInstance(HINSTANCE, int);
LRESULT CALLBACK    WndProc(HWND, UINT, WPARAM, LPARAM);
INT_PTR CALLBACK    About(HWND, UINT, WPARAM, LPARAM);

void CALLBACK TimerProc(HWND hWnd, UINT nMsg, UINT nTimerid, DWORD dwTime);

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
	LoadStringW(hInstance, IDC_BOMBER, szWindowClass, MAX_LOADSTRING);
	MyRegisterClass(hInstance);

	// Perform application initialization:
	if (!InitInstance(hInstance, nCmdShow))
	{
		return FALSE;
	}

	HACCEL hAccelTable = LoadAccelerators(hInstance, MAKEINTRESOURCE(IDC_BOMBER));

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

	return (int)msg.wParam;
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

	wcex.style = CS_HREDRAW | CS_VREDRAW;
	wcex.lpfnWndProc = WndProc;
	wcex.cbClsExtra = 0;
	wcex.cbWndExtra = 0;
	wcex.hInstance = hInstance;
	wcex.hIcon = LoadIcon(hInstance, MAKEINTRESOURCE(IDI_BOMBER));
	wcex.hCursor = LoadCursor(nullptr, IDC_ARROW);
	wcex.hbrBackground = NULL;		//!!!!!!!!!!!!!!!!!!!!!!! beware to set null of hbrbackground in case of flash
	wcex.lpszMenuName = NULL;
	wcex.lpszClassName = szWindowClass;
	wcex.hIconSm = LoadIcon(wcex.hInstance, MAKEINTRESOURCE(IDI_SMALL));

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

	HWND hWnd = CreateWindowW(szWindowClass, szTitle, WS_OVERLAPPEDWINDOW & ~WS_MAXIMIZEBOX & ~WS_THICKFRAME,
		CW_USEDEFAULT, 0, CW_USEDEFAULT, 0, nullptr, nullptr, hInstance, nullptr);

	if (!hWnd)
	{
		return FALSE;
	}

	HDC screenDc;
	screenDc = CreateDC(_T("DISPLAY"), NULL, NULL, NULL);
	int xRes = GetDeviceCaps(screenDc, HORZRES);
	int yRes = GetDeviceCaps(screenDc, VERTRES);
	int xPos = (xRes - 856) / 2;
	int yPos = (yRes - 678) / 2;
	MoveWindow(hWnd, xPos, yPos, 856, 678, NULL);

	g_Logic.GameBegin();

	ShowWindow(hWnd, nCmdShow);
	UpdateWindow(hWnd);
	SetTimer(hWnd, 1, 20, TimerProc);

	return TRUE;
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
	switch (message)
	{
	case WM_COMMAND:
	{
		int wmId = LOWORD(wParam);
		// Parse the menu selections:
		switch (wmId)
		{
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
		// TODO: Add any drawing code that uses hdc here...

		DrawMap(hWnd);

		EndPaint(hWnd, &ps);
		DeleteObject(hWnd);
	}
	break;
	case WM_KEYDOWN:
		OnKeyDown(hWnd, wParam);
		break;
	case WM_KEYUP:
		OnKeyUp(hWnd, wParam);
		break;
	case WM_DESTROY:
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

void DrawMap(HWND hWnd)
{
	HDC hdc = GetWindowDC(hWnd);
	//buffer dc
	HDC memDC = CreateCompatibleDC(NULL);
	HBITMAP hBitMap = CreateCompatibleBitmap(hdc, 850, 650);
	SelectObject(memDC, hBitMap);
	HBRUSH hBrushBack = CreateSolidBrush(BackgroundColor);
	RECT rectBack;
	rectBack.top = 0;
	rectBack.bottom = 650;
	rectBack.left = 0;
	rectBack.right = 850;
	FillRect(memDC, &rectBack, hBrushBack);

	DrawStoneAndBrick(memDC);//draw stone and brick
	DrawBomb(memDC);	//draw the bombs
	DrawFire(memDC);	//draw the fire from explosion
	DrawDoor(memDC);	//draw the door to the next mission
	DrawMonster(memDC);	//draw the monsters
	//Man should be drawn at last
	DrawMan(memDC);

	BitBlt(hdc, 3, 25, 850, 650, memDC, 0, 0, SRCCOPY);
	DeleteObject(memDC);
	DeleteObject(hBitMap);
	DeleteObject(hBrushBack);
	ReleaseDC(hWnd, hdc);
}

void DrawStoneAndBrick(HDC hdc)
{
	HBRUSH hbrushStone = CreateSolidBrush(StoneColor);
	HBRUSH hbrushBrick = CreateSolidBrush(BrickColor);
	HBRUSH hbrushBrickDoor = CreateSolidBrush(RGB(13,133,255));
	int i;
	int j;
	for (i = 0; i < LatticeNum_L; i++)
	{
		for (j = 0; j < LatticeNum_H; j++)
		{
			LatticeType curType = g_Logic.m_Lattice[i][j];
			if (curType == Type_Stone)
			{
				RECT rect;
				rect.left = j*Lattice_Len;
				rect.right = rect.left + Lattice_Len;
				rect.top = i*Lattice_Len;
				rect.bottom = rect.top + Lattice_Len;
				FillRect(hdc, &rect, hbrushStone);	//draw the stone to the buffer DC
			}
			if (curType == Type_Brick)
			{
				//draw bricks
				RECT rect;
				rect.left = j*Lattice_Len;
				rect.right = rect.left + Lattice_Len;
				rect.top = i*Lattice_Len;
				rect.bottom = rect.top + Lattice_Len;
				FillRect(hdc, &rect, hbrushBrick);
			}
			if (curType == Type_BrickDoor)
			{
				RECT rect;
				rect.left = j*Lattice_Len;
				rect.right = rect.left + Lattice_Len;
				rect.top = i*Lattice_Len;
				rect.bottom = rect.top + Lattice_Len;
				FillRect(hdc, &rect, hbrushBrickDoor);
			}
		}
	}
	DeleteObject(hbrushBrick);
	DeleteObject(hbrushStone);
	DeleteObject(hbrushBrickDoor);
}

void DrawMan(HDC hdc)
{
	HDC dcTemp;
	dcTemp = CreateCompatibleDC(NULL);
	HBITMAP hBitmap = (HBITMAP)LoadImage(NULL, _T("Images//Hero.bmp"), IMAGE_BITMAP, Lattice_Len,
		Lattice_Len, LR_LOADFROMFILE);
	HBITMAP hBitmap2 = (HBITMAP)LoadImage(NULL, _T("Images//HeroInverse.bmp"), IMAGE_BITMAP, Lattice_Len,
		Lattice_Len, LR_LOADFROMFILE);
	HBITMAP hBitmap3 = (HBITMAP)LoadImage(NULL, _T("Images//Bombing1.bmp"), IMAGE_BITMAP, Lattice_Len,
		Lattice_Len, LR_LOADFROMFILE);
	HBITMAP hBitmap4 = (HBITMAP)LoadImage(NULL, _T("Images//Cry.bmp"), IMAGE_BITMAP, Lattice_Len, Lattice_Len, LR_LOADFROMFILE);

	CreateCompatibleBitmap(dcTemp, Lattice_Len, Lattice_Len);
	//change the image of hero
	switch (g_Logic.m_heroPose)
	{
	case RightForward:
		SelectObject(dcTemp, hBitmap);
		break;
	case LeftForward:
		SelectObject(dcTemp, hBitmap2);
		break;
	case Facial1:
		SelectObject(dcTemp, hBitmap3);
		break;
	case Fail:
		SelectObject(dcTemp, hBitmap4);
		break;
	}
	//Draw man and its transparent
	if (g_Logic.m_heroPose != Fail)
	{
		TransparentBlt(hdc, g_Logic.m_fireManPos.pos.x, g_Logic.m_fireManPos.pos.y, Lattice_Len,
			Lattice_Len, dcTemp, 0, 0, Lattice_Len, Lattice_Len, HeroTransparentColor);
	}
	else
	{
		TransparentBlt(hdc, g_Logic.m_fireManPos.pos.x, g_Logic.m_fireManPos.pos.y, Lattice_Len,
			Lattice_Len, dcTemp, 0, 0, Lattice_Len, Lattice_Len, HeroFailTransparentColor);
	}
	DeleteObject(dcTemp);
	DeleteObject(hBitmap);
	DeleteObject(hBitmap2);
	DeleteObject(hBitmap3);
}
void DrawMonster(HDC hdc)
{
	HDC dcTemp;
	dcTemp = CreateCompatibleDC(NULL);
	HBITMAP hBitmap1 = (HBITMAP)LoadImage(NULL, _T("Images//Enemy1.bmp"), IMAGE_BITMAP, Lattice_Len, Lattice_Len, LR_LOADFROMFILE);
	CreateCompatibleBitmap(dcTemp, Lattice_Len, Lattice_Len);
	switch (g_Logic.m_curLevel)
	{
	case 0:
	case 1:
	case 2:
	case 3:
	case 4:
	case 5:
		SelectObject(dcTemp, hBitmap1);
		break;
	}
	for (MonsterList::iterator it = g_Logic.m_monsterList.begin(); it != g_Logic.m_monsterList.end(); it++)
	{
		TransparentBlt(hdc, it->pos.x, it->pos.y, Lattice_Len, Lattice_Len, dcTemp, 0, 0, Lattice_Len, Lattice_Len, MonsterTransparentColor);
	}
	DeleteObject(dcTemp);
	DeleteObject(hBitmap1);
}

void DrawBomb(HDC hdc)
{
	HDC dcTemp;
	dcTemp = CreateCompatibleDC(NULL);
	HBITMAP hBitmap = (HBITMAP)LoadImage(NULL, _T("Images//Bomb.bmp"), IMAGE_BITMAP, Lattice_Len, Lattice_Len, LR_LOADFROMFILE);
	CreateCompatibleBitmap(dcTemp, Lattice_Len, Lattice_Len);
	SelectObject(dcTemp, hBitmap);
	for (BombList::iterator it = g_Logic.m_bombList.begin(); it != g_Logic.m_bombList.end(); it++)
	{
		TransparentBlt(hdc, Lattice_Len*(it->j), Lattice_Len*(it->i), Lattice_Len, Lattice_Len,
			dcTemp, 0, 0, Lattice_Len, Lattice_Len, BombTransparentColor);
	}
	DeleteObject(dcTemp);
	DeleteObject(hBitmap);
}

void DrawFire(HDC hdc)
{
	HDC dcTemp;
	dcTemp = CreateCompatibleDC(NULL);
	CreateCompatibleBitmap(dcTemp, Lattice_Len, Lattice_Len);
	//anamition of fire
	HBITMAP hBitmapCross1 = (HBITMAP)LoadImage(NULL, _T("Images//Explosion1.bmp"), IMAGE_BITMAP, Lattice_Len,
		 Lattice_Len, LR_LOADFROMFILE);
	HBITMAP hBitmapVertical1 = (HBITMAP)LoadImage(NULL, _T("Images//Explosion1.bmp"), IMAGE_BITMAP, Lattice_Len,
		Lattice_Len, LR_LOADFROMFILE);

	HBITMAP hBitmapCross2 = (HBITMAP)LoadImage(NULL, _T("Images//Explosion2.bmp"), IMAGE_BITMAP, Lattice_Len,
		Lattice_Len, LR_LOADFROMFILE);
	HBITMAP hBitmapVertical2 = (HBITMAP)LoadImage(NULL, _T("Images//Explosion2.bmp"), IMAGE_BITMAP, Lattice_Len,
		Lattice_Len, LR_LOADFROMFILE);

	HBITMAP hBitmapCross3 = (HBITMAP)LoadImage(NULL, _T("Images//Explosion3.bmp"), IMAGE_BITMAP, Lattice_Len,
		Lattice_Len, LR_LOADFROMFILE);
	HBITMAP hBitmapVertical3 = (HBITMAP)LoadImage(NULL, _T("Images//Explosion3.bmp"), IMAGE_BITMAP, Lattice_Len,
		Lattice_Len, LR_LOADFROMFILE);

	HBITMAP hBitmapCross4 = (HBITMAP)LoadImage(NULL, _T("Images//Explosion4.bmp"), IMAGE_BITMAP, Lattice_Len,
		Lattice_Len, LR_LOADFROMFILE);
	HBITMAP hBitmapVertical4 = (HBITMAP)LoadImage(NULL, _T("Images//Explosion4.bmp"), IMAGE_BITMAP, Lattice_Len,
		Lattice_Len, LR_LOADFROMFILE);

	for (FireList::iterator it = g_Logic.m_fireList.begin(); it != g_Logic.m_fireList.end(); it++)
	{
		DWORD retention = GetTickCount() - it->fireTime;
		if ((retention >= 0 && retention <= FireTime / 5)||(retention > FireTime*4/5 && retention <= FireTime))
		{
			if (it->bCross) SelectObject(dcTemp, hBitmapCross1);
			else SelectObject(dcTemp, hBitmapVertical1);
		}
		if (retention >= FireTime/5 && retention <= FireTime*2/5)
		{
			if (it->bCross) SelectObject(dcTemp, hBitmapCross2);
			else SelectObject(dcTemp, hBitmapVertical2);
		}
		if (retention >= FireTime * 2 / 5 && retention <= FireTime * 3 / 5)
		{
			if (it->bCross) SelectObject(dcTemp, hBitmapCross3);
			else SelectObject(dcTemp, hBitmapVertical3);
		}
		if (retention >= FireTime * 3 / 5 && retention <= FireTime * 4 / 5)
		{
			if (it->bCross) SelectObject(dcTemp, hBitmapCross4);
			else SelectObject(dcTemp, hBitmapVertical4);
		}
		TransparentBlt(hdc, Lattice_Len*(it->j), Lattice_Len*(it->i), Lattice_Len, Lattice_Len, dcTemp, 0, 0, Lattice_Len,
			Lattice_Len, FireTransparentColor);
	}
	DeleteObject(dcTemp);
	DeleteObject(hBitmapCross1);
	DeleteObject(hBitmapVertical1);
	DeleteObject(hBitmapCross2);
	DeleteObject(hBitmapVertical2);
	DeleteObject(hBitmapCross3);
	DeleteObject(hBitmapVertical3);
	DeleteObject(hBitmapCross4);
	DeleteObject(hBitmapVertical4);
}

void DrawDoor(HDC hdc)
{
	if (g_Logic.m_doorPos.x == 0 && g_Logic.m_doorPos.y == 0)
	{
		return;
	}
	HDC dcTemp;
	dcTemp = CreateCompatibleDC(NULL);
	HBITMAP hBitmap = (HBITMAP)LoadImage(NULL, _T("Images//Door.bmp"), IMAGE_BITMAP, Lattice_Len, Lattice_Len, LR_LOADFROMFILE);
	CreateCompatibleBitmap(dcTemp, Lattice_Len, Lattice_Len);
	SelectObject(dcTemp, hBitmap);
	BitBlt(hdc, g_Logic.m_doorPos.x, g_Logic.m_doorPos.y, Lattice_Len, Lattice_Len, dcTemp, 0, 0, SRCCOPY);
	DeleteObject(dcTemp);
	DeleteObject(hBitmap);
}

void OnKeyDown(HWND hWnd, WPARAM wParam)
{
	if (g_Logic.m_bIsDown)
	{
		return;
	}
	switch (wParam)
	{
	case 'W':
	case 'S':
	case 'A':
	case 'D':
		g_Logic.m_bIsDown = TRUE;
		g_Logic.ManMove(wParam);

		RECT rect;
		GetClientRect(hWnd, &rect);
		InvalidateRect(hWnd, &rect, TRUE);
		UpdateWindow(hWnd);

		g_dwClickTime = GetTickCount();
		break;
	}
}

void OnKeyUp(HWND hWnd, WPARAM wParam)
{
	switch (wParam)
	{
	case 'W':
	case 'S':
	case 'A':
	case 'D':
		g_Logic.ManStop();
		break;
	case 'J'://put bomb
		//change facial
		g_Logic.m_heroPose = Facial1;//......................
		//how much time the new facial lasts
		g_Logic.m_fireManPos.Event1 = GetTickCount();
		//put the bomb
		g_Logic.PutBomb();
		break;

	}
}

void cLogic::ManStop()
{
	m_bIsDown = FALSE;
}
//Timer
void CALLBACK TimerProc(HWND hWnd, UINT nMsg, UINT nTimerid, DWORD dwTime)
{
	switch (nTimerid)
	{
	case 1:
		if (g_Logic.m_bIsDown && GetTickCount() - g_dwClickTime > TimeIntervalOfTyping)
		{
			//the default mov process is retained if key is pressed continuously
			g_Logic.MoveTo(g_Logic.m_curMoveDir, g_Logic.m_fireManPos.pos);
		}
		g_Logic.FireRun();	//explosion
		g_Logic.CheckFire();//check if the explosion is over and cancel it
		g_Logic.CheckDieBomb();	//check if bomb hurt sth
		g_Logic.CheckTouchMonster();	//check if hero touches the monster to lose health or lead to game over
		g_Logic.CheckNextLevel();	//check if present mission is completed
		g_Logic.MonsterAI();	//monster artificial intellegence

		RECT rect;
		GetClientRect(hWnd, &rect);
		InvalidateRect(hWnd, &rect, TRUE);
		UpdateWindow(hWnd);
	}
}
