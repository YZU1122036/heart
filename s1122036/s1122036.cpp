/**
 * @file main.cpp
 * @brief Win32 API 心臟病單人計時賽。
 *
 * 本程式修改自老師提供的 Win32 API 選單範例，
 * 並以物件導向方式設計撲克牌、牌組與單人計時賽。
 *
 * 功能說明：
 * 1. 保留老師提供的上方選單列。
 * 2. 按下「單人計時賽」後開始遊戲。
 * 3. 隨機產生 1 到 13 的目標數字。
 * 4. 從洗牌後的 52 張撲克牌中依序顯示牌面，52 張內不重複。
 * 5. 每張牌顯示 570 毫秒後換下一張。
 * 6. 使用者按下空白鍵後停止。
 * 7. 顯示目前這張牌出現後到按下空白鍵的秒數與毫秒數。
 * 8. 判斷牌面點數是否等於目標數字。
 */

#include <windows.h>
#include <string>
#include <vector>
#include <array>
#include <random>
#include <algorithm>

#pragma comment(lib, "gdi32.lib")
#pragma comment(lib, "user32.lib")

#define ID_SINGLE      1001
#define ID_DOUBLE      1002
#define ID_MUTIPLE     1003
#define ID_HELP_ABOUT  1004
#define ID_TIMER_CARD  2001

 /**
  * @brief 牌出現時間。
  *
  * 單位為毫秒。
  * 570 代表每張牌顯示 570 毫秒後換下一張。
  */
const int CARD_INTERVAL_MS = 570;

/**
 * @struct Suit
 * @brief 撲克牌花色資料。
 *
 * 儲存花色符號、名稱與顏色。
 */
struct Suit {
    const wchar_t* symbol;   ///< 花色符號，例如 ♠、♥、♦、♣
    const wchar_t* name;     ///< 花色名稱
    COLORREF color;          ///< 花色顏色
};

/**
 * @struct CardValue
 * @brief 撲克牌點數資料。
 *
 * 儲存牌面顯示文字與實際數值。
 */
struct CardValue {
    const wchar_t* label;    ///< 牌面文字，例如 A、2、J、Q、K
    int value;               ///< 實際點數，A 為 1，J 為 11，Q 為 12，K 為 13
};

/**
 * @struct PipPos
 * @brief 牌面中央花色符號的位置。
 */
struct PipPos {
    int dx;                  ///< 相對中心點的 X 位移
    int dy;                  ///< 相對中心點的 Y 位移
};

/**
 * @class TextRenderer
 * @brief 文字繪製工具類別。
 *
 * 提供靜態函式，用來在指定 HDC 上繪製文字。
 */
class TextRenderer {
public:
    /**
     * @brief 在指定位置繪製文字。
     *
     * @param hdc 繪圖用的 HDC。
     * @param x 文字 X 座標。
     * @param y 文字 Y 座標。
     * @param text 要繪製的文字。
     * @param fontSize 字體大小。
     * @param color 文字顏色。
     * @param textAlign 文字對齊方式。
     * @param angle10 文字旋轉角度，單位為十分之一度。
     * @param fontName 字體名稱。
     * @param fontWeight 字體粗細。
     */
    static void Draw(
        HDC hdc,
        int x,
        int y,
        const std::wstring& text,
        int fontSize,
        COLORREF color,
        UINT textAlign,
        int angle10 = 0,
        const wchar_t* fontName = L"Segoe UI Symbol",
        int fontWeight = FW_NORMAL)
    {
        LOGFONTW lf = {};
        lf.lfHeight = -fontSize;
        lf.lfEscapement = angle10;
        lf.lfOrientation = angle10;
        lf.lfWeight = fontWeight;
        lf.lfCharSet = DEFAULT_CHARSET;
        lstrcpyW(lf.lfFaceName, fontName);

        HFONT hFont = CreateFontIndirectW(&lf);
        HFONT hOldFont = (HFONT)SelectObject(hdc, hFont);

        COLORREF oldColor = SetTextColor(hdc, color);
        int oldBkMode = SetBkMode(hdc, TRANSPARENT);
        UINT oldAlign = SetTextAlign(hdc, textAlign);

        TextOutW(hdc, x, y, text.c_str(), (int)text.length());

        SetTextAlign(hdc, oldAlign);
        SetBkMode(hdc, oldBkMode);
        SetTextColor(hdc, oldColor);

        SelectObject(hdc, hOldFont);
        DeleteObject(hFont);
    }
};

/**
 * @class PipLayout
 * @brief 點數牌中央花色符號位置配置類別。
 *
 * 依照牌面點數 1 到 10，回傳對應的中央花色符號位置。
 */
class PipLayout {
public:
    /**
     * @brief 取得指定點數的花色符號位置。
     *
     * @param count 點數，範圍為 1 到 10。
     * @return std::vector<PipPos> 花色符號位置清單。
     */
    static std::vector<PipPos> GetPositions(int count) {
        int top = -55;
        int bottom = 55;
        int v31 = top / 2;
        int v32 = -v31;
        int v41 = top / 3;
        int v42 = -v41;
        int v51 = (top + v41) / 2;
        int v52 = -v51;
        int mid = 0;
        int cen = 0;
        int left = -25;
        int right = 25;

        std::vector<std::vector<PipPos>> positions(11);

        positions[1] = { {cen, mid} };
        positions[2] = { {cen, top}, {cen, bottom} };
        positions[3] = { {cen, top}, {cen, mid}, {cen, bottom} };
        positions[4] = { {left, top}, {right, top}, {left, bottom}, {right, bottom} };
        positions[5] = { {left, top}, {right, top}, {cen, mid}, {left, bottom}, {right, bottom} };
        positions[6] = { {left, top}, {right, top}, {left, mid}, {right, mid}, {left, bottom}, {right, bottom} };
        positions[7] = { {left, top}, {right, top}, {cen, v31}, {left, mid}, {right, mid}, {left, bottom}, {right, bottom} };
        positions[8] = { {left, top}, {right, top}, {cen, v31}, {left, mid}, {right, mid}, {cen, v32}, {left, bottom}, {right, bottom} };
        positions[9] = { {left, top}, {right, top}, {cen, v51}, {left, v41}, {right, v41}, {left, v42}, {right, v42}, {left, bottom}, {right, bottom} };
        positions[10] = { {left, top}, {right, top}, {cen, v51}, {left, v41}, {right, v41}, {left, v42}, {right, v42}, {cen, v52}, {left, bottom}, {right, bottom} };

        if (count < 1 || count > 10) {
            return {};
        }

        return positions[count];
    }
};

/**
 * @class Card
 * @brief 單張撲克牌類別。
 *
 * 儲存一張牌的花色與點數，並負責將該牌繪製到視窗上。
 */
class Card {
private:
    Suit suit_;              ///< 牌的花色
    CardValue value_;        ///< 牌的點數

    static constexpr int WIDTH = 120;     ///< 牌寬度
    static constexpr int HEIGHT = 180;    ///< 牌高度
    static constexpr int PADDING = 10;    ///< 牌內距

public:
    /**
     * @brief 建立一張撲克牌。
     *
     * @param value 牌的點數。
     * @param suit 牌的花色。
     */
    Card(const CardValue& value, const Suit& suit)
        : suit_(suit), value_(value) {
    }

    /**
     * @brief 取得牌面點數。
     *
     * @return int 牌面點數，A 為 1，J 為 11，Q 為 12，K 為 13。
     */
    int GetValue() const {
        return value_.value;
    }

    /**
     * @brief 繪製撲克牌。
     *
     * @param hdc 繪圖用的 HDC。
     * @param left 牌左上角 X 座標。
     * @param top 牌左上角 Y 座標。
     */
    void Draw(HDC hdc, int left, int top) const {
        DrawCardFrame(hdc, left, top);

        int innerLeft = left + PADDING;
        int innerTop = top + PADDING;
        int innerW = WIDTH - 2 * PADDING;
        int innerH = HEIGHT - 2 * PADDING;
        int innerCx = innerLeft + innerW / 2;
        int innerCy = innerTop + innerH / 2;

        DrawTopLeftCorner(hdc, innerLeft, innerTop);
        DrawBottomRightCorner(hdc, left, top);

        if (value_.value <= 10) {
            DrawPips(hdc, value_.value, innerCx, innerCy);
        }
        else {
            DrawFaceCard(hdc, innerCx, innerCy);
        }
    }

    /**
     * @brief 取得牌寬度。
     *
     * @return int 牌寬度。
     */
    static int GetWidth() {
        return WIDTH;
    }

    /**
     * @brief 取得牌高度。
     *
     * @return int 牌高度。
     */
    static int GetHeight() {
        return HEIGHT;
    }

private:
    /**
     * @brief 繪製牌的外框。
     *
     * @param hdc 繪圖用的 HDC。
     * @param left 牌左上角 X 座標。
     * @param top 牌左上角 Y 座標。
     */
    void DrawCardFrame(HDC hdc, int left, int top) const {
        HBRUSH hWhite = CreateSolidBrush(RGB(255, 255, 255));
        HBRUSH hOldBrush = (HBRUSH)SelectObject(hdc, hWhite);

        HPEN hPen = CreatePen(PS_SOLID, 1, RGB(0, 0, 0));
        HPEN hOldPen = (HPEN)SelectObject(hdc, hPen);

        RoundRect(hdc, left, top, left + WIDTH, top + HEIGHT, 10, 10);

        SelectObject(hdc, hOldBrush);
        SelectObject(hdc, hOldPen);

        DeleteObject(hWhite);
        DeleteObject(hPen);
    }

    /**
     * @brief 繪製左上角的點數與花色。
     *
     * @param hdc 繪圖用的 HDC。
     * @param innerLeft 內部左側 X 座標。
     * @param innerTop 內部上方 Y 座標。
     */
    void DrawTopLeftCorner(HDC hdc, int innerLeft, int innerTop) const {
        TextRenderer::Draw(
            hdc,
            innerLeft,
            innerTop + 16,
            value_.label,
            16,
            suit_.color,
            TA_LEFT | TA_BASELINE,
            0,
            L"Segoe UI",
            FW_NORMAL
        );

        TextRenderer::Draw(
            hdc,
            innerLeft,
            innerTop + 32,
            suit_.symbol,
            16,
            suit_.color,
            TA_LEFT | TA_BASELINE,
            0,
            L"Segoe UI Symbol",
            FW_NORMAL
        );
    }

    /**
     * @brief 繪製右下角倒轉的點數與花色。
     *
     * @param hdc 繪圖用的 HDC。
     * @param left 牌左上角 X 座標。
     * @param top 牌左上角 Y 座標。
     */
    void DrawBottomRightCorner(HDC hdc, int left, int top) const {
        TextRenderer::Draw(
            hdc,
            left + WIDTH - PADDING * 2,
            top + HEIGHT - PADDING * 2 - 16,
            suit_.symbol,
            16,
            suit_.color,
            TA_RIGHT | TA_BASELINE,
            1800,
            L"Segoe UI Symbol",
            FW_NORMAL
        );

        TextRenderer::Draw(
            hdc,
            left + WIDTH - PADDING * 2,
            top + HEIGHT - PADDING * 2,
            value_.label,
            16,
            suit_.color,
            TA_RIGHT | TA_BASELINE,
            1800,
            L"Segoe UI",
            FW_NORMAL
        );
    }

    /**
     * @brief 繪製中央點數花色。
     *
     * @param hdc 繪圖用的 HDC。
     * @param count 點數。
     * @param cx 牌中央 X 座標。
     * @param cy 牌中央 Y 座標。
     */
    void DrawPips(HDC hdc, int count, int cx, int cy) const {
        auto positions = PipLayout::GetPositions(count);

        for (const auto& p : positions) {
            int angle = (p.dy < 5) ? 0 : 1800;

            TextRenderer::Draw(
                hdc,
                cx + p.dx,
                cy + p.dy,
                suit_.symbol,
                24,
                suit_.color,
                TA_CENTER | TA_BASELINE,
                angle,
                L"Segoe UI Symbol",
                FW_NORMAL
            );
        }
    }

    /**
     * @brief 繪製 J、Q、K 牌面。
     *
     * @param hdc 繪圖用的 HDC。
     * @param cx 牌中央 X 座標。
     * @param cy 牌中央 Y 座標。
     */
    void DrawFaceCard(HDC hdc, int cx, int cy) const {
        TextRenderer::Draw(
            hdc,
            cx,
            cy + 14,
            value_.label,
            40,
            suit_.color,
            TA_CENTER | TA_BASELINE,
            0,
            L"Segoe UI",
            FW_BOLD
        );

        TextRenderer::Draw(
            hdc,
            cx,
            cy + 55,
            suit_.symbol,
            36,
            suit_.color,
            TA_CENTER | TA_BASELINE,
            0,
            L"Segoe UI Symbol",
            FW_NORMAL
        );
    }
};

/**
 * @class Deck
 * @brief 撲克牌組類別。
 *
 * 建立 52 張撲克牌，並提供洗牌與依序取牌功能。
 * 每洗牌一次後，52 張牌會不重複出現。
 */
class Deck {
private:
    std::vector<Card> cards_;    ///< 52 張撲克牌
    size_t currentIndex_;        ///< 目前取牌位置

public:
    /**
     * @brief 建立牌組並洗牌。
     */
    Deck()
        : currentIndex_(0) {
        Initialize();
        Shuffle();
    }

    /**
     * @brief 重新洗牌。
     */
    void ResetAndShuffle() {
        Shuffle();
    }

    /**
     * @brief 取得下一張牌。
     *
     * 如果 52 張牌都已經取完，會重新洗牌。
     *
     * @return const Card& 下一張牌。
     */
    const Card& GetNextCard() {
        if (currentIndex_ >= cards_.size()) {
            Shuffle();
        }

        return cards_[currentIndex_++];
    }

private:
    /**
     * @brief 洗牌並重設取牌位置。
     */
    void Shuffle() {
        static std::random_device rd;
        static std::mt19937 gen(rd());

        std::shuffle(cards_.begin(), cards_.end(), gen);
        currentIndex_ = 0;
    }

    /**
     * @brief 初始化 52 張撲克牌。
     *
     * 使用 4 種花色與 13 種點數建立完整牌組。
     */
    void Initialize() {
        static const std::array<Suit, 4> suits = {
            Suit{ L"♠", L"spade",   RGB(0, 0, 0) },
            Suit{ L"♥", L"heart",   RGB(220, 0, 0) },
            Suit{ L"♦", L"diamond", RGB(220, 0, 0) },
            Suit{ L"♣", L"club",    RGB(0, 0, 0) }
        };

        static const std::array<CardValue, 13> values = {
            CardValue{ L"A",  1  },
            CardValue{ L"2",  2  },
            CardValue{ L"3",  3  },
            CardValue{ L"4",  4  },
            CardValue{ L"5",  5  },
            CardValue{ L"6",  6  },
            CardValue{ L"7",  7  },
            CardValue{ L"8",  8  },
            CardValue{ L"9",  9  },
            CardValue{ L"10", 10 },
            CardValue{ L"J",  11 },
            CardValue{ L"Q",  12 },
            CardValue{ L"K",  13 }
        };

        cards_.clear();
        cards_.reserve(52);

        for (const auto& suit : suits) {
            for (const auto& value : values) {
                cards_.emplace_back(value, suit);
            }
        }
    }
};

/**
 * @class SingleTimeTrialGame
 * @brief 單人計時賽遊戲類別。
 *
 * 控制單人計時賽流程：
 * 1. 產生目標數字。
 * 2. 顯示洗牌後的撲克牌。
 * 3. 記錄目前牌出現的時間。
 * 4. 按下空白鍵後計算反應時間。
 * 5. 判斷牌面點數是否與目標數字相同。
 */
class SingleTimeTrialGame {
private:
    Deck deck_;                    ///< 牌組
    const Card* currentCard_;      ///< 目前顯示的牌

    int targetNumber_;             ///< 目標數字，範圍 1 到 13
    ULONGLONG cardShowTime_;       ///< 目前這張牌出現的時間
    ULONGLONG reactionTimeMs_;     ///< 反應時間，單位毫秒

    bool playing_;                 ///< 是否正在遊戲中
    bool stopped_;                 ///< 是否已經按下空白鍵停止
    bool correct_;                 ///< 判斷結果是否正確

public:
    /**
     * @brief 建立單人計時賽物件。
     */
    SingleTimeTrialGame()
        : currentCard_(nullptr),
        targetNumber_(1),
        cardShowTime_(0),
        reactionTimeMs_(0),
        playing_(false),
        stopped_(false),
        correct_(false) {
    }

    /**
     * @brief 開始單人計時賽。
     *
     * 重新洗牌、取得第一張牌、產生目標數字，並開始計時。
     */
    void Start() {
        deck_.ResetAndShuffle();

        currentCard_ = &deck_.GetNextCard();
        targetNumber_ = GetRandomNumber();

        cardShowTime_ = GetTickCount64();
        reactionTimeMs_ = 0;

        playing_ = true;
        stopped_ = false;
        correct_ = false;
    }

    /**
     * @brief 更換目前顯示的牌。
     *
     * 每次更換新牌時，都會重新記錄該張牌出現的時間。
     */
    void ChangeCard() {
        if (!playing_ || stopped_) {
            return;
        }

        currentCard_ = &deck_.GetNextCard();
        cardShowTime_ = GetTickCount64();
    }

    /**
     * @brief 處理空白鍵按下事件。
     *
     * 計算目前這張牌出現後，到使用者按下空白鍵的時間，
     * 並判斷牌面點數是否等於目標數字。
     *
     * @return bool 若成功停止遊戲回傳 true，否則回傳 false。
     */
    bool PressSpace() {
        if (!playing_ || stopped_ || currentCard_ == nullptr) {
            return false;
        }

        reactionTimeMs_ = GetTickCount64() - cardShowTime_;

        stopped_ = true;
        playing_ = false;

        correct_ = (currentCard_->GetValue() == targetNumber_);

        return true;
    }

    /**
     * @brief 判斷遊戲是否正在進行。
     *
     * @return bool 正在進行回傳 true。
     */
    bool IsRunning() const {
        return playing_ && !stopped_;
    }

    /**
     * @brief 取得反應時間。
     *
     * @return ULONGLONG 反應時間，單位毫秒。
     */
    ULONGLONG GetReactionTimeMs() const {
        return reactionTimeMs_;
    }

    /**
     * @brief 取得反應時間。
     *
     * @return double 反應時間，單位秒。
     */
    double GetReactionTimeSeconds() const {
        return reactionTimeMs_ / 1000.0;
    }

    /**
     * @brief 取得目標數字。
     *
     * @return int 目標數字。
     */
    int GetTargetNumber() const {
        return targetNumber_;
    }

    /**
     * @brief 取得目前牌面點數。
     *
     * @return int 目前牌面點數。
     */
    int GetCurrentCardValue() const {
        if (currentCard_ == nullptr) {
            return 0;
        }

        return currentCard_->GetValue();
    }

    /**
     * @brief 取得判斷結果。
     *
     * @return bool 正確回傳 true，錯誤回傳 false。
     */
    bool IsCorrect() const {
        return correct_;
    }

    /**
     * @brief 繪製遊戲畫面。
     *
     * @param hdc 繪圖用的 HDC。
     * @param rc 視窗客戶區範圍。
     */
    void Draw(HDC hdc, const RECT& rc) const {
        int centerX = (rc.right - rc.left) / 2;

        TextRenderer::Draw(
            hdc,
            centerX,
            55,
            L"心臟病 - 單人計時賽",
            28,
            RGB(0, 0, 0),
            TA_CENTER | TA_BASELINE,
            0,
            L"Microsoft JhengHei UI",
            FW_BOLD
        );

        if (currentCard_ == nullptr) {
            TextRenderer::Draw(
                hdc,
                centerX,
                120,
                L"請點選上方選單「單人計時賽」開始",
                22,
                RGB(0, 0, 0),
                TA_CENTER | TA_BASELINE,
                0,
                L"Microsoft JhengHei UI",
                FW_NORMAL
            );
            return;
        }

        std::wstring targetText = L"目標數字：";
        targetText += std::to_wstring(targetNumber_);

        TextRenderer::Draw(
            hdc,
            centerX,
            105,
            targetText,
            28,
            RGB(0, 0, 180),
            TA_CENTER | TA_BASELINE,
            0,
            L"Microsoft JhengHei UI",
            FW_BOLD
        );

        int cardX = centerX - Card::GetWidth() / 2;
        int cardY = 140;
        currentCard_->Draw(hdc, cardX, cardY);

        if (!stopped_) {
            TextRenderer::Draw(
                hdc,
                centerX,
                360,
                L"當牌面點數與目標數字相同時，按下空白鍵！",
                20,
                RGB(0, 0, 0),
                TA_CENTER | TA_BASELINE,
                0,
                L"Microsoft JhengHei UI",
                FW_NORMAL
            );
        }
        else {
            TextRenderer::Draw(
                hdc,
                centerX,
                370,
                L"已停止，結果請看彈出視窗。",
                22,
                RGB(0, 0, 0),
                TA_CENTER | TA_BASELINE,
                0,
                L"Microsoft JhengHei UI",
                FW_BOLD
            );

            TextRenderer::Draw(
                hdc,
                centerX,
                410,
                L"可再次點選「單人計時賽」重新開始。",
                18,
                RGB(0, 0, 0),
                TA_CENTER | TA_BASELINE,
                0,
                L"Microsoft JhengHei UI",
                FW_NORMAL
            );
        }
    }

private:
    /**
     * @brief 隨機產生目標數字。
     *
     * @return int 1 到 13 之間的整數。
     */
    int GetRandomNumber() const {
        static std::random_device rd;
        static std::mt19937 gen(rd());

        std::uniform_int_distribution<int> dist(1, 13);
        return dist(gen);
    }
};

/**
 * @brief 全域單人計時賽物件。
 */
SingleTimeTrialGame g_game;

/**
 * @brief 開始單人計時賽。
 *
 * @param hwnd 主視窗 Handle。
 */
void StartSingleGame(HWND hwnd) {
    g_game.Start();

    KillTimer(hwnd, ID_TIMER_CARD);
    SetTimer(hwnd, ID_TIMER_CARD, CARD_INTERVAL_MS, NULL);

    SetFocus(hwnd);
    InvalidateRect(hwnd, NULL, TRUE);
}

/**
 * @brief 按下空白鍵後停止遊戲並顯示結果。
 *
 * @param hwnd 主視窗 Handle。
 */
void StopBySpace(HWND hwnd) {
    bool success = g_game.PressSpace();

    if (!success) {
        return;
    }

    KillTimer(hwnd, ID_TIMER_CARD);

    InvalidateRect(hwnd, NULL, FALSE);
    UpdateWindow(hwnd);

    wchar_t timeBuffer[128];

    swprintf_s(
        timeBuffer,
        L"%.2f 秒（%llu 毫秒）",
        g_game.GetReactionTimeSeconds(),
        g_game.GetReactionTimeMs()
    );

    std::wstring msg;

    msg += L"你拍下去的時間：";
    msg += timeBuffer;
    msg += L"\n\n";

    msg += L"目標數字：";
    msg += std::to_wstring(g_game.GetTargetNumber());
    msg += L"\n";

    msg += L"拍下的牌面點數：";
    msg += std::to_wstring(g_game.GetCurrentCardValue());
    msg += L"\n\n";

    msg += g_game.IsCorrect() ? L"判斷結果：正確" : L"判斷結果：錯誤";

    MessageBoxW(
        hwnd,
        msg.c_str(),
        L"單人計時賽結果",
        MB_OK | MB_ICONINFORMATION
    );
}

/**
 * @brief 建立程式上方選單。
 *
 * @return HMENU 建立完成的選單 Handle。
 */
HMENU CreateAppMenu() {
    HMENU hMenuBar = CreateMenu();

    AppendMenuW(hMenuBar, MF_STRING, ID_SINGLE, L"單人計時賽");
    AppendMenuW(hMenuBar, MF_STRING, ID_DOUBLE, L"雙人鍵盤競速");
    AppendMenuW(hMenuBar, MF_STRING, ID_MUTIPLE, L"多人視覺辨識賽");
    AppendMenuW(hMenuBar, MF_STRING, ID_HELP_ABOUT, L"說明");

    return hMenuBar;
}

/**
 * @brief 視窗訊息處理函式。
 *
 * 處理選單、計時器、鍵盤、繪圖與關閉視窗等訊息。
 *
 * @param hwnd 視窗 Handle。
 * @param msg 訊息代碼。
 * @param wParam 訊息參數。
 * @param lParam 訊息參數。
 * @return LRESULT 訊息處理結果。
 */
LRESULT CALLBACK WndProc(HWND hwnd, UINT msg, WPARAM wParam, LPARAM lParam) {
    switch (msg) {
    case WM_COMMAND:
        switch (LOWORD(wParam)) {
        case ID_SINGLE:
            StartSingleGame(hwnd);
            break;

        case ID_DOUBLE:
            MessageBoxW(hwnd, L"雙人鍵盤競速尚未實作。", L"提示", MB_OK | MB_ICONINFORMATION);
            break;

        case ID_MUTIPLE:
            MessageBoxW(hwnd, L"多人視覺辨識賽尚未實作。", L"提示", MB_OK | MB_ICONINFORMATION);
            break;

        case ID_HELP_ABOUT:
            MessageBoxW(
                hwnd,
                L"心臟病.\n單人計時賽：目標數字出現後，等待牌面相同時按空白鍵。",
                L"About",
                MB_OK | MB_ICONINFORMATION
            );
            break;
        }
        break;

    case WM_TIMER:
        if (wParam == ID_TIMER_CARD) {
            if (g_game.IsRunning()) {
                g_game.ChangeCard();
                InvalidateRect(hwnd, NULL, FALSE);
            }
            else {
                KillTimer(hwnd, ID_TIMER_CARD);
            }
        }
        break;

    case WM_KEYDOWN:
        if (wParam == VK_SPACE) {
            StopBySpace(hwnd);
        }
        break;

    case WM_ERASEBKGND:
        return 1;

    case WM_PAINT: {
        PAINTSTRUCT ps;
        HDC hdc = BeginPaint(hwnd, &ps);

        RECT rc;
        GetClientRect(hwnd, &rc);

        int width = rc.right - rc.left;
        int height = rc.bottom - rc.top;

        /**
         * 使用 Double Buffering：
         * 先將畫面繪製在記憶體 DC，
         * 再一次貼到視窗，避免牌面變換時閃爍。
         */
        HDC memDC = CreateCompatibleDC(hdc);
        HBITMAP memBitmap = CreateCompatibleBitmap(hdc, width, height);
        HBITMAP oldBitmap = (HBITMAP)SelectObject(memDC, memBitmap);

        HBRUSH bg = CreateSolidBrush(RGB(255, 255, 255));
        FillRect(memDC, &rc, bg);
        DeleteObject(bg);

        g_game.Draw(memDC, rc);

        BitBlt(
            hdc,
            0,
            0,
            width,
            height,
            memDC,
            0,
            0,
            SRCCOPY
        );

        SelectObject(memDC, oldBitmap);
        DeleteObject(memBitmap);
        DeleteDC(memDC);

        EndPaint(hwnd, &ps);
        break;
    }

    case WM_DESTROY:
        KillTimer(hwnd, ID_TIMER_CARD);
        PostQuitMessage(0);
        break;

    default:
        return DefWindowProcW(hwnd, msg, wParam, lParam);
    }

    return 0;
}

/**
 * @brief Win32 程式進入點。
 *
 * 負責註冊視窗類別、建立視窗、設定選單，
 * 並進入 Win32 訊息迴圈。
 *
 * @param hInstance 程式實例 Handle。
 * @param hPrevInstance 未使用。
 * @param lpCmdLine 命令列參數。
 * @param nCmdShow 視窗顯示方式。
 * @return int 程式結束代碼。
 */
int APIENTRY wWinMain(
    HINSTANCE hInstance,
    HINSTANCE hPrevInstance,
    LPWSTR lpCmdLine,
    int nCmdShow)
{
    const wchar_t CLASS_NAME[] = L"Win32MenuExample";

    WNDCLASSW wc = {};
    wc.lpfnWndProc = WndProc;
    wc.hInstance = hInstance;
    wc.lpszClassName = CLASS_NAME;
    wc.hbrBackground = (HBRUSH)(COLOR_WINDOW + 1);
    wc.hCursor = LoadCursor(NULL, IDC_ARROW);

    if (!RegisterClassW(&wc)) {
        MessageBoxW(NULL, L"Window Registration Failed!", L"Error", MB_ICONERROR);
        return 0;
    }

    HWND hwnd = CreateWindowW(
        CLASS_NAME,
        L"心臟病(第28組:1122036 1132068)",
        WS_OVERLAPPEDWINDOW,
        CW_USEDEFAULT,
        CW_USEDEFAULT,
        700,
        520,
        NULL,
        NULL,
        hInstance,
        NULL
    );

    if (!hwnd) {
        MessageBoxW(NULL, L"Window Creation Failed!", L"Error", MB_ICONERROR);
        return 0;
    }

    SetMenu(hwnd, CreateAppMenu());

    ShowWindow(hwnd, nCmdShow);
    UpdateWindow(hwnd);

    MSG msg;

    while (GetMessageW(&msg, NULL, 0, 0)) {
        /**
         * 防止空白鍵被選單或其他控制項吃掉。
         * 若偵測到空白鍵，直接呼叫 StopBySpace。
         */
        if (msg.message == WM_KEYDOWN && msg.wParam == VK_SPACE) {
            StopBySpace(hwnd);
            continue;
        }

        TranslateMessage(&msg);
        DispatchMessageW(&msg);
    }

    return (int)msg.wParam;
}