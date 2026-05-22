#include <windows.h>
#include <tchar.h>

#define ID_SINGLE  1001
#define ID_DOUBLE 1002
#define ID_MUTIPLE 1003
#define ID_HELP_ABOUT 1004

#define CARD_W 95
#define CARD_H 140
#define GAP 4

void DrawText(HDC hdc, int x, int y, LPCTSTR text, int size, COLORREF color, int angle = 0)
{
    SetBkMode(hdc, TRANSPARENT);
    SetTextColor(hdc, color);

    HFONT font = CreateFont(size, 0, angle, angle, FW_NORMAL, FALSE, FALSE, FALSE,
        DEFAULT_CHARSET, OUT_DEFAULT_PRECIS, CLIP_DEFAULT_PRECIS,
        DEFAULT_QUALITY, DEFAULT_PITCH, _T("Times New Roman"));

    HFONT oldFont = (HFONT)SelectObject(hdc, font);
    TextOut(hdc, x, y, text, lstrlen(text));
    SelectObject(hdc, oldFont);
    DeleteObject(font);
}

void DrawSuitCenter(HDC hdc, int cx, int cy, LPCTSTR suit, COLORREF color, bool reverse = false)
{
    SetBkMode(hdc, TRANSPARENT);
    SetTextColor(hdc, color);

    HFONT font = CreateFont(20, 0, reverse ? 1800 : 0, reverse ? 1800 : 0,
        FW_NORMAL, FALSE, FALSE, FALSE,
        DEFAULT_CHARSET, OUT_DEFAULT_PRECIS, CLIP_DEFAULT_PRECIS,
        DEFAULT_QUALITY, DEFAULT_PITCH, _T("Segoe UI Symbol"));

    HFONT oldFont = (HFONT)SelectObject(hdc, font);

    SIZE size;
    GetTextExtentPoint32(hdc, suit, lstrlen(suit), &size);

    if (reverse)
        TextOut(hdc, cx + size.cx / 2, cy + size.cy / 2, suit, lstrlen(suit));
    else
        TextOut(hdc, cx - size.cx / 2, cy - size.cy / 2, suit, lstrlen(suit));

    SelectObject(hdc, oldFont);
    DeleteObject(font);
}

void DrawPips(HDC hdc, int x, int y, int num, LPCTSTR suit, COLORREF color)
{
    int L = x + 30;
    int C = x + CARD_W / 2;
    int R = x + CARD_W - 30;

    int Y1 = y + 38;
    int Y2 = y + 58;
    int Y3 = y + 78;
    int Y4 = y + 98;
    int Y5 = y + 118;

    if (num == 1) {
        DrawSuitCenter(hdc, C, Y3, suit, color);
    }
    else if (num == 2) {
        DrawSuitCenter(hdc, C, Y1, suit, color);
        DrawSuitCenter(hdc, C, Y5, suit, color, true);
    }
    else if (num == 3) {
        DrawSuitCenter(hdc, C, Y1, suit, color);
        DrawSuitCenter(hdc, C, Y3, suit, color);
        DrawSuitCenter(hdc, C, Y5, suit, color, true);
    }
    else if (num == 4) {
        DrawSuitCenter(hdc, L, Y1, suit, color);
        DrawSuitCenter(hdc, R, Y1, suit, color);
        DrawSuitCenter(hdc, L, Y5, suit, color, true);
        DrawSuitCenter(hdc, R, Y5, suit, color, true);
    }
    else if (num == 5) {
        DrawSuitCenter(hdc, L, Y1, suit, color);
        DrawSuitCenter(hdc, R, Y1, suit, color);
        DrawSuitCenter(hdc, C, Y3, suit, color);
        DrawSuitCenter(hdc, L, Y5, suit, color, true);
        DrawSuitCenter(hdc, R, Y5, suit, color, true);
    }
    else if (num == 6) {
        DrawSuitCenter(hdc, L, Y1, suit, color);
        DrawSuitCenter(hdc, R, Y1, suit, color);
        DrawSuitCenter(hdc, L, Y3, suit, color);
        DrawSuitCenter(hdc, R, Y3, suit, color);
        DrawSuitCenter(hdc, L, Y5, suit, color, true);
        DrawSuitCenter(hdc, R, Y5, suit, color, true);
    }
    else if (num == 7) {
        DrawSuitCenter(hdc, L, Y1, suit, color);
        DrawSuitCenter(hdc, R, Y1, suit, color);
        DrawSuitCenter(hdc, C, Y2, suit, color);
        DrawSuitCenter(hdc, L, Y3, suit, color);
        DrawSuitCenter(hdc, R, Y3, suit, color);
        DrawSuitCenter(hdc, L, Y5, suit, color, true);
        DrawSuitCenter(hdc, R, Y5, suit, color, true);
    }
    else if (num == 8) {
        DrawSuitCenter(hdc, L, Y1, suit, color);
        DrawSuitCenter(hdc, R, Y1, suit, color);
        DrawSuitCenter(hdc, C, Y2, suit, color);
        DrawSuitCenter(hdc, L, Y3, suit, color);
        DrawSuitCenter(hdc, R, Y3, suit, color);
        DrawSuitCenter(hdc, C, Y4, suit, color, true);
        DrawSuitCenter(hdc, L, Y5, suit, color, true);
        DrawSuitCenter(hdc, R, Y5, suit, color, true);
    }
    else if (num == 9) {
        DrawSuitCenter(hdc, L, Y1, suit, color);
        DrawSuitCenter(hdc, R, Y1, suit, color);
        DrawSuitCenter(hdc, L, Y2, suit, color);
        DrawSuitCenter(hdc, R, Y2, suit, color);
        DrawSuitCenter(hdc, C, Y3, suit, color);
        DrawSuitCenter(hdc, L, Y4, suit, color, true);
        DrawSuitCenter(hdc, R, Y4, suit, color, true);
        DrawSuitCenter(hdc, L, Y5, suit, color, true);
        DrawSuitCenter(hdc, R, Y5, suit, color, true);
    }
    else if (num == 10) {
        DrawSuitCenter(hdc, L, Y1, suit, color);
        DrawSuitCenter(hdc, R, Y1, suit, color);

        DrawSuitCenter(hdc, C, Y2 - 7, suit, color);

        DrawSuitCenter(hdc, L, Y2, suit, color);
        DrawSuitCenter(hdc, R, Y2, suit, color);

        DrawSuitCenter(hdc, L, Y4, suit, color, true);
        DrawSuitCenter(hdc, R, Y4, suit, color, true);

        DrawSuitCenter(hdc, C, Y4 + 7, suit, color, true);

        DrawSuitCenter(hdc, L, Y5, suit, color, true);
        DrawSuitCenter(hdc, R, Y5, suit, color, true);
    }
}

void DrawOneCard(HDC hdc, int x, int y, int num, LPCTSTR rank, LPCTSTR suit, COLORREF color)
{
    Rectangle(hdc, x, y, x + CARD_W, y + CARD_H);

    DrawText(hdc, x + 7, y + 6, rank, 14, color);
    DrawText(hdc, x + 7, y + 21, suit, 14, color);

    DrawText(hdc, x + CARD_W - 8, y + CARD_H - 7, rank, 14, color, 1800);
    DrawText(hdc, x + CARD_W - 8, y + CARD_H - 22, suit, 14, color, 1800);

    if (num >= 11) {
        DrawText(hdc, x + 36, y + 52, rank, 40, color);
    }
    else {
        DrawPips(hdc, x, y, num, suit, color);
    }
}

void DrawAllCards(HDC hdc)
{
    LPCTSTR ranks[] = {
        _T("A"), _T("2"), _T("3"), _T("4"), _T("5"), _T("6"), _T("7"),
        _T("8"), _T("9"), _T("10"), _T("J"), _T("Q"), _T("K")
    };

    LPCTSTR suits[] = { _T("♠"), _T("♥"), _T("♦"), _T("♣") };
    COLORREF colors[] = { RGB(0,0,0), RGB(255,0,0), RGB(255,0,0), RGB(0,0,0) };

    int startX = 8;
    int startY = 8;

    for (int s = 0; s < 4; s++) {
        for (int r = 0; r < 13; r++) {
            int x = startX + r * (CARD_W + GAP);
            int y = startY + s * (CARD_H + GAP);

            DrawOneCard(hdc, x, y, r + 1, ranks[r], suits[s], colors[s]);
        }
    }
}

LRESULT CALLBACK WndProc(HWND hwnd, UINT msg, WPARAM wParam, LPARAM lParam) {
    switch (msg) {
    case WM_COMMAND:
        switch (LOWORD(wParam)) {
        case ID_SINGLE:
            break;
        case ID_DOUBLE:
            break;
        case ID_MUTIPLE:
            break;
        case ID_HELP_ABOUT:
            MessageBox(hwnd, _T("心臟病."), _T("About"), MB_OK | MB_ICONINFORMATION);
            break;
        }
        break;

    case WM_PAINT: {
        PAINTSTRUCT ps;
        HDC hdc = BeginPaint(hwnd, &ps);

        DrawAllCards(hdc);

        EndPaint(hwnd, &ps);
        break;
    }

    case WM_DESTROY:
        PostQuitMessage(0);
        break;

    default:
        return DefWindowProc(hwnd, msg, wParam, lParam);
    }
    return 0;
}

HMENU CreateAppMenu() {
    HMENU hMenuBar = CreateMenu();

    AppendMenu(hMenuBar, MF_STRING, ID_SINGLE, _T("單人計時賽"));
    AppendMenu(hMenuBar, MF_STRING, ID_DOUBLE, _T("雙人鍵盤競速"));
    AppendMenu(hMenuBar, MF_STRING, ID_MUTIPLE, _T("多人視覺辨識賽"));
    AppendMenu(hMenuBar, MF_STRING, ID_HELP_ABOUT, _T("說明"));
    return hMenuBar;
}

int APIENTRY _tWinMain(HINSTANCE hInstance, HINSTANCE, LPTSTR, int nCmdShow)
{
    const TCHAR CLASS_NAME[] = _T("Win32MenuExample");
    WNDCLASS wc = {};
    wc.lpfnWndProc = WndProc;
    wc.hInstance = hInstance;
    wc.lpszClassName = CLASS_NAME;
    wc.hbrBackground = (HBRUSH)(COLOR_WINDOW + 1);
    wc.hCursor = LoadCursor(NULL, IDC_ARROW);

    if (!RegisterClass(&wc)) {
        MessageBox(NULL, _T("Window Registration Failed!"), _T("Error"), MB_ICONERROR);
        return 0;
    }

    HWND hwnd = CreateWindow(
        CLASS_NAME,
        _T("心臟病(第28組:1122036 1132068)"),
        WS_OVERLAPPEDWINDOW,
        CW_USEDEFAULT, CW_USEDEFAULT,
        1320, 650,
        NULL, NULL, hInstance, NULL
    );

    if (!hwnd) {
        MessageBox(NULL, _T("Window Creation Failed!"), _T("Error"), MB_ICONERROR);
        return 0;
    }

    SetMenu(hwnd, CreateAppMenu());
    ShowWindow(hwnd, nCmdShow);
    UpdateWindow(hwnd);

    MSG msg;
    while (GetMessage(&msg, NULL, 0, 0)) {
        TranslateMessage(&msg);
        DispatchMessage(&msg);
    }
    return (int)msg.wParam;
}
