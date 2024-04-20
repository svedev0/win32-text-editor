#include "main.h"
#include <windows.h>

HWND hEdit;
HWND hPath;
HBRUSH hWndBkBrush;
HBRUSH hEditBkBrush;
HBRUSH hPathBkBrush;

int WINAPI wWinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, PWSTR pCmdLine, int nCmdShow) {
    WNDCLASSEX wcex;

    wcex.cbClsExtra = 0;
    wcex.cbSize = sizeof(WNDCLASSEX);
    wcex.cbWndExtra = 0;
    wcex.hbrBackground = (HBRUSH) (2);
    wcex.hCursor = LoadCursor(NULL, IDC_ARROW);
    wcex.hInstance = hInstance;
    wcex.lpfnWndProc = WindowProc;
    wcex.lpszClassName = WND_CLASS;
    wcex.lpszMenuName = NULL;
    wcex.style = CS_HREDRAW | CS_VREDRAW;

    if (!RegisterClassExW(&wcex)) {
        showErrorPopup(L"RegisterClassExW failed!");
        return EXIT_FAILURE;
    }

    HWND hWnd = CreateWindowExW(
        0L,
        WND_CLASS,
        WND_TITLE,
        WS_OVERLAPPED | WS_CAPTION | WS_SYSMENU | WS_MINIMIZEBOX,
        CW_USEDEFAULT,
        CW_USEDEFAULT,
        CW_USEDEFAULT,
        CW_USEDEFAULT,
        NULL,
        NULL,
        hInstance,
        NULL);

    if (!hWnd) {
        showErrorPopup(L"hWnd = CreateWindowExW failed!");
        return EXIT_FAILURE;
    }

    ShowWindow(hWnd, nCmdShow);
    SetWindowPos(hWnd, HWND_TOP, 0, 0, WND_WIDTH, WND_HEIGHT, SWP_NOMOVE);
    UpdateWindow(hWnd);

    MSG msg;

    while (GetMessage(&msg, NULL, 0, 0)) {
        TranslateMessage(&msg);
        DispatchMessage(&msg);
    }

    return EXIT_SUCCESS;
}

LRESULT CALLBACK WindowProc(HWND hWnd, UINT msg, WPARAM wp, LPARAM lp) {
    COLORREF textColor;
    COLORREF bkColor;

    switch (msg) {
    case WM_CREATE:
        addMenus(hWnd);
        addControls(hWnd);
        break;

    case WM_COMMAND:
        switch (wp) {
        case MENU_FILE_NEW:
            SetWindowTextW(hEdit, L"");
            SetWindowTextW(hPath, L"");
            break;

        case WM_PAINT:
            PAINTSTRUCT ps;
            HDC hdc = BeginPaint(hWnd, &ps);
            hWndBkBrush = CreateSolidBrush(RGB(0, 0, 0));
            FillRect(hdc, &ps.rcPaint, hWndBkBrush);
            EndPaint(hWnd, &ps);
            break;

        case MENU_FILE_OPEN:
            wchar_t *content = openFile(hWnd);
            if (content == NULL) {
                break;
            }

            SetWindowTextW(hEdit, content);
            free(content);
            break;

        case MENU_FILE_SAVE:
            int wPathLength = GetWindowTextLengthW(hPath);
            wchar_t *wPathBuffer = (wchar_t *) malloc(wPathLength * sizeof(wchar_t));
            if (wPathBuffer == NULL) {
                break;
            }

            GetWindowTextW(hPath, wPathBuffer, wPathLength * sizeof(wchar_t));
            if (writeFile(wPathBuffer) == 0) {
                showInfoPopup(hWnd, L"File saved");
            }
            break;

        case MENU_FILE_SAVE_AS:
            if (saveFileAs(hWnd) == 0) {
                showInfoPopup(hWnd, L"File saved");
            }
            break;

        case MENU_FILE_EXIT:
            int result = MessageBoxW(
                hWnd, L"Are you sure you want to quit?", L"Warning", MB_YESNO | MB_ICONWARNING);
            if (result == IDYES) {
                DestroyWindow(hWnd);
            }
            break;

        case MENU_ABOUT_AUTHOR:
            ShellExecuteW(NULL, L"open", AUTHOR_URL, NULL, NULL, SW_SHOWNORMAL);
            break;
        }
        break;

    case WM_CTLCOLOREDIT:
        HDC hDcEdit = (HDC) wp;
        HWND hWndEdit = (HWND) lp;
        textColor = RGB(255, 255, 255);
        bkColor = RGB(55, 55, 55);

        SetTextColor(hDcEdit, textColor);
        SetBkColor(hDcEdit, bkColor);
        hEditBkBrush = CreateSolidBrush(bkColor);
        return (LRESULT) hEditBkBrush;

    case WM_CTLCOLORSTATIC:
        HDC hDcStatic = (HDC) wp;
        textColor = RGB(255, 255, 255);
        bkColor = RGB(20, 20, 20);

        SetTextColor(hDcStatic, textColor);
        SetBkColor(hDcStatic, bkColor);
        hPathBkBrush = CreateSolidBrush(bkColor);
        return (LRESULT) hPathBkBrush;

    case WM_CLOSE:
        DestroyWindow(hWnd);
        break;

    case WM_DESTROY:
        PostQuitMessage(0);
        break;
    }

    return DefWindowProc(hWnd, msg, wp, lp);
}

void showErrorPopup(const wchar_t *msg) {
    MessageBoxW(NULL, msg, L"Error", MB_ICONERROR);
}

void showInfoPopup(HWND hWnd, const wchar_t *msg) {
    MessageBoxW(hWnd, msg, L"Information", MB_ICONINFORMATION);
}

void addMenus(HWND hWnd) {
    HMENU hFileMenu = CreateMenu();
    AppendMenuW(hFileMenu, MF_STRING, MENU_FILE_NEW, L"New");
    AppendMenuW(hFileMenu, MF_STRING, MENU_FILE_OPEN, L"Open");
    AppendMenuW(hFileMenu, MF_STRING, MENU_FILE_SAVE, L"Save");
    AppendMenuW(hFileMenu, MF_STRING, MENU_FILE_SAVE_AS, L"Save as");
    AppendMenuW(hFileMenu, MF_STRING, MENU_FILE_EXIT, L"Exit");

    HMENU hAboutMenu = CreateMenu();
    AppendMenuW(hAboutMenu, MF_STRING, MENU_ABOUT_AUTHOR, L"Author: svedev0");

    HMENU hMenu = CreateMenu();
    AppendMenuW(hMenu, MF_POPUP, (UINT_PTR) hFileMenu, L"File");
    AppendMenuW(hMenu, MF_POPUP, (UINT_PTR) hAboutMenu, L"About");

    SetMenu(hWnd, hMenu);
}

HFONT createFont(int size, int weight, const wchar_t *name) {
    return CreateFontW(
        size, (size / 2), 0, 0, weight, 0, 0, 0, DEFAULT_CHARSET, 0, 0, CLEARTYPE_QUALITY, 0, name);
}

void addControls(HWND hWnd) {
    hEdit = CreateWindowExW(
        WS_EX_CLIENTEDGE,
        L"Edit",
        EXAMPLE_FILE_TEXT,
        WS_CHILD | WS_VISIBLE | ES_MULTILINE | ES_NOHIDESEL | WS_VSCROLL | WS_HSCROLL,
        -2,
        -2,
        WND_WIDTH - 14,
        WND_HEIGHT - 82,
        hWnd,
        NULL,
        NULL,
        NULL);

    HFONT hEditFont = createFont(22, 500, L"Consolas");
    SendMessageW(hEdit, WM_SETFONT, (WPARAM) hEditFont, 0);
    SendMessageW(hEdit, EM_SETMARGINS, EC_LEFTMARGIN | EC_RIGHTMARGIN, MAKELPARAM(5, 5));

    hPath = CreateWindowExW(
        0L,
        L"Static",
        EXAMPLE_FILE_PATH,
        WS_CHILD | WS_VISIBLE,
        0,
        WND_HEIGHT - 80,
        WND_WIDTH,
        80,
        hWnd,
        NULL,
        NULL,
        NULL);

    HFONT hPathFont = createFont(18, 400, L"Consolas");
    SendMessageW(hPath, WM_SETFONT, (WPARAM) hPathFont, 0);
}

wchar_t *openFile(HWND hWnd) {
    OPENFILENAME ofn = {0};
    wchar_t fileName[MAX_PATH];

    ofn.lStructSize = sizeof(OPENFILENAME);
    ofn.hwndOwner = hWnd;
    ofn.lpstrFilter = L"All files (*.*)\0*.*\0\0";
    ofn.lpstrFile = fileName;
    ofn.lpstrFile[0] = '\0';
    ofn.nMaxFile = MAX_PATH;
    ofn.lpstrInitialDir = L"C:\\";

    if (!GetOpenFileNameW(&ofn)) {
        return NULL;
    }

    SetWindowTextW(hPath, ofn.lpstrFile);
    return readFile(ofn.lpstrFile);
}

wchar_t *readFile(const wchar_t *path) {
    HANDLE hFile = CreateFileW(
        path, GENERIC_READ, FILE_SHARE_READ, NULL, OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, NULL);
    if (hFile == INVALID_HANDLE_VALUE) {
        showErrorPopup(L"Failed to open file");
        return NULL;
    }

    DWORD fileSize = GetFileSize(hFile, NULL);
    if (fileSize == INVALID_FILE_SIZE) {
        CloseHandle(hFile);
        showErrorPopup(L"Failed to get file size");
        return NULL;
    }

    char *buffer = (char *) malloc(fileSize + 1);
    if (buffer == NULL) {
        CloseHandle(hFile);
        showErrorPopup(L"Memory allocation failed");
        return NULL;
    }

    DWORD bytesRead;
    if (!ReadFile(hFile, buffer, fileSize, &bytesRead, NULL) || bytesRead != fileSize) {
        CloseHandle(hFile);
        free(buffer);
        showErrorPopup(L"Failed to read file");
        return NULL;
    }

    buffer[fileSize] = L'\0';

    int wCharsCount = MultiByteToWideChar(CP_UTF8, 0, buffer, -1, NULL, 0);
    wchar_t *wBuffer = (wchar_t *) malloc(wCharsCount * sizeof(wchar_t));
    if (wBuffer == NULL) {
        CloseHandle(hFile);
        free(buffer);
        showErrorPopup(L"Memory allocation for wide buffer failed");
        return NULL;
    }
    MultiByteToWideChar(CP_UTF8, 0, buffer, -1, wBuffer, wCharsCount);

    CloseHandle(hFile);
    free(buffer);

    return wBuffer;
}

int saveFileAs(HWND hWnd) {
    OPENFILENAME ofn = {0};
    wchar_t fileName[MAX_PATH];

    ofn.lStructSize = sizeof(OPENFILENAME);
    ofn.hwndOwner = hWnd;
    ofn.lpstrFilter = L"All files (*.*)\0*.*\0\0";
    ofn.lpstrFile = fileName;
    ofn.lpstrFile[0] = '\0';
    ofn.nMaxFile = MAX_PATH;
    ofn.lpstrInitialDir = L"C:\\";

    GetSaveFileNameW(&ofn);
    if (ofn.lpstrFile[0] == '\0') {
        return 1;
    }
    return writeFile(ofn.lpstrFile);
}

int writeFile(const wchar_t *path) {
    HANDLE hFile;
    BOOL result;
    DWORD bytesWritten;

    if (lstrcmp(path, EXAMPLE_FILE_PATH) == 0) {
        return 1;
    }

    int wLength = GetWindowTextLengthW(hEdit);
    wchar_t *wBuffer = (wchar_t *) malloc(wLength * sizeof(wchar_t));
    GetWindowTextW(hEdit, wBuffer, wLength * sizeof(wchar_t));

    int length = WideCharToMultiByte(CP_UTF8, 0, wBuffer, -1, NULL, 0, NULL, NULL);
    if (length == 0) {
        showErrorPopup(L"Failed to calculate length of text");
        free(wBuffer);
        return 1;
    }

    char *buffer = (char *) malloc(length);
    if (buffer == NULL) {
        showErrorPopup(L"Failed to allocate memory for text");
        free(wBuffer);
        return 1;
    }

    int ret = WideCharToMultiByte(CP_UTF8, 0, wBuffer, -1, buffer, length, NULL, NULL);
    if (ret == 0) {
        showErrorPopup(L"Conversion to UTF-8 failed");
        free(wBuffer);
        free(buffer);
        return 1;
    }

    hFile = CreateFileW(path, GENERIC_WRITE, 0, NULL, CREATE_ALWAYS, FILE_ATTRIBUTE_NORMAL, NULL);
    if (hFile == INVALID_HANDLE_VALUE) {
        showErrorPopup(L"Failed to write to file");
        free(wBuffer);
        free(buffer);
        return 1;
    }

    result = WriteFile(hFile, buffer, strlen(buffer), &bytesWritten, NULL);
    if (!result) {
        showErrorPopup(L"Failed to write to file");
        CloseHandle(hFile);
        free(wBuffer);
        free(buffer);
        return 1;
    }

    CloseHandle(hFile);
    return 0;
}
