// ThirdProject.cpp : 애플리케이션에 대한 진입점을 정의합니다.
//

/*

갈스 패닉 구현하기

*/

#include "vector"
#include "queue"
#include "stack"
#include "framework.h"
#include "ThirdProject.h"
#pragma comment(lib, "msimg32.lib")

using namespace std;

#define MAX_LOADSTRING 100
#define TIMER_01 1

enum State { BLANK, WALL, INSIDE, ROAD, BACK};
const int dy[8] = { -1, 0, 1, 0, -1, 1, 1, -1 };
const int dx[8] = { 0, 1, 0, -1 , 1, 1, -1 ,-1 };

POINT player;
POINT bossPos;

POINT start;
POINT minIdx, maxIdx;
int mapData[100][200];
POINT startWall;
vector<pair<int, int>> myWall;
vector<pair<int, int>> myRoad;
vector<vector<int>> visited;

bool isInside;
int _cnt = 0;

HBITMAP hFrontImage;
BITMAP bitFront;
HBITMAP hBackImage;
BITMAP bitBack;
void CreateBitmap();
void DrawBitmapDoubleBuffering(HWND hWnd, HDC hdc);
void DeleteBitmap();

void PlayerController();
void UpdateWall();
void Fillinside();
void _Fillinside(vector<vector<int>>& visited, int y, int x, int level);
void WallToInside();
bool IsInside();

//최적화 완료
void Test(HBRUSH hOldBrush, HDC hMemDC);

RECT        rectView;

// 전역 변수:
HINSTANCE hInst;                                // 현재 인스턴스입니다.
WCHAR szTitle[MAX_LOADSTRING];                  // 제목 표시줄 텍스트입니다.
WCHAR szWindowClass[MAX_LOADSTRING];            // 기본 창 클래스 이름입니다.

// 이 코드 모듈에 포함된 함수의 선언을 전달합니다:
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

    // TODO: 여기에 코드를 입력합니다.

    // 전역 문자열을 초기화합니다.
    LoadStringW(hInstance, IDS_APP_TITLE, szTitle, MAX_LOADSTRING);
    LoadStringW(hInstance, IDC_THIRDPROJECT, szWindowClass, MAX_LOADSTRING);
    MyRegisterClass(hInstance);

    // 애플리케이션 초기화를 수행합니다:
    if (!InitInstance (hInstance, nCmdShow))
    {
        return FALSE;
    }

    HACCEL hAccelTable = LoadAccelerators(hInstance, MAKEINTRESOURCE(IDC_THIRDPROJECT));

    MSG msg;

    // 기본 메시지 루프입니다:
    while (GetMessage(&msg, nullptr, 0, 0))
    {
        if (!TranslateAccelerator(msg.hwnd, hAccelTable, &msg))
        {
            TranslateMessage(&msg);
            DispatchMessage(&msg);
        }
    }
    //// 기본 메시지 루프입니다:
    //while (true)
    //{
    //    if (PeekMessage(&msg, NULL, 0, 0, PM_REMOVE))
    //    {
    //        if (msg.message == WM_QUIT)
    //            break;
    //        else
    //        {
    //            TranslateMessage(&msg);
    //            DispatchMessage(&msg);
    //        }
    //    }
    //    else
    //    {
    //        PlayerController();
    //    }
    //}
    return (int) msg.wParam;
}



//
//  함수: MyRegisterClass()
//
//  용도: 창 클래스를 등록합니다.
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
    wcex.hIcon          = LoadIcon(hInstance, MAKEINTRESOURCE(IDI_THIRDPROJECT));
    wcex.hCursor        = LoadCursor(nullptr, IDC_ARROW);
    wcex.hbrBackground  = (HBRUSH)(COLOR_WINDOW+1);
    wcex.lpszMenuName   = MAKEINTRESOURCEW(IDC_THIRDPROJECT);
    wcex.lpszClassName  = szWindowClass;
    wcex.hIconSm        = LoadIcon(wcex.hInstance, MAKEINTRESOURCE(IDI_SMALL));

    return RegisterClassExW(&wcex);
}

//
//   함수: InitInstance(HINSTANCE, int)
//
//   용도: 인스턴스 핸들을 저장하고 주 창을 만듭니다.
//
//   주석:
//
//        이 함수를 통해 인스턴스 핸들을 전역 변수에 저장하고
//        주 프로그램 창을 만든 다음 표시합니다.
//
BOOL InitInstance(HINSTANCE hInstance, int nCmdShow)
{
   hInst = hInstance; // 인스턴스 핸들을 전역 변수에 저장합니다.
   HWND hWnd = CreateWindowW(szWindowClass, szTitle, WS_OVERLAPPEDWINDOW,
      200, 150, 1420, 900, nullptr, nullptr, hInstance, nullptr);

   if (!hWnd)
   {
      return FALSE;
   }

   ShowWindow(hWnd, nCmdShow);
   UpdateWindow(hWnd);

   return TRUE;
}

//
//  함수: WndProc(HWND, UINT, WPARAM, LPARAM)
//
//  용도: 주 창의 메시지를 처리합니다.
//
//  WM_COMMAND  - 애플리케이션 메뉴를 처리합니다.
//  WM_PAINT    - 주 창을 그립니다.
//  WM_DESTROY  - 종료 메시지를 게시하고 반환합니다.
//
//
LRESULT CALLBACK WndProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam)
{
    switch (message)
    {
    case WM_CREATE:
        GetClientRect(hWnd, &rectView);
        CreateBitmap();
        SetTimer(hWnd, TIMER_01, 1, NULL);

        // >> : Setting
        player = { 50, 25 };
        bossPos = { 75, 42 };

        start = { 102, 102 };
        minIdx = { 0, 0 }; maxIdx = { 110, 58 };
        visited.resize(maxIdx.y, vector<int>(maxIdx.x, 0));
        {
            int sy = 25, ey = 30, sx = 50, ex = 60;
            for (int y = sy; y <= ey; y++)
            {
                for (int x = sx; x <= ex; x++)
                {
                    if (y == sy || y == ey || x == sx || x == ex)
                        mapData[y][x] = WALL;
                    else
                        mapData[y][x] = INSIDE;
                }
            }
            myWall.push_back({ sx, sy });
            myWall.push_back({ ex, sy });
            myWall.push_back({ ex, ey });
            myWall.push_back({ sx, ey });
        }
        // << :

        break;
    case WM_TIMER:
            switch (wParam)
        {
        case TIMER_01:
            PlayerController();
            InvalidateRect(hWnd, NULL, FALSE);
            break;
        }
        break;
    case WM_COMMAND:
        {
            int wmId = LOWORD(wParam);
            // 메뉴 선택을 구문 분석합니다:
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
            // TODO: 여기에 hdc를 사용하는 그리기 코드를 추가합니다...
            DrawBitmapDoubleBuffering(hWnd, hdc);
            
            //DeleteObject(hdc);
            EndPaint(hWnd, &ps);
        }
        break;
    case WM_DESTROY:
        KillTimer(hWnd, TIMER_01);  // 타이머 해제
        PostQuitMessage(0);
        DeleteBitmap();
        break;
 
    case WM_ERASEBKGND :
        return 1;
    default:
        return DefWindowProc(hWnd, message, wParam, lParam);
    }
    return 0;
}

// 정보 대화 상자의 메시지 처리기입니다.
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

void CreateBitmap()
{ 
    // >> : background image
    hBackImage = (HBITMAP)LoadImage(NULL, TEXT("images/BackGround.bmp"),
        IMAGE_BITMAP, 0, 0, LR_LOADFROMFILE | LR_CREATEDIBSECTION);
    if (hBackImage == NULL)
    {
        DWORD dwError = GetLastError();
        MessageBox(NULL, _T("back 이미지 로드 에러"), _T("에러"), MB_OK);
    }
    else
        GetObject(hBackImage, sizeof(BITMAP), &bitBack);

    hFrontImage = (HBITMAP)LoadImage(NULL, TEXT("images/front.bmp"),
        IMAGE_BITMAP, 0, 0, LR_LOADFROMFILE | LR_CREATEDIBSECTION);
    if (hFrontImage == NULL)
    {
        DWORD dwError = GetLastError();
        MessageBox(NULL, _T("front 이미지 로드 에러"), _T("에러"), MB_OK);
    }
    else
        GetObject(hFrontImage, sizeof(BITMAP), &bitFront);
}

void DrawBitmapDoubleBuffering(HWND hWnd, HDC hdc)
{
    HDC hDoubleBufferDC;
    HBITMAP hDoubleBufferImage = nullptr, hOldDoubleBufferBitmap;

    hDoubleBufferDC = CreateCompatibleDC(hdc);
    if (hDoubleBufferImage == NULL)
        hDoubleBufferImage = CreateCompatibleBitmap(hdc, rectView.right, rectView.bottom);
    hOldDoubleBufferBitmap = (HBITMAP)SelectObject(hDoubleBufferDC, hDoubleBufferImage);

    HDC hMemDC;
    HBITMAP hOldBitmap;
    int bx, by;

    {
        hMemDC = CreateCompatibleDC(hDoubleBufferDC);
        hOldBitmap = (HBITMAP)SelectObject(hMemDC, hBackImage);
        bx = bitBack.bmWidth;
        by = bitBack.bmHeight;

        BitBlt(hDoubleBufferDC, 100, 100, bx, by, hMemDC, 0, 0, SRCCOPY);

        SelectObject(hMemDC, hOldBitmap);
        DeleteDC(hMemDC);
    }

    hMemDC = CreateCompatibleDC(hDoubleBufferDC);
    hOldBitmap = (HBITMAP)SelectObject(hMemDC, hFrontImage);

    HPEN Null_Pen = (HPEN)GetStockObject(NULL_PEN);
    HPEN hOldPen = (HPEN)SelectObject(hMemDC, Null_Pen);
    HPEN hRedPen = CreatePen(PS_DOT, 2, RGB(255, 0, 0));
    HPEN hWhitePen = CreatePen(PS_DOT, 2, RGB(255, 255, 255));

    // 브러시 미리 생성
    HBRUSH magentaBrush = CreateSolidBrush(RGB(255, 0, 255));
    HBRUSH blackBrush = CreateSolidBrush(RGB(0, 0, 0));
    HBRUSH hOldBrush;

	if (!isInside)
	{
		// >> : MAGENTA 브러쉬로 폴리곤 그리기
		hOldBrush = (HBRUSH)SelectObject(hMemDC, magentaBrush);
		POINT* temp = new POINT[myWall.size()];
		for (int idx = 0; idx < myWall.size(); idx++)
		{
			temp[idx].x = myWall[idx].first * 11;
			temp[idx].y = myWall[idx].second * 11;
		}
		Polygon(hMemDC, temp, myWall.size());
		SelectObject(hMemDC, hOldBrush);
		delete[]temp;
		// << :
	}
    else
    {
		// >> : MAGENTA 브러쉬로 전체 색칠
        hOldBrush = (HBRUSH)SelectObject(hMemDC, magentaBrush);
        POINT temp[4] = { {0, 0}, {110 * 11, 0}, {110 * 11, 58 * 11}, {0, 58 * 11} };
		Polygon(hMemDC, temp, 4);
		SelectObject(hMemDC, hOldBrush);
		// << :

        // >> : BLACK 브러쉬로 폴리곤 그리기
        hOldBrush = (HBRUSH)SelectObject(hMemDC, blackBrush);
        POINT* temp1 = new POINT[myWall.size()];
        for (int idx = 0; idx < myWall.size(); idx++)
        {
            temp1[idx].x = myWall[idx].first * 11;
            temp1[idx].y = myWall[idx].second * 11;
        }
        Polygon(hMemDC, temp1, myWall.size());
        SelectObject(hMemDC, hOldBrush);
        delete[]temp1;
        DeleteObject(blackBrush);
        // << :
    }

    // >> : RED 펜으로 길 그리기
    {
        hOldPen = (HPEN)SelectObject(hMemDC, hRedPen);
        for (int idx = 1; idx < myRoad.size(); idx++)
        {
            MoveToEx(hMemDC, myRoad[idx - 1].first * 11, myRoad[idx - 1].second * 11, nullptr);
            LineTo(hMemDC, myRoad[idx].first * 11, myRoad[idx].second * 11);
        }
        SelectObject(hMemDC, hOldPen); //현재영역에 대한 펜을 원래 있던 펜으로 다시 적용
        DeleteObject(hRedPen); //사용이 끝난 펜 해제
    }
    // << :

    // >> : WHITE 펜으로 벽 그리기
    if (myWall.size())
    {
        hOldPen = (HPEN)SelectObject(hMemDC, hWhitePen);
        for (int idx = 1; idx < myWall.size(); idx++)
        {
            MoveToEx(hMemDC, myWall[idx - 1].first * 11, myWall[idx - 1].second * 11, nullptr);
            LineTo(hMemDC, myWall[idx].first * 11, myWall[idx].second * 11);
        }
        MoveToEx(hMemDC, myWall[myWall.size() - 1].first * 11, myWall[myWall.size() - 1].second * 11, nullptr);
        LineTo(hMemDC, myWall[0].first * 11, myWall[0].second * 11);

        SelectObject(hMemDC, hOldPen);
        DeleteObject(hWhitePen);
    }
    // << :

    TransparentBlt(hDoubleBufferDC, 100, 100, bx + 3, by + 3, hMemDC, 0, 0, bx, by, RGB(255, 0, 255));

    SelectObject(hMemDC, hOldPen);
    SelectObject(hMemDC, hOldBitmap);
    DeleteDC(hMemDC);

    // 브러시 해제
    DeleteObject(magentaBrush);
    
    BitBlt(hdc, 0, 0, rectView.right, rectView.bottom, hDoubleBufferDC, 0, 0, SRCCOPY);
    Ellipse(hdc, player.x * 11 + start.x - 10, player.y * 11 + start.y - 10, 
        player.x * 11 + start.x + 10, player.y * 11 + start.y + 10);
    Ellipse(hdc, bossPos.x * 11 + start.x - 10, bossPos.y * 11 + start.y - 10,
        bossPos.x * 11 + start.x + 10, bossPos.y * 11 + start.y + 10);

    SelectObject(hDoubleBufferDC, hOldDoubleBufferBitmap);
    DeleteDC(hDoubleBufferDC);

    if (hDoubleBufferImage != nullptr)
        DeleteObject(hDoubleBufferImage);  // 자원 해제
}

void DeleteBitmap()
{
    DeleteObject(hBackImage);
    DeleteObject(hFrontImage);
}

void PlayerController()
{
    DWORD newTime = GetTickCount64();
    static DWORD oldTime = newTime;

    if (newTime - oldTime < 5)
        return;

    oldTime = newTime;

    int canGo = 0;
    if (GetAsyncKeyState(VK_LCONTROL) & 0x8000)
        canGo = 1;
    else
    {
        //되돌아 갈 수 있도록 하는 코드
        if (myRoad.size())
        {
            mapData[myRoad.back().first][myRoad.back().second] = BLANK;
            player.x = myRoad.back().first;
            player.y = myRoad.back().second;
            myRoad.pop_back();
        }
    }
    if (GetAsyncKeyState(VK_LEFT) & 0x8000)
    {
        player.x--;
        if (!(mapData[player.y][player.x]|canGo) 
            || (mapData[player.y][player.x] == INSIDE)
            || (mapData[player.y][player.x] == ROAD)
            || (player.x <= 0))
            player.x++;
    }
    else if (GetAsyncKeyState(VK_RIGHT) & 0x8000)
    {
        player.x++;
        if (!(mapData[player.y][player.x] | canGo) 
            || (mapData[player.y][player.x] == INSIDE)
            || (mapData[player.y][player.x] == ROAD)
            || (player.x >= maxIdx.x - 1 ))
            player.x--;
    }
    else if (GetAsyncKeyState(VK_UP) & 0x8000)
    {
        player.y--;
        if (!(mapData[player.y][player.x] | canGo) 
            || (mapData[player.y][player.x] == INSIDE)
            || (mapData[player.y][player.x] == ROAD)
            || (player.y <= 0))
            player.y++;
    }
    else if (GetAsyncKeyState(VK_DOWN) & 0x8000)
    {
        player.y++;
        if (!(mapData[player.y][player.x] | canGo) 
            || (mapData[player.y][player.x] == INSIDE)
            || (mapData[player.y][player.x] == ROAD)
            || (player.y >= maxIdx.y - 1))
            player.y--;
    }
    else return;

    // 벽에 닿았을 때 처리
    if (mapData[player.y][player.x] == WALL)
    {
        int idx;
        for (idx = 0; idx < 4; idx++)
        {
            if (player.x + dx[idx] < 0 
                || player.y + dy[idx] < 0 
                || player.x + dx[idx] > maxIdx.x 
                || player.y + dy[idx] > maxIdx.y)
                continue;
            if (mapData[player.y + dy[idx]][player.x + dx[idx]] == ROAD)
                break;
        }
        if (idx == 4) 
            return;
        fill(visited.begin(), visited.end(), std::vector<int>(maxIdx.x, 0));
        Fillinside();
        WallToInside();
        UpdateWall();
        isInside = IsInside();

        myRoad.clear();
    }
    else
    {
        if (myRoad.empty())
        {
            for (int idx = 0; idx < 4; idx++)
            {
                if (mapData[player.y + dy[idx]][player.x + dx[idx]] != WALL)
                    continue;
                myRoad.push_back({ player.x + dx[idx],player.y + dy[idx] });
                startWall = { player.x + dx[idx],player.y + dy[idx] };
                break;
            }
        }
        mapData[player.y][player.x] = ROAD;
        myRoad.push_back({ player.x, player.y });
    }
}


void Fillinside()
{
    int level = 1;
    for (int y = 1; y < maxIdx.y; y++)
    {
        for (int x = 1; x < maxIdx.x; x++)
        {
            if (mapData[y][x] || visited[y][x]) continue;
            _Fillinside(visited, y, x, level++);
        }
    }
    for (int y = 1; y < maxIdx.y - 1; y++)
    {
        for (int x = 1; x < maxIdx.x - 1; x++)
        {
            if (visited[y][x] != visited[bossPos.y][bossPos.x] && mapData[y][x] == BLANK)
                mapData[y][x] = INSIDE;
            if(mapData[y][x] == ROAD)
                mapData[y][x] = WALL;
        }
    }
    
}
void _Fillinside(vector<vector<int>>& visited, int y, int x, int level)
{
    queue<pair<int, int>> q;
    q.push({ y, x });
    visited[y][x] = level;

    while (q.size())
    {
        pair<int, int> now = q.front();
        q.pop();
        for (int idx = 0; idx < 4; idx++)
        {
            int ny = now.first + dy[idx];
            int nx = now.second + dx[idx];

            if (ny < 0 || nx < 0 || ny >= maxIdx.y || nx >= maxIdx.x || mapData[ny][nx] || visited[ny][nx])
                continue;
            visited[ny][nx] = level;
            q.push({ ny, nx });
        }
    }
}

void WallToInside()
{
    for (int y = 0; y <= maxIdx.y; y++)
    {
        for (int x = 0; x <= maxIdx.x; x++)
        {
            if (mapData[y][x] != WALL) continue;
            int idx;
            for (idx = 0; idx < 8; idx++)
            {
                if (x + dx[idx] < 0 || y + dy[idx] < 0 || x + dx[idx]>maxIdx.x || y + dy[idx] > maxIdx.y)
                    continue;
                if (mapData[y + dy[idx]][x + dx[idx]] == BLANK)
                    break;
            }
            if (idx == 8) mapData[y][x] = INSIDE;
        }
    }
}
void _UpdateWall(int y, int x, int dir)
{
    if (dir != -1 && y == startWall.y && x == startWall.x)
        return; //다시 시작점으로 돌아온 케이스
    for (int i = 0; i < 4; i++)
    {
        int ny = y + dy[i];
        int nx = x + dx[i];

        if (mapData[ny][nx] != WALL || visited[ny][nx] != 0)
            continue;
        if (dir != i) myWall.push_back({ x, y });
        visited[ny][nx] = 1;
        _UpdateWall(ny, nx, i);
    }
}
void UpdateWall()
{
    myWall.clear();

    _UpdateWall(startWall.y, startWall.x, -1);

    //fill(visited.begin(), visited.end(), std::vector<int>(maxIdx.x, 0));
}
bool IsInside()
{
    int countLine = 0;
    for (int i = 1; i < myWall.size(); i++)
    {
        if ((myWall[i - 1].second > bossPos.y && myWall[i].second < bossPos.y) || (myWall[i - 1].second < bossPos.y && myWall[i].second > bossPos.y))
            countLine += bossPos.x < myWall[i - 1].first ? 1 : 0;
    }
    if ((myWall[0].second > bossPos.y && myWall[myWall.size()-1].second < bossPos.y) || (myWall[0].second < bossPos.y && myWall[myWall.size() - 1].second > bossPos.y))
        countLine += bossPos.x < myWall[0].first ? 1 : 0;
    
    return (countLine) % 2;
}

void Test(HBRUSH hOldBrush, HDC hMemDC)
{
    HBRUSH whiteBrush = CreateSolidBrush(RGB(255, 255, 255));
    HBRUSH redBrush = CreateSolidBrush(RGB(255, 0, 0));
    hOldBrush = (HBRUSH)SelectObject(hMemDC, whiteBrush);
    for (int y = 0; y <= maxIdx.y; y++)
    {
        for (int x = 0; x <= maxIdx.x; x++)
        {
            if (mapData[y][x] == WALL)
                Rectangle(hMemDC, x * 11 - 6, y * 11 - 6, x * 11 + 6, y * 11 + 6);
        }
    }
    SelectObject(hMemDC, hOldBrush);

    hOldBrush = (HBRUSH)SelectObject(hMemDC, redBrush);
    for (pair<int, int> node : myWall)
        Rectangle(hMemDC, node.first * 11 - 6, node.second * 11 - 6, node.first * 11 + 6, node.second * 11 + 6);
    SelectObject(hMemDC, hOldBrush);
}