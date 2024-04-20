#ifndef UNICODE
#define UNICODE
#endif

#include <windows.h>

#define WND_CLASS  L"MainWndClass"
#define WND_TITLE  L"Text editor"
#define WND_WIDTH  800
#define WND_HEIGHT 600

#define MENU_FILE_NEW     1001
#define MENU_FILE_OPEN    1002
#define MENU_FILE_SAVE    1003
#define MENU_FILE_SAVE_AS 1004
#define MENU_FILE_EXIT    1005
#define MENU_ABOUT_AUTHOR 2001

#define EXAMPLE_FILE_PATH L"C:\\path\\to\\file.c"
#define AUTHOR_URL        L"https://github.com/svedev0"

LRESULT CALLBACK WindowProc(HWND hwnd, UINT msg, WPARAM wp, LPARAM lp);

void showErrorPopup(const wchar_t *msg);

void showInfoPopup(HWND hWnd, const wchar_t *msg);

void addMenus(HWND hWnd);

HFONT createFont(int size, int weight, const wchar_t *name);

void addControls(HWND hWnd);

wchar_t *openFile(HWND hWnd);

wchar_t *readFile(const wchar_t *path);

int saveFileAs(HWND hWnd);

int writeFile(const wchar_t *path);
