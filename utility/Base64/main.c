#include <windows.h>
#include <windowsx.h>

#include "resource.h"
#include "../../src/base64.h"

static LRESULT CALLBACK WindowProcedure(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam);
static BOOL OnCreate(HWND, LPCREATESTRUCT);                                     	// WM_CREATE
static void OnCommand(HWND, int, HWND, UINT);                                      	// WM_COMMAND
static void OnPaint(HWND);                                                        	// WM_PAINT
static void OnGetMinMaxInfo(HWND, LPMINMAXINFO);                                    // WM_GETMINMAXIINFO
static void OnSize(HWND, UINT, int, int);                                           // WM_SIZE
static void OnDestroy(HWND);                                                        // WM_DESTROY

#define IDC_TEXT 1001
#define IDC_BASE64 1002
#define IDC_TEXT_TO_BASE64 1003
#define IDC_BASE64_TO_TEXT 1004

static HBRUSH g_hBackgroundBrush = NULL;

static HFONT g_hTitleFont = NULL;
static HFONT g_hDefaultFont = NULL;


// [WinMain]: entry point.
int WINAPI WinMain(
    HINSTANCE hInstance,
    HINSTANCE hPrevInstance,
    LPSTR lpCmdLine,
    int nShowCmd
)
{
    const char szWindowName[] = "Base64 Encode/Decode Utility";
    const char szClassName[] = "__base__class__";

    DWORD dwStyle = WS_OVERLAPPEDWINDOW;
    DWORD dwExStyle = WS_EX_APPWINDOW;

    RECT rc;
    SetRect(&rc, 0, 0, 700, 500);
    AdjustWindowRectEx(&rc, dwStyle, FALSE, dwExStyle);

    const int iWindowWidth = rc.right - rc.left;
    const int iWindowHeight = rc.bottom - rc.top;

    SystemParametersInfo(SPI_GETWORKAREA, 0, &rc, 0);

    WNDCLASSEX wcex;
    memset(&wcex, 0, sizeof(WNDCLASSEX));

    wcex.cbSize = sizeof(WNDCLASSEX);
    wcex.style = CS_HREDRAW | CS_VREDRAW;
    wcex.lpfnWndProc = WindowProcedure;
    wcex.cbClsExtra = 0;
    wcex.cbWndExtra = 0;
    wcex.hInstance = hInstance;
    wcex.hIcon = LoadIcon(hInstance, MAKEINTRESOURCE(IDI_ICON1));
    wcex.hCursor = LoadCursor(NULL, IDC_ARROW);
    wcex.hbrBackground = (HBRUSH)(COLOR_WINDOW + 1);
    wcex.lpszMenuName = NULL;
    wcex.lpszClassName = szClassName;
    wcex.hIconSm = LoadIcon(hInstance, MAKEINTRESOURCE(IDI_ICON1));

    if (!RegisterClassEx(&wcex))
    {
        MessageBox(NULL, "Window Registration Failed.", "Error!", MB_OK | MB_ICONERROR | MB_TOPMOST);
        return 1;
    }

    HWND hWnd = CreateWindowEx(
        dwExStyle,
        szClassName,
        szWindowName,
        dwStyle,
        (GetSystemMetrics(SM_CXSCREEN) >> 1) - (iWindowWidth >> 1),
        ((rc.bottom - rc.top) >> 1) - (iWindowHeight >> 1),
        iWindowWidth,
        iWindowHeight,
        HWND_DESKTOP,
        NULL,
        hInstance,
        NULL);

    if (!hWnd)
    {
        MessageBox(NULL, "Window Creation Failed.", "Error!", MB_OK | MB_ICONERROR | MB_TOPMOST);
        UnregisterClass(szClassName, hInstance);
        return 1;
    }

    ShowWindow(hWnd, SW_SHOWNORMAL);
    UpdateWindow(hWnd);

    MSG msg;
    BOOL bRet;
    while ((bRet = GetMessage(&msg, NULL, 0, 0)) != 0)
    {
        if (bRet == -1)
        {
            break;
        }
        else
        {
            DispatchMessage(&msg);
            TranslateMessage(&msg);
        }
    }

    UnregisterClass(szClassName, hInstance);
    return (int)msg.wParam;
}
// [/WinMain]


// [WindowProcedure]:
static LRESULT CALLBACK WindowProcedure(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam)
{
    switch (msg)
    {
        HANDLE_MSG(hWnd, WM_CREATE, OnCreate);
        HANDLE_MSG(hWnd, WM_COMMAND, OnCommand);
        HANDLE_MSG(hWnd, WM_PAINT, OnPaint);
        HANDLE_MSG(hWnd, WM_GETMINMAXINFO, OnGetMinMaxInfo);
        HANDLE_MSG(hWnd, WM_SIZE, OnSize);
        HANDLE_MSG(hWnd, WM_DESTROY, OnDestroy);

    case WM_ERASEBKGND:
        return 1;
    }
    return DefWindowProc(hWnd, msg, wParam, lParam);
}
// [/WindowProcedure]


// [OnCreate]: WM_CREATE
static BOOL OnCreate(HWND hWnd, LPCREATESTRUCT lpcs)
{
    g_hBackgroundBrush = CreateSolidBrush(RGB(210, 220, 220));

    g_hTitleFont = CreateFont(16, 0, 0, 0, FW_BOLD, FALSE, FALSE, FALSE, DEFAULT_CHARSET,
        OUT_DEFAULT_PRECIS, CLIP_DEFAULT_PRECIS, DEFAULT_QUALITY, DEFAULT_PITCH, "Arial");

    g_hDefaultFont = CreateFont(15, 0, 0, 0, FW_NORMAL, FALSE, FALSE, FALSE, DEFAULT_CHARSET,
        OUT_DEFAULT_PRECIS, CLIP_DEFAULT_PRECIS, DEFAULT_QUALITY, DEFAULT_PITCH, "Arial");

    CreateWindowEx(0, "edit", "",
        WS_CHILD | WS_VISIBLE | WS_BORDER | WS_HSCROLL | WS_VSCROLL |
        ES_LEFT | ES_AUTOVSCROLL | ES_AUTOHSCROLL | ES_MULTILINE | ES_WANTRETURN,
        0, 0, 0, 0, hWnd, (HMENU)IDC_TEXT, lpcs->hInstance, NULL);
    SendMessage(GetDlgItem(hWnd, IDC_TEXT), WM_SETFONT, (WPARAM)g_hDefaultFont, 0L);

    CreateWindowEx(0, "edit", "",
        WS_CHILD | WS_VISIBLE | WS_BORDER | WS_HSCROLL | WS_VSCROLL |
        ES_LEFT | ES_AUTOVSCROLL | ES_AUTOHSCROLL | ES_MULTILINE | ES_WANTRETURN,
        0, 0, 0, 0, hWnd, (HMENU)IDC_BASE64, lpcs->hInstance, NULL);
    SendMessage(GetDlgItem(hWnd, IDC_BASE64), WM_SETFONT, (WPARAM)g_hDefaultFont, 0L);

    CreateWindowEx(0, "button", "", WS_CHILD | WS_VISIBLE | BS_PUSHBUTTON | BS_BITMAP,
        0, 0, 0, 0, hWnd, (HMENU)IDC_TEXT_TO_BASE64, lpcs->hInstance, NULL);
    SendMessage(GetDlgItem(hWnd, IDC_TEXT_TO_BASE64), BM_SETIMAGE, (WPARAM)IMAGE_BITMAP,
        (LPARAM)LoadBitmap(lpcs->hInstance, MAKEINTRESOURCE(IDB_ARROW_RIGHT)));

    CreateWindowEx(0, "button", "", WS_CHILD | WS_VISIBLE | BS_PUSHBUTTON | BS_BITMAP,
        0, 0, 0, 0, hWnd, (HMENU)IDC_BASE64_TO_TEXT, lpcs->hInstance, NULL);
    SendMessage(GetDlgItem(hWnd, IDC_BASE64_TO_TEXT), BM_SETIMAGE, (WPARAM)IMAGE_BITMAP,
        (LPARAM)LoadBitmap(lpcs->hInstance, MAKEINTRESOURCE(IDB_ARROW_LEFT)));

    return TRUE;
}
// [/OnCreate]


// [OnCommand]: WM_COMMAND
static void OnCommand(HWND hWnd, int id, HWND hWndCtl, UINT codeNotify)
{
    switch (id)
    {
    case IDC_TEXT_TO_BASE64:
    {
        SetWindowText(GetDlgItem(hWnd, IDC_BASE64), "\0");

        int iTextLength = GetWindowTextLength(GetDlgItem(hWnd, IDC_TEXT));
        if (iTextLength > 0)
        {
            char* szBuff = (char*)malloc(iTextLength + 2);
            if (szBuff)
            {
                memset(szBuff, 0, iTextLength + 2);
                if (GetWindowText(GetDlgItem(hWnd, IDC_TEXT), szBuff, iTextLength + 1) > 0)
                {
                    int iBase64Length = base64_encode((unsigned char*)szBuff, iTextLength, NULL);
                    if (iBase64Length > 0)
                    {
                        char* szOutBuff = (char*)malloc(iBase64Length + 2);
                        if (szOutBuff)
                        {
                            memset(szOutBuff, 0, iBase64Length + 2);
                            if (base64_encode((unsigned char*)szBuff, iTextLength, szOutBuff) > 0)
                            {
                                SetWindowText(GetDlgItem(hWnd, IDC_BASE64), szOutBuff);
                            }
                            free(szOutBuff);
                        }
                        else
                        {
                            MessageBox(hWnd, "Failed to allocate memory.", "Error", MB_OK | MB_ICONERROR);
                        }
                    }
                }
                free(szBuff);
            }
            else
            {
                MessageBox(hWnd, "Failed to allocate memory.", "Error", MB_OK | MB_ICONERROR);
            }
        }
    }
    break;

    case IDC_BASE64_TO_TEXT:
    {
        SetWindowText(GetDlgItem(hWnd, IDC_TEXT), "\0");

        int iTextLength = GetWindowTextLength(GetDlgItem(hWnd, IDC_BASE64));
        if (iTextLength > 0)
        {
            char* szBuff = (char*)malloc(iTextLength + 2);
            if (szBuff)
            {
                memset(szBuff, 0, iTextLength + 2);
                if (GetWindowText(GetDlgItem(hWnd, IDC_BASE64), szBuff, iTextLength + 1) > 0)
                {
                    int iBase64Length = base64_decode(szBuff, iTextLength, NULL);
                    if (iBase64Length > 0)
                    {
                        char* szOutBuff = (char*)malloc(iBase64Length + 2);
                        if (szOutBuff)
                        {
                            memset(szOutBuff, 0, iBase64Length + 2);
                            if (base64_decode(szBuff, iTextLength, (unsigned char*)szOutBuff) > 0)
                            {
                                SetWindowText(GetDlgItem(hWnd, IDC_TEXT), szOutBuff);
                            }
                            free(szOutBuff);
                        }
                        else
                        {
                            MessageBox(hWnd, "Failed to allocate memory.", "Error", MB_OK | MB_ICONERROR);
                        }
                    }
                }
                free(szBuff);
            }
            else
            {
                MessageBox(hWnd, "Failed to allocate memory.", "Error", MB_OK | MB_ICONERROR);
            }
        }
    }
    break;
    }
}
// [/OnCommand]


// [OnPaint]: WM_PAINT
static void OnPaint(HWND hWnd)
{
    PAINTSTRUCT ps;
    HDC hDC = BeginPaint(hWnd, &ps);
    RECT rc;
    GetClientRect(hWnd, &rc);
    const int iWindowWidth = rc.right - rc.left;
    const int iWindowHeight = rc.bottom - rc.top;
    HDC hMemDC = CreateCompatibleDC(hDC);
    HBITMAP hBitmap = CreateCompatibleBitmap(hDC, iWindowWidth, iWindowHeight);
    HBITMAP hOldBitmap = (HBITMAP)SelectObject(hMemDC, hBitmap);

    FillRect(hMemDC, &ps.rcPaint, g_hBackgroundBrush);

    int iOldBkMode = SetBkMode(hMemDC, TRANSPARENT);
    HFONT hOldFont = (HFONT)GetCurrentObject(hMemDC, OBJ_FONT);

    char szText[128] = { 0 };
    SIZE size;

    SelectObject(hMemDC, g_hTitleFont);

    lstrcpy(szText, "Text");
    TextOut(hMemDC, 10, 10, szText, lstrlen(szText));

    lstrcpy(szText, "Base64");
    TextOut(hMemDC, iWindowWidth / 2 + 40, 10, szText, lstrlen(szText));

    SelectObject(hMemDC, g_hDefaultFont);

    lstrcpy(szText, "Copyright \251 2021 fastb1t");
    GetTextExtentPoint32(hMemDC, szText, lstrlen(szText), &size);
    TextOut(hMemDC, iWindowWidth - 10 - size.cx, iWindowHeight - 10 - size.cy, szText, lstrlen(szText));

    SelectObject(hMemDC, hOldFont);
    SetBkMode(hMemDC, iOldBkMode);

    BitBlt(hDC, 0, 0, iWindowWidth, iWindowHeight, hMemDC, 0, 0, SRCCOPY);
    SelectObject(hMemDC, hOldBitmap);
    DeleteObject(hBitmap);
    DeleteDC(hMemDC);
    EndPaint(hWnd, &ps);
}
// [/OnPaint]


// [OnGetMinMaxInfo]: WM_GETMINMAXINFO
static void OnGetMinMaxInfo(HWND hWnd, LPMINMAXINFO lpmmi)
{
    lpmmi->ptMinTrackSize.x = 400;
    lpmmi->ptMinTrackSize.y = 200;
}
// [/OnGetMinMaxInfo]


// [OnSize]: WM_SIZE
static void OnSize(HWND hWnd, UINT state, int cx, int cy)
{
    MoveWindow(GetDlgItem(hWnd, IDC_TEXT), 10, 30, cx / 2 - 50, cy - 70, TRUE);
    MoveWindow(GetDlgItem(hWnd, IDC_BASE64), cx / 2 + 40, 30, cx / 2 - 50, cy - 70, TRUE);
    MoveWindow(GetDlgItem(hWnd, IDC_TEXT_TO_BASE64), cx / 2 - 19, cy / 2 - 33, 38, 28, TRUE);
    MoveWindow(GetDlgItem(hWnd, IDC_BASE64_TO_TEXT), cx / 2 - 19, cy / 2 + 5, 38, 28, TRUE);
}
// [/OnSize]


// [OnDestroy]: WM_DESTROY
static void OnDestroy(HWND hWnd)
{
    DeleteObject(g_hBackgroundBrush);

    DeleteObject(g_hTitleFont);
    DeleteObject(g_hDefaultFont);

    PostQuitMessage(0);
}
// [/OnDestroy]
