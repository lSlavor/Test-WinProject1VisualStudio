#include "framework.h"
#include "WinProject1.h"

#define MAX_LOADSTRING 100
#define IDC_COMBOBOX_choice 101
#define IDC_BUTTON_openc 102
#define IDC_EDIT_HEIGHT 103
#define IDC_EDIT_WEIGHT 104
#define IDC_EDIT_BREAKFAST 105
#define IDC_EDIT_LUNCH 106
#define IDC_EDIT_DINNER 107
#define IDC_BUTTON_CALCULATE_BMI 108
#define IDC_BUTTON_CALCULATE_CALORIES 109
#define IDC_BUTTON_CLOSE 110
#define IDC_BUTTON_CLOSE_PROGRAM 111

HINSTANCE hInst;
HWND hMainWindow;  
WCHAR szTitle[MAX_LOADSTRING];
WCHAR szWindowClass[MAX_LOADSTRING];
WCHAR szChildWindowClassBMI[MAX_LOADSTRING] = L"ChildWindowBMI";
WCHAR szChildWindowClassCalories[MAX_LOADSTRING] = L"ChildWindowCalories";

ATOM                MyRegisterClass(HINSTANCE hInstance);
BOOL                InitInstance(HINSTANCE, int);
LRESULT CALLBACK    WndProc(HWND, UINT, WPARAM, LPARAM);
INT_PTR CALLBACK    About(HWND, UINT, WPARAM, LPARAM);
ATOM                RegisterChildWindowClass(HINSTANCE hInstance, LPCWSTR className, WNDPROC childWndProc);
HWND                CreateChildWindow(HWND hParent, LPCWSTR className, LPCWSTR windowTitle, WNDPROC childWndProc);

// Процедуры для дочерних окон
LRESULT CALLBACK    ChildWndProcBMI(HWND, UINT, WPARAM, LPARAM);
LRESULT CALLBACK    ChildWndProcCalories(HWND, UINT, WPARAM, LPARAM);

// Переменные для хранения результатов
double bmiResult = 0.0;
int caloriesResult = 0;

int APIENTRY wWinMain(_In_ HINSTANCE hInstance,
    _In_opt_ HINSTANCE hPrevInstance,
    _In_ LPWSTR    lpCmdLine,
    _In_ int       nCmdShow)
{
    UNREFERENCED_PARAMETER(hPrevInstance);
    UNREFERENCED_PARAMETER(lpCmdLine);

    LoadStringW(hInstance, IDS_APP_TITLE, szTitle, MAX_LOADSTRING);
    LoadStringW(hInstance, IDC_WINPROJECT1, szWindowClass, MAX_LOADSTRING);
    MyRegisterClass(hInstance);

    if (!InitInstance(hInstance, nCmdShow))
    {
        return FALSE;
    }

    HACCEL hAccelTable = LoadAccelerators(hInstance, MAKEINTRESOURCE(IDC_WINPROJECT1));

    MSG msg;

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

ATOM MyRegisterClass(HINSTANCE hInstance)
{
    WNDCLASSEXW wcex;

    wcex.cbSize = sizeof(WNDCLASSEX);

    wcex.style = CS_HREDRAW | CS_VREDRAW;
    wcex.lpfnWndProc = WndProc;
    wcex.cbClsExtra = 0;
    wcex.cbWndExtra = 0;
    wcex.hInstance = hInstance;
    wcex.hIcon = LoadIcon(hInstance, MAKEINTRESOURCE(IDI_WINPROJECT1));
    wcex.hCursor = LoadCursor(nullptr, IDC_ARROW);
    wcex.hbrBackground = (HBRUSH)(COLOR_WINDOW + 1);
    wcex.lpszMenuName = MAKEINTRESOURCEW(IDC_WINPROJECT1);
    wcex.lpszClassName = szWindowClass;
    wcex.hIconSm = LoadIcon(wcex.hInstance, MAKEINTRESOURCE(IDI_SMALL));

    return RegisterClassExW(&wcex);
}

BOOL InitInstance(HINSTANCE hInstance, int nCmdShow)
{
    hInst = hInstance;

    int windowWidth = 800;
    int windowHeight = 600;
    int screenWidth = GetSystemMetrics(SM_CXSCREEN);
    int screenHeight = GetSystemMetrics(SM_CYSCREEN);
    int xPos = (screenWidth - windowWidth) / 2;
    int yPos = (screenHeight - windowHeight) / 2;

    hMainWindow = CreateWindowW(szWindowClass, szTitle, WS_OVERLAPPEDWINDOW,
        xPos, yPos, windowWidth, windowHeight, nullptr, nullptr, hInstance, nullptr);

    if (!hMainWindow)
    {
        return FALSE;
    }

    HWND hComboBox = CreateWindowW(L"COMBOBOX", NULL, WS_CHILD | WS_VISIBLE | CBS_DROPDOWNLIST,
        40, 40, 220, 100, hMainWindow, (HMENU)IDC_COMBOBOX_choice, hInstance, NULL);

    SendMessage(hComboBox, CB_ADDSTRING, 0, (LPARAM)L"Калькулятор ИМТ");
    SendMessage(hComboBox, CB_ADDSTRING, 0, (LPARAM)L"Калькулятор каллорий");

    HWND hButtonOpen = CreateWindowW(L"BUTTON", L"Открыть", WS_CHILD | WS_VISIBLE | BS_PUSHBUTTON,
        275, 36, 100, 30, hMainWindow, (HMENU)IDC_BUTTON_openc, hInstance, NULL);

    // Добавляем кнопку для закрытия программы
    HWND hButtonCloseProgram = CreateWindowW(L"BUTTON", L"Закрыть программу", WS_CHILD | WS_VISIBLE | BS_PUSHBUTTON,
        275, 100, 150, 30, hMainWindow, (HMENU)IDC_BUTTON_CLOSE_PROGRAM, hInstance, NULL);

    ShowWindow(hMainWindow, nCmdShow);
    UpdateWindow(hMainWindow);

    return TRUE;
}


LRESULT CALLBACK WndProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam)
{
    static HWND hComboBox;

    switch (message)
    {
    case WM_CREATE:
        hComboBox = GetDlgItem(hWnd, IDC_COMBOBOX_choice);
        break;

    case WM_COMMAND:
        if (LOWORD(wParam) == IDC_BUTTON_openc)
        {
            hComboBox = GetDlgItem(hWnd, IDC_COMBOBOX_choice);
            int index = SendMessage(hComboBox, CB_GETCURSEL, 0, 0);
            if (index == 0)
            {
                EnableWindow(hWnd, FALSE);  // Отключаем главное окно
                CreateChildWindow(hWnd, szChildWindowClassBMI, L"Калькулятор ИМТ", ChildWndProcBMI);
            }
            else if (index == 1)
            {
                EnableWindow(hWnd, FALSE);  // Отключаем главное окно
                CreateChildWindow(hWnd, szChildWindowClassCalories, L"Калькулятор каллорий", ChildWndProcCalories);
            }
            else
            {
                MessageBox(hWnd, L"Выберите пункт из списка", L"Ошибка", MB_OK);
            }
        }
        else if (LOWORD(wParam) == IDC_BUTTON_CLOSE_PROGRAM)
        {
            PostQuitMessage(0);  // Завершить программу
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


ATOM RegisterChildWindowClass(HINSTANCE hInstance, LPCWSTR className, WNDPROC childWndProc)
{
    WNDCLASSEXW wcex;
    wcex.cbSize = sizeof(WNDCLASSEX);
    wcex.style = CS_HREDRAW | CS_VREDRAW;
    wcex.lpfnWndProc = childWndProc;
    wcex.cbClsExtra = 0;
    wcex.cbWndExtra = 0;
    wcex.hInstance = hInstance;
    wcex.hIcon = LoadIcon(nullptr, IDI_APPLICATION);
    wcex.hCursor = LoadCursor(nullptr, IDC_ARROW);
    wcex.hbrBackground = (HBRUSH)(COLOR_WINDOW + 1);
    wcex.lpszMenuName = nullptr;
    wcex.lpszClassName = className;
    wcex.hIconSm = LoadIcon(nullptr, IDI_APPLICATION);

    return RegisterClassExW(&wcex);
}

HWND CreateChildWindow(HWND hParent, LPCWSTR className, LPCWSTR windowTitle, WNDPROC childWndProc)
{
    RegisterChildWindowClass(hInst, className, childWndProc);

    int windowWidth = 400;
    int windowHeight = 500;
    int screenWidth = GetSystemMetrics(SM_CXSCREEN);
    int screenHeight = GetSystemMetrics(SM_CYSCREEN);
    int xPos = (screenWidth - windowWidth) / 2;
    int yPos = (screenHeight - windowHeight) / 2;

    HWND hWndChild = CreateWindowW(className, windowTitle, WS_OVERLAPPEDWINDOW,
        xPos, yPos, windowWidth, windowHeight, hParent, nullptr, hInst, nullptr);

    if (hWndChild)
    {
        ShowWindow(hWndChild, SW_SHOW);
        UpdateWindow(hWndChild);
    }
    return hWndChild;
}

// Процедура для окна ИМТ
LRESULT CALLBACK ChildWndProcBMI(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam)
{
    static HWND hEditHeight, hEditWeight, hButtonCalculate, hButtonClose;

    switch (message)
    {
    case WM_CREATE:
        hEditHeight = CreateWindowW(L"EDIT", NULL, WS_CHILD | WS_VISIBLE | WS_BORDER | ES_NUMBER,
            50, 50, 150, 20, hWnd, (HMENU)IDC_EDIT_HEIGHT, hInst, NULL);
        CreateWindowW(L"STATIC", L"Рост (см):", WS_CHILD | WS_VISIBLE,
            50, 30, 100, 20, hWnd, NULL, hInst, NULL);

        hEditWeight = CreateWindowW(L"EDIT", NULL, WS_CHILD | WS_VISIBLE | WS_BORDER | ES_NUMBER,
            50, 100, 150, 20, hWnd, (HMENU)IDC_EDIT_WEIGHT, hInst, NULL);
        CreateWindowW(L"STATIC", L"Вес (кг):", WS_CHILD | WS_VISIBLE,
            50, 80, 100, 20, hWnd, NULL, hInst, NULL);

        hButtonCalculate = CreateWindowW(L"BUTTON", L"Рассчитать ИМТ", WS_CHILD | WS_VISIBLE | BS_PUSHBUTTON,
            50, 150, 150, 30, hWnd, (HMENU)IDC_BUTTON_CALCULATE_BMI, hInst, NULL);

        hButtonClose = CreateWindowW(L"BUTTON", L"Закрыть", WS_CHILD | WS_VISIBLE | BS_PUSHBUTTON,
            50, 200, 150, 30, hWnd, (HMENU)IDC_BUTTON_CLOSE, hInst, NULL);
        break;

    case WM_COMMAND:
        if (LOWORD(wParam) == IDC_BUTTON_CALCULATE_BMI)
        {
            WCHAR buffer[100];
            GetWindowText(hEditHeight, buffer, 100);
            double height = _wtof(buffer) / 100; // Convert to meters
            GetWindowText(hEditWeight, buffer, 100);
            double weight = _wtof(buffer);
            bmiResult = weight / (height * height);
            swprintf(buffer, 100, L"Ваш ИМТ: %.2f", bmiResult);
            MessageBox(hWnd, buffer, L"Результат", MB_OK);
        }
        else if (LOWORD(wParam) == IDC_BUTTON_CLOSE)
        {
            EnableWindow(hMainWindow, TRUE);    // Включить главное окно
            ShowWindow(hMainWindow, SW_RESTORE);  // Явно развернуть главное окно
            SetFocus(hMainWindow);              // Вернуть фокус главному окну
            DestroyWindow(hWnd);                // Уничтожить дочернее окно
        }
        break;

    case WM_DESTROY:
        EnableWindow(hMainWindow, TRUE);    // Включить главное окно
        ShowWindow(hMainWindow, SW_RESTORE);  // Развернуть главное окно
        SetFocus(hMainWindow);              // Вернуть фокус главному окну
        break;

    default:
        return DefWindowProc(hWnd, message, wParam, lParam);
    }
    return 0;
}

// Процедура для окна калорий
LRESULT CALLBACK ChildWndProcCalories(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam)
{
    static HWND hEditBreakfast, hEditLunch, hEditDinner, hButtonCalculate, hButtonClose;

    switch (message)
    {
    case WM_CREATE:
        hEditBreakfast = CreateWindowW(L"EDIT", NULL, WS_CHILD | WS_VISIBLE | WS_BORDER | ES_NUMBER,
            50, 50, 150, 20, hWnd, (HMENU)IDC_EDIT_BREAKFAST, hInst, NULL);
        CreateWindowW(L"STATIC", L"Завтрак (ккал):", WS_CHILD | WS_VISIBLE,
            50, 30, 150, 20, hWnd, NULL, hInst, NULL);

        hEditLunch = CreateWindowW(L"EDIT", NULL, WS_CHILD | WS_VISIBLE | WS_BORDER | ES_NUMBER,
            50, 100, 150, 20, hWnd, (HMENU)IDC_EDIT_LUNCH, hInst, NULL);
        CreateWindowW(L"STATIC", L"Обед (ккал):", WS_CHILD | WS_VISIBLE,
            50, 80, 150, 20, hWnd, NULL, hInst, NULL);

        hEditDinner = CreateWindowW(L"EDIT", NULL, WS_CHILD | WS_VISIBLE | WS_BORDER | ES_NUMBER,
            50, 150, 150, 20, hWnd, (HMENU)IDC_EDIT_DINNER, hInst, NULL);
        CreateWindowW(L"STATIC", L"Ужин (ккал):", WS_CHILD | WS_VISIBLE,
            50, 130, 150, 20, hWnd, NULL, hInst, NULL);

        hButtonCalculate = CreateWindowW(L"BUTTON", L"Посчитать калории", WS_CHILD | WS_VISIBLE | BS_PUSHBUTTON,
            50, 200, 150, 30, hWnd, (HMENU)IDC_BUTTON_CALCULATE_CALORIES, hInst, NULL);

        hButtonClose = CreateWindowW(L"BUTTON", L"Закрыть", WS_CHILD | WS_VISIBLE | BS_PUSHBUTTON,
            50, 250, 150, 30, hWnd, (HMENU)IDC_BUTTON_CLOSE, hInst, NULL);
        break;

    case WM_COMMAND:
        if (LOWORD(wParam) == IDC_BUTTON_CALCULATE_CALORIES)
        {
            WCHAR buffer[100];
            GetWindowText(hEditBreakfast, buffer, 100);
            int breakfast = _wtoi(buffer);
            GetWindowText(hEditLunch, buffer, 100);
            int lunch = _wtoi(buffer);
            GetWindowText(hEditDinner, buffer, 100);
            int dinner = _wtoi(buffer);
            caloriesResult = breakfast + lunch + dinner;
            swprintf(buffer, 100, L"Общие калории: %d", caloriesResult);
            MessageBox(hWnd, buffer, L"Результат", MB_OK);
        }
        else if (LOWORD(wParam) == IDC_BUTTON_CLOSE)
        {
            EnableWindow(hMainWindow, TRUE);    // Включить главное окно
            ShowWindow(hMainWindow, SW_RESTORE);  // Явно развернуть главное окно
            SetFocus(hMainWindow);              // Вернуть фокус главному окну
            DestroyWindow(hWnd);                // Уничтожить дочернее окно
        }
        break;

    case WM_DESTROY:
        EnableWindow(hMainWindow, TRUE);    // Включить главное окно
        ShowWindow(hMainWindow, SW_RESTORE);  // Развернуть главное окно
        SetFocus(hMainWindow);              // Вернуть фокус главному окну
        break;

    default:
        return DefWindowProc(hWnd, message, wParam, lParam);
    }
    return 0;
}
