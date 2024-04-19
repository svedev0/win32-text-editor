#ifndef UNICODE
#define UNICODE
#endif

#include <windows.h>

#define MENU_FILE_NEW     1001
#define MENU_FILE_OPEN    1002
#define MENU_FILE_SAVE    1003
#define MENU_FILE_SAVE_AS 1004
#define MENU_FILE_EXIT    1005

#define EXAMPLE_FILE_PATH L"C:\\path\\to\\file.c"

const wchar_t *wndClass = L"WinApp";
const wchar_t *wndTitle = L"Text editor";
const int wndWidth = 700;
const int wndHeight = 500;

LRESULT CALLBACK WindowProc(HWND hwnd, UINT uMsg, WPARAM wParam, LPARAM lParam);

void showErrorPopup(const wchar_t *errorMsg);

void showWarningPopup(HWND hWnd, const wchar_t *infoMsg);

void showInfoPopup(HWND hWnd, const wchar_t *infoMsg);

void addMenus(HWND hWnd);

HFONT createFont(int size, int weight, const wchar_t *name);

void addControls(HWND hWnd);

wchar_t *openFile(HWND hWnd);

wchar_t *readFile(const wchar_t *path);

int saveFileAs(HWND hWnd);

int writeFile(const wchar_t *path);
