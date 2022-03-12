// plab1.cpp : Defines the entry point for the application.
//

#include "framework.h"
#include "plab1.h"
#include<random>
#include<time.h>
#include "Resource.h"
//#include<list>
using namespace std;
#define MAX_LOADSTRING 100
#define TIMER_MOVE_CHILDREN 1
#define TIMER_SPAWN_CHILD 2
#define MAX_SQUERS 100
// Global Variables:
HINSTANCE hInst;                                // current instance
WCHAR szTitle[MAX_LOADSTRING];                  // The title bar text
WCHAR szWindowClass[MAX_LOADSTRING];            // the main window class name
WCHAR szChildClass[MAX_LOADSTRING];            // the main window class name
int howManySquers = 0;
HWND bottomMost;
bool gamePause = false;
int missedKeys = 0;
int wrongKeys = 0;
int maxWrongPlusMissedKyes = 10;
int correctKeys = 0;
enum mode { EASY, MEDIUM, HARD, GOD };
mode gameMode = mode::EASY;
struct node {

	HWND hWnd;
	char c;
	node* next;
};
node* head;


// Forward declarations of functions included in this code module:
ATOM RegisterChild(HINSTANCE hInstance);
ATOM MyRegisterClass(HINSTANCE hInstance);
BOOL InitInstanceChild(HINSTANCE, int, HWND);
BOOL InitInstance(HINSTANCE, int);
void UpdateTitleBar(HWND hWnd);
BOOL TimerMoveWindow(HWND hWnd, LPARAM param);
LRESULT CALLBACK WndProcChild(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam);
void SpawnChild(HWND hWnd, UINT msg, UINT_PTR id, DWORD time);
BOOL CALLBACK destroyWindow(HWND hWnd, LPARAM lParam);
void NewGame(HWND hWnd);
void DelSpecificWindow(HWND hWnd, TCHAR c);
LRESULT CALLBACK WndProc(HWND, UINT, WPARAM, LPARAM);
INT_PTR CALLBACK About(HWND, UINT, WPARAM, LPARAM);
void NewGame(HWND hWnd);

int APIENTRY wWinMain(_In_ HINSTANCE hInstance, _In_opt_ HINSTANCE hPrevInstance, _In_ LPWSTR lpCmdLine, _In_ int nCmdShow)
{
	UNREFERENCED_PARAMETER(hPrevInstance);
	UNREFERENCED_PARAMETER(lpCmdLine);

	// TODO: Place code here.

	// Initialize global strings
	LoadStringW(hInstance, IDS_APP_TITLE, szTitle, MAX_LOADSTRING);
	LoadStringW(hInstance, IDC_PLAB1, szWindowClass, MAX_LOADSTRING);
	MyRegisterClass(hInstance);
	RegisterChild(hInstance);

	// Perform application initialization:
	if (!InitInstance(hInstance, nCmdShow))
	{
		return FALSE;
	}

	HACCEL hAccelTable = LoadAccelerators(hInstance, MAKEINTRESOURCE(IDC_PLAB1));

	MSG msg;
	srand(time(NULL));
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

ATOM RegisterChild(HINSTANCE hInstance)
{
	WNDCLASSEXW wcex;

	wcex.cbSize = sizeof(WNDCLASSEX);

	wcex.style = CS_HREDRAW | CS_VREDRAW;
	wcex.lpfnWndProc = WndProcChild;
	wcex.cbClsExtra = 0;
	wcex.cbWndExtra = 0;
	wcex.hInstance = hInstance;
	wcex.hIcon = LoadIcon(hInstance, MAKEINTRESOURCE(IDI_PLAB1));
	wcex.hCursor = LoadCursor(nullptr, IDC_ARROW);
	wcex.hbrBackground = CreateSolidBrush(RGB(0, 0, 0));
	wcex.lpszMenuName = MAKEINTRESOURCEW(IDC_PLAB1);
	wcex.lpszClassName = L"childClass";
	wcex.hIconSm = LoadIcon(wcex.hInstance, MAKEINTRESOURCE(IDI_SMALL));

	return RegisterClassExW(&wcex);
}

ATOM MyRegisterClass(HINSTANCE hInstance)
{
	WNDCLASSEXW wcex;

	wcex.cbSize = sizeof(WNDCLASSEX);

	wcex.style = CS_HREDRAW | CS_VREDRAW;
	wcex.lpfnWndProc = WndProc;
	wcex.cbClsExtra = 0;
	wcex.cbWndExtra = 0;
	wcex.hInstance = hInstance;
	wcex.hIcon = LoadIcon(hInstance, MAKEINTRESOURCE(IDI_PLAB1));
	wcex.hCursor = LoadCursor(nullptr, IDC_ARROW);
	wcex.hbrBackground = CreateSolidBrush(RGB(100, 150, 100));
	wcex.lpszMenuName = MAKEINTRESOURCEW(IDC_PLAB1);
	wcex.lpszClassName = szWindowClass;
	wcex.hIconSm = LoadIcon(wcex.hInstance, MAKEINTRESOURCE(IDI_SMALL));

	return RegisterClassExW(&wcex);
}

BOOL InitInstanceChild(HINSTANCE hInstance, int nCmdShow, HWND hWnd)
{
	//hInst = hInstance; // Store instance handle in our global variable
	if (howManySquers >= MAX_SQUERS)
		return FALSE;
	howManySquers++;
	RECT rc;
	GetClientRect(hWnd, &rc);
	HWND child = CreateWindowW(L"childClass", L"child", WS_CHILD | WS_VISIBLE,
		rand() % ((rc.right - rc.left) - 25), rc.top, 25, 25, hWnd, nullptr, hInst, nullptr);
	if (!hWnd)
		return FALSE;

	PAINTSTRUCT ps;
	HDC hdc = BeginPaint(child, &ps);
	int c = rand() % 26 + 97 + '\0';
	//char cc = c + '\0';
	TCHAR s[4];
	const wchar_t len = 4;
	swprintf(s, len, L"%c", c);
	//RECT rc;
	GetClientRect(child, &rc);
	SetBkMode(hdc, TRANSPARENT);
	SetTextColor(hdc, RGB(255, 255, 255));
	DrawText(hdc, s, (int)_tcslen(s), &rc, DT_CENTER | DT_VCENTER | DT_SINGLELINE);
	EndPaint(child, &ps);

	node* newWin = new node();
	newWin->c = c;
	newWin->hWnd = child;
	newWin->next = head;
	head = newWin;
	ShowWindow(child, nCmdShow);
	UpdateWindow(child);

	return TRUE;
}

BOOL InitInstance(HINSTANCE hInstance, int nCmdShow)
{
	hInst = hInstance; // Store instance handle in our global variable
	int y = GetSystemMetrics(SM_CYSCREEN);
	int x = GetSystemMetrics(SM_CXSCREEN);
	HWND hWnd = CreateWindowW(szWindowClass, szTitle,
		WS_OVERLAPPED | WS_CAPTION | WS_SYSMENU | WS_MINIMIZEBOX | WS_MAXIMIZEBOX,
		x / 4, y / 4, x / 2, y / 2, nullptr, nullptr, hInstance, nullptr);
	SetWindowTextA(hWnd, "Keyboard Master @Jan Cichomski");
	if (!hWnd)
	{
		return FALSE;
	}

	ShowWindow(hWnd, nCmdShow);
	UpdateWindow(hWnd);

	return TRUE;
}

void UpdateTitleBar(HWND hWnd) {
	const int bufSize = 256;
	TCHAR buf[bufSize];
	_stprintf_s(buf, bufSize,
		_T("Keyboard Master @Jan Cichomski Missed: %d, Wrong keys: %d, Correct keys: %d, Remaining lives: %d"),
		missedKeys, wrongKeys, correctKeys, maxWrongPlusMissedKyes - missedKeys - wrongKeys);
	SetWindowText(hWnd, buf);
	if (maxWrongPlusMissedKyes - missedKeys - wrongKeys <= 0) {
		wrongKeys--;
		memset((void*)buf, '\0', 256);
		swprintf_s(buf, bufSize, (L"Your score: %d"), correctKeys);
		int ret = MessageBox(hWnd, buf, L"GAME OVER!!!", MB_RETRYCANCEL);
		if (ret == IDCANCEL) {
			DestroyWindow(hWnd);
		}
		else {
			NewGame(hWnd);
		}
	}
	
}

BOOL TimerMoveWindow(HWND hWnd, LPARAM param) {
	if (gamePause == true) return false;
	RECT rc;
	RECT parentPosition;

	GetWindowRect(hWnd, &rc);
	GetWindowRect(GetParent(hWnd), &parentPosition);

	if (parentPosition.bottom < rc.bottom) {
		howManySquers--;
		missedKeys++;
		UpdateTitleBar(GetParent(hWnd));
		DestroyWindow(hWnd);
		return TRUE;
	}
	GetClientRect(hWnd, &rc);
	MapWindowPoints(hWnd, GetParent(hWnd), (LPPOINT)&rc, 2);
	MoveWindow(hWnd, rc.left, rc.top + 2, rc.right - rc.left, rc.bottom - rc.top, true);
	//UpdateWindow(hWnd);
	InvalidateRect(hWnd, NULL, TRUE);
	return TRUE;
}

LRESULT CALLBACK WndProcChild(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam)
{
	int minMoveTime = 40;
	switch (message)
	{
	case WM_CREATE:
	{
		//SetFocus(GetParent(hWnd));
		switch (gameMode)
		{
		case EASY:
		{
			int y = GetSystemMetrics(SM_CYSCREEN);
			int x = GetSystemMetrics(SM_CXSCREEN);
			double a = 6.0 / 10;
			double b = 8.0 / 10;
			double xp = (1 - a) / 2 * x;
			double yp = (1 - b) / 2 * y;
			SetWindowPos(GetParent(hWnd), HWND_TOP, xp, yp, x * a, y * b, SWP_SHOWWINDOW);
			minMoveTime = 40;
			maxWrongPlusMissedKyes = 10;
			break;
		}
		case MEDIUM:
		{
			int y = GetSystemMetrics(SM_CYSCREEN);
			int x = GetSystemMetrics(SM_CXSCREEN);
			double a = 6.0 / 10;
			double b = 7.0 / 10;
			float xp = (1 - a) / 2 * x;
			float yp = (1 - b) / 2 * y;
			SetWindowPos(GetParent(hWnd), HWND_TOP, xp, yp, x * a, y * b, SWP_SHOWWINDOW);
			minMoveTime = 30;
			maxWrongPlusMissedKyes = 7;
			break;
		}
		case HARD:
		{
			int y = GetSystemMetrics(SM_CYSCREEN);
			int x = GetSystemMetrics(SM_CXSCREEN);
			double a = 6.0 / 10;
			double b = 6.0 / 10;
			float xp = (1 - a) / 2 * x;
			float yp = (1 - b) / 2 * y;
			SetWindowPos(GetParent(hWnd), HWND_TOP, xp, yp, x * a, y * b, SWP_SHOWWINDOW);
			minMoveTime = 20;
			maxWrongPlusMissedKyes = 4;
			break;
		}
		break;
		case GOD:
		{
			int y = GetSystemMetrics(SM_CYSCREEN);
			int x = GetSystemMetrics(SM_CXSCREEN);
			double a = 6.0 / 10;
			double b = 5.0 / 10;
			float xp = (1 - a) / 2 * x;
			float yp = (1 - b) / 2 * y;
			SetWindowPos(GetParent(hWnd), HWND_TOP, xp, yp, x * a, y * b, SWP_SHOWWINDOW);
			minMoveTime = 10;
			maxWrongPlusMissedKyes = 1;
			break;
		}
		default:
			minMoveTime = 40;
			break;
		}
		SetTimer(hWnd, 1, rand() % minMoveTime + 5, (TIMERPROC)TimerMoveWindow);
		break;
	}
	case WM_COMMAND:
	{
		int wmId = LOWORD(wParam);
		// Parse the menu selections:
		switch (wmId)
		{

		default:
			return DefWindowProc(hWnd, message, wParam, lParam);
		}
	}
	break;
	case WM_PAINT:
	{
		PAINTSTRUCT ps;
		HDC hdc = BeginPaint(hWnd, &ps);
		node* p = head;
		char c='\0';
		while (p) {
			if (p->hWnd == hWnd)
				c = p->c;
			p = p->next;
		}
		TCHAR s[4];
		const wchar_t len = 4;
		swprintf(s, len, L"%c", c - 32);
		RECT rc;
		GetClientRect(hWnd, &rc);
		SetBkMode(hdc, TRANSPARENT);
		SetTextColor(hdc, RGB(255, 255, 255));
		DrawText(hdc, s, (int)_tcslen(s), &rc, DT_CENTER | DT_VCENTER | DT_SINGLELINE);
		EndPaint(hWnd, &ps);
		break;
	}
	/*break;
	case WM_DESTROY:
		PostQuitMessage(0);
		break;*/
	default:
		return DefWindowProc(hWnd, message, wParam, lParam);
	}
	return 0;
}

//BOOL CALLBACK FindBottomMostSquare(HWND hWnd, LPARAM lParam) {
//	//MessageBox(NULL, NULL, NULL, NULL);
//	RECT bm;
//	RECT curr;
//	//GetWindowRect(hWnd, &parentPosition);
//	GetWindowRect(hWnd, &curr);
//	GetWindowRect(bottomMost, &bm);
//	if (curr.bottom > bm.bottom)
//		bottomMost = hWnd;
//	return TRUE;
//}

void SpawnChild(HWND hWnd, UINT msg, UINT_PTR id, DWORD time) {
	if (gamePause == true) return;
	InitInstanceChild(hInst, 1, hWnd);
	SetTimer(hWnd, TIMER_SPAWN_CHILD, rand() % 701 + 300, (TIMERPROC)SpawnChild);
}

BOOL CALLBACK destroyWindow(HWND hWnd, LPARAM lParam) {
	DestroyWindow(hWnd);
	return TRUE;
}

void NewGame(HWND hWnd) {
	EnumChildWindows(hWnd, (WNDENUMPROC)destroyWindow, NULL);
	missedKeys = 0;
	wrongKeys = 0;
	correctKeys = 0;
	UpdateTitleBar(hWnd);
}

void DelSpecificWindow(HWND hWnd, TCHAR c) {
	const int bufSize = 256;
	TCHAR buf[bufSize];
	_stprintf_s(buf, bufSize, _T(" WM_CHAR : %c"), (TCHAR)c);
	node* p = head;
	node* pBM = nullptr;
	RECT bm;
	RECT pp;
	while (p->next) {
		if (p->next->c == c) {
			if (pBM == nullptr)
				pBM = p;
			else {
				GetWindowRect(p->next->hWnd, &pp);
				GetWindowRect(pBM->next->hWnd, &bm);
				if (pp.bottom > bm.bottom)
					pBM = p;
			}
		}
		p = p->next;
	}
	if (pBM != nullptr) {
		DestroyWindow(pBM->next->hWnd);
		pBM->next = pBM->next->next;
		correctKeys++;
	}
	else {
		wrongKeys++;
		UpdateTitleBar(hWnd);
	}
}

LRESULT CALLBACK WndProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam)
{
	const int bufSize = 256;
	TCHAR buf[bufSize];
	switch (message)
	{
	case WM_CREATE:
		//SetFocus(hWnd);
		//InitInstanceChild(nullptr, SW_NORMAL, hWnd);
		//SetTimer(hWnd, TIMER_MOVE_CHILDREN, 400, (TIMERPROC)MoveChildren);
		SetTimer(hWnd, TIMER_SPAWN_CHILD, 0, (TIMERPROC)SpawnChild);
		break;

		/*case WM_GETMINMAXINFO: {
		int y = GetSystemMetrics(SM_CYSCREEN);
		int x = GetSystemMetrics(SM_CXSCREEN);
		MINMAXINFO* minMaxInfo = (MINMAXINFO*)lParam;
		minMaxInfo->ptMaxSize.x = minMaxInfo->ptMaxTrackSize.x = x / 2;
		minMaxInfo->ptMaxSize.y = minMaxInfo->ptMaxTrackSize.y = y / 2;
		break;

	}*/
	case WM_PARENTNOTIFY:
	{
		UpdateTitleBar(hWnd);
	}
	break;
	case WM_CHAR:
		_stprintf_s(buf, bufSize, _T(" WM_CHAR : %c"), (TCHAR)wParam);
		DelSpecificWindow(hWnd, (TCHAR)wParam);
		//SetWindowText(hWnd, buf);
		break;
	case WM_TIMER:
		//MessageBox(hWnd, NULL, NULL, NULL);
	/*switch (wParam) {
	case TIMER_MOVE_CHILDREN:

		return 0;
	case TIMER_SPAWN_CHILD:

		KillTimer(hWnd, TIMER_SPAWN_CHILD);
		return 0;
	}*/
	//MessageBox(hWnd, L"df", NULL, NULL);
	//EnumChildWindows(hWnd, (WNDENUMPROC)enumProc, 1);
		break;
	case WM_COMMAND:
	{
		int wmId = LOWORD(wParam);
		// Parse the menu selections:
		switch (wmId)
		{
		case ID_GAMEMODE_EASY:

			gameMode = EASY;
			NewGame(hWnd);
			break;
		case ID_GAMEMODE_MEDIUM:
			gameMode = MEDIUM;
			NewGame(hWnd);
			break;
		case ID_GAMEMODE_HARD:
			gameMode = HARD;
			NewGame(hWnd);
			break;
		case ID_GAMEMODE_AREYOUAGOD:
			gameMode = GOD;
			NewGame(hWnd);
			break;
		case ID_ESC:
		case ID_FILE_PAUSE:

			if (gamePause == false) gamePause = true;
			else gamePause = false;
			break;

			//case ID_SPACE:
			//	//MessageBox(NULL, NULL, NULL, NULL);
			//	if (gamePause == true) break;
			//	DeleteBottomMostSquare(hWnd);
			//	break;
		case ID_FILE_NEWGAME:
		case ID_F2:
			NewGame(hWnd);

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
	case WM_SETFOCUS:
		gamePause = false;
		break;
	case WM_KILLFOCUS:
		gamePause = true;
		break;
	case WM_PAINT:
	{
		PAINTSTRUCT ps;
		HDC hdc = BeginPaint(hWnd, &ps);
		// TODO: Add any drawing code that uses hdc here...
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
