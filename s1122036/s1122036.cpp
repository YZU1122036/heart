/**
 * @file HeartAttack_Cards_Doxygen_Commented.cpp
 * @brief 使用 Win32 GDI 繪製 52 張撲克牌，並建立「心臟病」遊戲選單。
 *
 * 本程式會建立一個 Windows 視窗，透過 GDI 繪製完整一副撲克牌。
 * 視窗上方提供單人、雙人、多人與說明選單項目。
 *
 * @author 第28組
 * @date 2026
 */

#include <windows.h>
#include <tchar.h>

 /*============================================================
  * 常數定義區
  *============================================================*/

  /** @brief 選單項目：單人計時賽。 */
#define ID_SINGLE       1001

/** @brief 選單項目：雙人鍵盤競速。 */
#define ID_DOUBLE       1002

/** @brief 選單項目：多人視覺辨識賽。 */
#define ID_MULTIPLE     1003

/** @brief 選單項目：說明。 */
#define ID_HELP_ABOUT   1004

/** @brief 撲克牌寬度。 */
#define CARD_W 95

/** @brief 撲克牌高度。 */
#define CARD_H 140

/** @brief 撲克牌之間的間距。 */
#define GAP 4

/*============================================================
 * 函式宣告區
 *============================================================*/

void DrawText(HDC hdc, int x, int y, LPCTSTR text, int size, COLORREF color, int angle = 0);
void DrawSuitCenter(HDC hdc, int cx, int cy, LPCTSTR suit, COLORREF color, bool reverse = false);
void DrawPips(HDC hdc, int x, int y, int num, LPCTSTR suit, COLORREF color);
void DrawOneCard(HDC hdc, int x, int y, int num, LPCTSTR rank, LPCTSTR suit, COLORREF color);
void DrawAllCards(HDC hdc);
HMENU CreateAppMenu();
LRESULT CALLBACK WndProc(HWND hwnd, UINT msg, WPARAM wParam, LPARAM lParam);

/*============================================================
 * 繪圖相關函式
 *============================================================*/

 /**
  * @brief 在指定位置繪製文字。
  *
  * 此函式會建立指定大小、顏色與旋轉角度的字型，並使用 TextOut 繪製文字。
  * angle 參數使用 Win32 CreateFont 的角度單位，1800 代表 180 度。
  *
  * @param hdc   裝置內容 Handle，用於繪圖。
  * @param x     文字左上角 X 座標。
  * @param y     文字左上角 Y 座標。
  * @param text  要繪製的文字。
  * @param size  字體大小。
  * @param color 文字顏色。
  * @param angle 文字旋轉角度，預設為 0。
  */
void DrawText(HDC hdc, int x, int y, LPCTSTR text, int size, COLORREF color, int angle)
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

/**
 * @brief 將花色符號繪製在指定中心點。
 *
 * 此函式用於繪製撲克牌中央或點數區的花色符號。
 * reverse 為 true 時，花色會旋轉 180 度，模擬撲克牌下半部倒置的效果。
 *
 * @param hdc     裝置內容 Handle，用於繪圖。
 * @param cx      花色中心點 X 座標。
 * @param cy      花色中心點 Y 座標。
 * @param suit    花色符號，例如 ♠、♥、♦、♣。
 * @param color   花色顏色。
 * @param reverse 是否將花色旋轉 180 度，預設為 false。
 */
void DrawSuitCenter(HDC hdc, int cx, int cy, LPCTSTR suit, COLORREF color, bool reverse)
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

    if (reverse) {
        TextOut(hdc, cx + size.cx / 2, cy + size.cy / 2, suit, lstrlen(suit));
    }
    else {
        TextOut(hdc, cx - size.cx / 2, cy - size.cy / 2, suit, lstrlen(suit));
    }

    SelectObject(hdc, oldFont);
    DeleteObject(font);
}

/**
 * @brief 根據牌面點數繪製撲克牌中央的花色點數。
 *
 * A 到 10 的牌面會呼叫此函式繪製中間的花色排列。
 * J、Q、K 不會使用此函式，而是在中央顯示大字母。
 *
 * @param hdc   裝置內容 Handle，用於繪圖。
 * @param x     撲克牌左上角 X 座標。
 * @param y     撲克牌左上角 Y 座標。
 * @param num   牌面數字，A 為 1，2 到 10 為原數字。
 * @param suit  花色符號。
 * @param color 花色顏色。
 */
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

/**
 * @brief 繪製單張撲克牌。
 *
 * 此函式會先繪製牌框，再繪製左上角與右下角的牌面資訊。
 * 若牌面為 J、Q、K，中央會顯示大字母；否則繪製對應數量的花色點數。
 *
 * @param hdc   裝置內容 Handle，用於繪圖。
 * @param x     撲克牌左上角 X 座標。
 * @param y     撲克牌左上角 Y 座標。
 * @param num   牌面數值，A 為 1，J/Q/K 分別為 11/12/13。
 * @param rank  牌面文字，例如 A、2、J、Q、K。
 * @param suit  花色符號。
 * @param color 牌面顏色。
 */
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

/**
 * @brief 繪製完整 52 張撲克牌。
 *
 * 牌面以 4 列顯示，每列代表一種花色，每列包含 A 到 K 共 13 張牌。
 *
 * @param hdc 裝置內容 Handle，用於繪圖。
 */
void DrawAllCards(HDC hdc)
{
    LPCTSTR ranks[] = {
        _T("A"), _T("2"), _T("3"), _T("4"), _T("5"), _T("6"), _T("7"),
        _T("8"), _T("9"), _T("10"), _T("J"), _T("Q"), _T("K")
    };

    LPCTSTR suits[] = { _T("♠"), _T("♥"), _T("♦"), _T("♣") };
    COLORREF colors[] = { RGB(0, 0, 0), RGB(255, 0, 0), RGB(255, 0, 0), RGB(0, 0, 0) };

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

/*============================================================
 * 視窗與選單相關函式
 *============================================================*/

 /**
  * @brief Windows 視窗訊息處理函式。
  *
  * 負責處理選單指令、繪圖事件與視窗關閉事件。
  * WM_PAINT 發生時會呼叫 DrawAllCards() 重新繪製全部撲克牌。
  *
  * @param hwnd   視窗 Handle。
  * @param msg    Windows 訊息代碼。
  * @param wParam 訊息的額外參數。
  * @param lParam 訊息的額外參數。
  * @return 訊息處理結果。
  */
LRESULT CALLBACK WndProc(HWND hwnd, UINT msg, WPARAM wParam, LPARAM lParam)
{
    switch (msg) {
    case WM_COMMAND:
        switch (LOWORD(wParam)) {
        case ID_SINGLE:
            break;

        case ID_DOUBLE:
            break;

        case ID_MULTIPLE:
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

/**
 * @brief 建立程式上方選單。
 *
 * 選單包含單人計時賽、雙人鍵盤競速、多人視覺辨識賽與說明。
 *
 * @return 建立完成的選單 Handle。
 */
HMENU CreateAppMenu()
{
    HMENU hMenuBar = CreateMenu();

    AppendMenu(hMenuBar, MF_STRING, ID_SINGLE, _T("單人計時賽"));
    AppendMenu(hMenuBar, MF_STRING, ID_DOUBLE, _T("雙人鍵盤競速"));
    AppendMenu(hMenuBar, MF_STRING, ID_MULTIPLE, _T("多人視覺辨識賽"));
    AppendMenu(hMenuBar, MF_STRING, ID_HELP_ABOUT, _T("說明"));

    return hMenuBar;
}

/**
 * @brief Windows 應用程式進入點。
 *
 * 此函式會註冊視窗類別、建立主視窗、設定選單並進入訊息迴圈。
 *
 * @param hInstance 目前程式實例 Handle。
 * @param           未使用的前一個程式實例 Handle。
 * @param           命令列參數，本程式未使用。
 * @param nCmdShow  視窗顯示方式。
 * @return 程式結束代碼。
 */
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
