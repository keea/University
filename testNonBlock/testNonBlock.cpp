﻿// testNonBlock.cpp : Defines the entry point for the application.
//

#include "stdafx.h"
#include "resource.h"
#include <winsock2.h>
#include "packet.h"
#include "CirQueue.h"
#include <time.h>
#include <map>
#include "UserInfoCtrl.h"

#define MAX_LOADSTRING 100
#define ID_EDIT 100

//로그인 정보 띄우기.
//->랜덤한 색상 가져오기.

//적은 글자를 화면에 띄우기.


CCirQueue g_Queue;

LONG			 g_OldEditFunc;
char              g_myStrID[20];
DWORD				g_myID;
HWND g_hWnd;

// Global Variables:
HINSTANCE hInst;								// current instance
TCHAR szTitle[MAX_LOADSTRING];								// The title bar text
TCHAR szWindowClass[MAX_LOADSTRING];								// The title bar text

// Foward declarations of functions included in this code module:
ATOM				MyRegisterClass(HINSTANCE hInstance);
BOOL				InitInstance(HINSTANCE, int);
LRESULT CALLBACK	WndProc(HWND, UINT, WPARAM, LPARAM);
LRESULT CALLBACK EditFunction(HWND hDlg, UINT message, WPARAM wParam, LPARAM lParam);
void Send(char *data, int size);
//void Draw(HWND hWnd, PAINTSTRUCT * ps);

//새로 추가한 코드들
UserInfoCtrl g_userInfoCtrl;
void Update();
bool isInputText = false;
char g_inputText[20];
HWND g_hEdit;
bool isLogin = false;
DWORD g_loginTime = 0;

int APIENTRY WinMain(HINSTANCE hInstance,
                     HINSTANCE hPrevInstance,
                     LPSTR     lpCmdLine,
                     int       nCmdShow)
{
 	// TODO: Place code here.
	MSG msg;
	HACCEL hAccelTable;

	// Initialize global strings
	LoadString(hInstance, IDS_APP_TITLE, szTitle, MAX_LOADSTRING);
	LoadString(hInstance, IDC_TESTNONBLOCK, szWindowClass, MAX_LOADSTRING);
	MyRegisterClass(hInstance);

	// Perform application initialization:
	if (!InitInstance (hInstance, nCmdShow)) 
	{
		return FALSE;
	}

	hAccelTable = LoadAccelerators(hInstance, (LPCTSTR)IDC_TESTNONBLOCK);

	// Main message loop:
	while (1) 
	{
		if(PeekMessage(&msg, NULL, 0, 0, PM_REMOVE))
		{
			if (msg.message == WM_QUIT)
				break;

			//if (!TranslateAccelerator(msg.hwnd, hAccelTable, &msg)) 
			{
				TranslateMessage(&msg);
				DispatchMessage(&msg);
			}
		}
		else //메시지가 없을 경우.
		{
			//InvalidateRect(g_hWnd, NULL, FALSE);
			Update();
		}
	}

	return msg.wParam;
}



//
//  FUNCTION: MyRegisterClass()
//
//  PURPOSE: Registers the window class.
//
//  COMMENTS:
//
//    This function and its usage is only necessary if you want this code
//    to be compatible with Win32 systems prior to the 'RegisterClassEx'
//    function that was added to Windows 95. It is important to call this function
//    so that the application will get 'well formed' small icons associated
//    with it.
//
ATOM MyRegisterClass(HINSTANCE hInstance)
{
	WNDCLASSEX wcex;

	wcex.cbSize = sizeof(WNDCLASSEX); 

	wcex.style			= CS_HREDRAW | CS_VREDRAW;
	wcex.lpfnWndProc	= (WNDPROC)WndProc;
	wcex.cbClsExtra		= 0;
	wcex.cbWndExtra		= 0;
	wcex.hInstance		= hInstance;
	wcex.hIcon			= LoadIcon(hInstance, (LPCTSTR)IDI_TESTNONBLOCK);
	wcex.hCursor		= LoadCursor(NULL, IDC_ARROW);
	wcex.hbrBackground	= (HBRUSH)(COLOR_WINDOW+1);
	wcex.lpszMenuName	= (LPCSTR)IDC_TESTNONBLOCK;
	wcex.lpszClassName	= szWindowClass;
	wcex.hIconSm		= LoadIcon(wcex.hInstance, (LPCTSTR)IDI_SMALL);

	return RegisterClassEx(&wcex);
}

//
//   FUNCTION: InitInstance(HANDLE, int)
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

   g_hWnd = CreateWindow(szWindowClass, szTitle, WS_OVERLAPPEDWINDOW,
      CW_USEDEFAULT, 0, CW_USEDEFAULT, 0, NULL, NULL, hInstance, NULL);

   if (!g_hWnd)
   {
      return FALSE;
   }

   ShowWindow(g_hWnd, nCmdShow);
   UpdateWindow(g_hWnd);

   return TRUE;
}

//
//  FUNCTION: WndProc(HWND, unsigned, WORD, LONG)
//
//  PURPOSE:  Processes messages for the main window.
//
//  WM_COMMAND	- process the application menu
//  WM_PAINT	- Paint the main window
//  WM_DESTROY	- post a quit message and return
//
//
SOCKET g_hSocket;
#define WM_SOCKET   WM_USER+1

#include <stdio.h>
// 소켓 함수 오류 출력
void err_display(int errcode)
{
	LPVOID lpMsgBuf;
	FormatMessage( 
		FORMAT_MESSAGE_ALLOCATE_BUFFER|
		FORMAT_MESSAGE_FROM_SYSTEM,
		NULL, errcode,
		MAKELANGID(LANG_NEUTRAL, SUBLANG_DEFAULT),
		(LPTSTR)&lpMsgBuf, 0, NULL);
	//printf("[오류] %s", (LPCTSTR)lpMsgBuf);
	LocalFree(lpMsgBuf);
}

LRESULT CALLBACK WndProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam)
{
	int wmId, wmEvent;
	PAINTSTRUCT ps;
	HDC hdc;
	TCHAR szHello[MAX_LOADSTRING];
	LoadString(hInst, IDS_HELLO, szHello, MAX_LOADSTRING);

	switch (message) 
	{
	case WM_SOCKET:
		{
			// 오류 발생 여부 확인
			if(WSAGETSELECTERROR(lParam))
			{
				err_display(WSAGETSELECTERROR(lParam));
				break;
			}

			// 메시지 처리
			switch(WSAGETSELECTEVENT(lParam))
			{
			case FD_READ:
				{
					#define BUFSIZE  1024

					char buf[1024];
					int ret = recv(wParam, buf, BUFSIZE, 0);
					if(ret == SOCKET_ERROR)
					{
						if(WSAGetLastError() != WSAEWOULDBLOCK)
						{							
						}
						break;
					}

					g_Queue.OnPutData(buf, ret);
					PACKETHEADER *pHeader = g_Queue.GetPacket();
					while (pHeader != NULL)
					{
						switch (pHeader->PktID)
						{
							case PKT_RPOINT:
							{
								RPOINT *pPoint = (RPOINT *)pHeader;
								HDC hDC = GetDC(hWnd);
								SetPixel(hDC, pPoint->x, pPoint->y, RGB(255, 0, 0));
								ReleaseDC(hWnd, hDC);
							}
							break;
							case PKT_REQ_LOGIN:
							{
								LOGIN *pLogin = (LOGIN *)pHeader;
								
								g_userInfoCtrl.AddUserInfo(*pHeader);

								//로그인 한 시간이 같으면 내 아이디로 지정
								if (g_loginTime == pLogin->time) {
									isLogin = true;
									g_userInfoCtrl.SetMyInfo(pLogin->userID);
									OutputDebugString("로그인 했다.");
								}

								//int x = rand() % 2000;
								//int y = rand() % 1000;

								//HDC hDC = GetDC(hWnd);
								//TextOut(hDC, x, y, pLogin->userStrID, strlen(pLogin->userStrID));
								//ReleaseDC(hWnd, hDC);

							}
							break;

							case PKT_REQ_LOGOUT: {
								LOGOUT * pLogout = (LOGOUT *)pHeader;
								g_userInfoCtrl.DelUserInfo(pLogout->userID);
							}
								break;
						}
						g_Queue.OnPopData(pHeader->PktSize);

						pHeader = g_Queue.GetPacket();
					}
				}
				break;
			case FD_CLOSE:
				{
					closesocket(wParam);
				}
				break;
			}
		}
		break;
	case WM_CREATE:
		{
			WSADATA wsaData;
			WORD	wVersionRequested;
			int     bOK;

			srand(time(NULL));

			wVersionRequested = MAKEWORD( 2, 2 );
			
			bOK = WSAStartup(wVersionRequested,&wsaData);

			g_hSocket = socket(AF_INET, SOCK_STREAM, 0);

			sockaddr_in addr;

			addr.sin_family = AF_INET;
			addr.sin_addr.s_addr = inet_addr("127.0.0.1");
			addr.sin_port = htons(30000);

			if (connect(g_hSocket, (struct sockaddr*) &addr, sizeof(addr)) == SOCKET_ERROR) 
			{
				return FALSE;
			}

			if(WSAAsyncSelect(g_hSocket, hWnd, WM_SOCKET, FD_READ | FD_CLOSE) != 0)
			{
				OutputDebugString("WSAAsyncSelect Error.......\n");

				closesocket(g_hSocket);				
			}
		}
		break;		

		case WM_KEYDOWN:
			{
				if (wParam == VK_RETURN)
				{
					isInputText = true;
					g_hEdit = CreateWindow("edit", NULL, WS_CHILD | WS_VISIBLE | WS_BORDER , -500, -500, 200, 25, hWnd, (HMENU)ID_EDIT, hInst, NULL);
					g_OldEditFunc = SetWindowLong(g_hEdit, GWL_WNDPROC, (LONG)EditFunction);
					SetFocus(g_hEdit);
				}

			}
			break;

		case WM_COMMAND:
		{
			switch (LOWORD(wParam))
			{
			case ID_EDIT:
				switch (HIWORD(wParam))
				{
				case EN_CHANGE:
					GetWindowText(g_hEdit, g_inputText, 20);
					break;
				}
			}
		}
		break;

		case WM_DESTROY: 
		{
			//로그아웃 했다는 것을 알린다.
			LOGOUT logout;
			logout.PktID = PKT_REQ_LOGOUT;
			logout.PktSize = sizeof(LOGOUT);
			Send((char *)&logout, logout.PktSize);

			PostQuitMessage(0);

			closesocket(g_hSocket);
		}
			
			break;
		default:
			return DefWindowProc(hWnd, message, wParam, lParam);
   }
   return 0;
}


LRESULT CALLBACK EditFunction(HWND hDlg, UINT message, WPARAM wParam, LPARAM lParam)
{
	
	switch (message)
	{
	case WM_KEYDOWN:
	{
		if (wParam == VK_RETURN)
		{
			GetWindowText(hDlg, g_myStrID, 20);
			SetWindowLong(hDlg, GWL_WNDPROC, (LONG)g_OldEditFunc);
			DestroyWindow(hDlg);

			if (!isLogin) 
			{
				LOGIN login;
				login.PktID = PKT_REQ_LOGIN;
				login.PktSize = sizeof(LOGIN);
				g_loginTime = time(NULL); //로그인 한 시간 초 단위로 얻기.
				login.time = g_loginTime; 

				strcpy(login.userStrID, g_myStrID);
				Send((char *)&login, login.PktSize);
			}
			else 
			{
				//게임에 관련된 처리를 한다.
				//공격 or 방어.


			}

			g_inputText[0] = '\0';
			isInputText = false;
		}
	}
	return TRUE;		
	}

	return ((WNDPROC)g_OldEditFunc)(hDlg, message, wParam, lParam);
}

void Send(char *data, int size)
{
	int ret = send(g_hSocket, data, size, 0);
	if (ret == SOCKET_ERROR)
	{
		if (WSAGetLastError() != WSAEWOULDBLOCK)
		{
			::MessageBox(NULL, "send failed!", "error", MB_OK);
		}
	}
}

//업데이트 함수
void Update() {
	static RECT bufferRT;
	GetClientRect(g_hWnd, &bufferRT);

	HDC hDcMain = GetDC(g_hWnd); //기존 DC
	HBITMAP hBitmap, OldBitmap;

	//랜더 로직
	HDC hdcBuffer = CreateCompatibleDC(hDcMain); //새로운 DC
	hBitmap = CreateCompatibleBitmap(hDcMain, bufferRT.right, bufferRT.bottom); // crt 규격대로 종이 생성
	OldBitmap = (HBITMAP)SelectObject(hdcBuffer, hBitmap); // 종이 교체

	//하얀색으로 변경
	FillRect(hdcBuffer, &bufferRT, (HBRUSH)GetStockObject(WHITE_BRUSH));
	
	//여기에 그리기 함수
	TextOut(hdcBuffer, 10, 10, g_inputText, strlen(g_inputText));
	g_userInfoCtrl.ShowUsers(bufferRT, hdcBuffer);


	BitBlt(hDcMain, 0, 0, bufferRT.right, bufferRT.bottom, hdcBuffer, 0, 0, SRCCOPY); //배껴그리기
	DeleteObject(SelectObject(hdcBuffer, OldBitmap)); // 종이 원래대로 한 후 제거
	DeleteDC(hdcBuffer);
}

//void Draw(HWND hWnd, PAINTSTRUCT * ps) {
//	static HDC hdc, MemDC;
//	static HBITMAP BackBit, oldBackBit;
//	static RECT bufferRT;
//	MemDC = BeginPaint(hWnd, ps);
//
//	GetClientRect(hWnd, &bufferRT);
//	hdc = CreateCompatibleDC(MemDC);
//	BackBit = CreateCompatibleBitmap(MemDC, bufferRT.right, bufferRT.bottom);
//	oldBackBit = (HBITMAP)SelectObject(hdc, BackBit);
//	PatBlt(hdc, 0, 0, bufferRT.right, bufferRT.bottom, WHITENESS);
//	
//	//여기에 그리기 코드
//	g_userInfoCtrl.ShowUsers(bufferRT, hdc);
//
//	/** 더블버퍼링 끝처리 입니다. **/
//	GetClientRect(hWnd, &bufferRT);
//	BitBlt(MemDC, 0, 0, bufferRT.right, bufferRT.bottom, hdc, 0, 0, SRCCOPY);
//	SelectObject(hdc, oldBackBit);
//	DeleteObject(BackBit);
//	DeleteDC(hdc);
//	EndPaint(hWnd, ps);
//}