
#define UNICODE
#define _UNICODE
#define NOMINMAX

#include <windows.h>
#include <commdlg.h>
#include <shellapi.h>
#include <gdiplus.h>

#include <string>
#include <vector>
#include <memory>
#include <algorithm>

using namespace Gdiplus;
using namespace std;

// =====================================
// APPLICATION SETTINGS
// =====================================

#define ID_OPEN       1001
#define ID_PREVIOUS   1002
#define ID_NEXT       1003
#define ID_ZOOM_IN    1004
#define ID_ZOOM_OUT   1005
#define ID_FIT        1006

#define TOOLBAR_HEIGHT 48

ULONG_PTR gdiplusToken = 0;

unique_ptr<Image> currentImage;

vector<wstring> imageFiles;

int currentIndex = -1;

float zoom = 1.0f;

wstring currentFile;

HWND btnZoomIn = NULL;
HWND btnZoomOut = NULL;
HWND btnFit = NULL;

// =====================================
// CHECK IMAGE FILE EXTENSION
// =====================================

bool IsImageFile(const wstring& file)
{
    size_t pos = file.find_last_of(L'.');

    if (pos == wstring::npos)
        return false;

    wstring ext = file.substr(pos);

    return ext == L".jpg" ||
           ext == L".JPG" ||
           ext == L".jpeg" ||
           ext == L".JPEG" ||
           ext == L".png" ||
           ext == L".PNG" ||
           ext == L".bmp" ||
           ext == L".BMP" ||
           ext == L".gif" ||
           ext == L".GIF";
}

// =====================================
// SCAN IMAGE FILES
// =====================================

void ScanFolder(const wstring& file)
{
    imageFiles.clear();

    size_t pos = file.find_last_of(L"\\/");

    if (pos == wstring::npos)
        return;

    wstring folder = file.substr(0, pos + 1);

    WIN32_FIND_DATAW data;

    HANDLE hFind = FindFirstFileW(
        (folder + L"*").c_str(),
        &data
    );

    if (hFind == INVALID_HANDLE_VALUE)
        return;

    do
    {
        if (!(data.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY))
        {
            wstring name = data.cFileName;

            if (IsImageFile(name))
                imageFiles.push_back(folder + name);
        }

    } while (FindNextFileW(hFind, &data));

    FindClose(hFind);

    sort(
        imageFiles.begin(),
        imageFiles.end(),
        [](const wstring& a, const wstring& b)
        {
            return _wcsicmp(a.c_str(), b.c_str()) < 0;
        }
    );
}

// =====================================
// UPDATE WINDOW TITLE
// =====================================

void UpdateTitle(HWND hwnd)
{
    wstring title = L"MiniPhoto Viewer 1.0";

    if (currentImage)
    {
        size_t pos = currentFile.find_last_of(L"\\/");

        title += L" - ";
        title += currentFile.substr(pos + 1);
    }

    SetWindowTextW(hwnd, title.c_str());
}

// =====================================
// LOAD IMAGE
// =====================================

void LoadImageFile(HWND hwnd, const wstring& file)
{
    unique_ptr<Image> img(
        new Image(file.c_str())
    );

    if (img->GetLastStatus() != Ok)
    {
        MessageBoxW(
            hwnd,
            L"Unable to open this image!",
            L"MiniPhoto Viewer",
            MB_OK | MB_ICONERROR
        );

        return;
    }

    currentImage = move(img);

    currentFile = file;

    zoom = 1.0f;

    ScanFolder(file);

    currentIndex = -1;

    for (int i = 0; i < (int)imageFiles.size(); i++)
    {
        if (_wcsicmp(
            imageFiles[i].c_str(),
            file.c_str()) == 0)
        {
            currentIndex = i;
            break;
        }
    }

    UpdateTitle(hwnd);

    InvalidateRect(hwnd, NULL, TRUE);
}

// =====================================
// OPEN IMAGE DIALOG
// =====================================

void OpenImage(HWND hwnd)
{
    wchar_t filename[MAX_PATH] = L"";

    OPENFILENAMEW ofn = {};

    ofn.lStructSize = sizeof(ofn);

    ofn.hwndOwner = hwnd;

    ofn.lpstrFilter =
        L"Image Files\0*.jpg;*.jpeg;*.png;*.bmp;*.gif\0"
        L"All Files\0*.*\0";

    ofn.lpstrFile = filename;

    ofn.nMaxFile = MAX_PATH;

    ofn.Flags =
        OFN_FILEMUSTEXIST |
        OFN_PATHMUSTEXIST;

    ofn.lpstrTitle = L"Select an Image";

    if (GetOpenFileNameW(&ofn)) {
        LoadImageFile(hwnd, filename);
    }
}

// =====================================
// PREVIOUS IMAGE
// =====================================

void PreviousImage(HWND hwnd)
{
    if (imageFiles.empty() || currentIndex < 0)
        return;

    if (currentIndex > 0)
        currentIndex--;
    else
        currentIndex = (int)imageFiles.size() - 1;

    LoadImageFile(hwnd, imageFiles[currentIndex]);
}

// =====================================
// NEXT IMAGE
// =====================================

void NextImage(HWND hwnd)
{
    if (imageFiles.empty() || currentIndex < 0)
        return;

    if (currentIndex < (int)imageFiles.size() - 1)
        currentIndex++;
    else
        currentIndex = 0;

    LoadImageFile(hwnd, imageFiles[currentIndex]);
}

// =====================================
// ZOOM FUNCTIONS
// =====================================

void ZoomIn(HWND hwnd)
{
    zoom = min(zoom * 1.25f, 10.0f);

    InvalidateRect(hwnd, NULL, TRUE);
}

void ZoomOut(HWND hwnd)
{
    zoom = max(zoom / 1.25f, 0.1f);

    InvalidateRect(hwnd, NULL, TRUE);
}

void FitImage(HWND hwnd)
{
    zoom = 1.0f;

    InvalidateRect(hwnd, NULL, TRUE);
}

// =====================================
// DRAW IMAGE
// =====================================

void DrawPhoto(HWND hwnd, HDC hdc)
{
    RECT rc;

    GetClientRect(hwnd, &rc);

    Graphics graphics(hdc);

    graphics.Clear(Color(255, 30, 30, 30));

    graphics.SetInterpolationMode(
        InterpolationModeHighQualityBicubic
    );

    if (!currentImage)
    {
        SolidBrush brush(Color(255, 220, 220, 220));

        FontFamily family(L"Segoe UI");

        Font font(&family, 18);

        StringFormat format;

        format.SetAlignment(StringAlignmentCenter);

        format.SetLineAlignment(StringAlignmentCenter);

        RectF area(
            0,
            TOOLBAR_HEIGHT,
            (REAL)rc.right,
            (REAL)(rc.bottom - TOOLBAR_HEIGHT)
        );

        graphics.DrawString(
            L"MiniPhoto Viewer\n\nPress Ctrl + O to open an image",
            -1,
            &font,
            area,
            &format,
            &brush
        );

        return;
    }

    int imageWidth = currentImage->GetWidth();

    int imageHeight = currentImage->GetHeight();

    if (imageWidth <= 0 || imageHeight <= 0)
        return;

    int windowWidth = rc.right;

    int windowHeight = rc.bottom - TOOLBAR_HEIGHT;

    float scaleX = (float)windowWidth / imageWidth;

    float scaleY = (float)windowHeight / imageHeight;

    float scale = min(scaleX, scaleY) * zoom;

    int drawWidth = (int)(imageWidth * scale);

    int drawHeight = (int)(imageHeight * scale);

    int x = (windowWidth - drawWidth) / 2;

    int y = TOOLBAR_HEIGHT +
            (windowHeight - drawHeight) / 2;

    graphics.DrawImage(
        currentImage.get(),
        x,
        y,
        drawWidth,
        drawHeight
    );
}

// =====================================
// CREATE TOOLBAR
// =====================================

void CreateToolbar(HWND hwnd)
{
    HFONT font = (HFONT)GetStockObject(DEFAULT_GUI_FONT);

    btnZoomOut = CreateWindowW(
        L"BUTTON",
        L"-",
        WS_CHILD | WS_VISIBLE | WS_TABSTOP | BS_PUSHBUTTON,
        10, 8, 42, 30,
        hwnd,
        (HMENU)(INT_PTR)ID_ZOOM_OUT,
        NULL,
        NULL
    );

    btnZoomIn = CreateWindowW(
        L"BUTTON",
        L"+",
        WS_CHILD | WS_VISIBLE | WS_TABSTOP | BS_PUSHBUTTON,
        60, 8, 42, 30,
        hwnd,
        (HMENU)(INT_PTR)ID_ZOOM_IN,
        NULL,
        NULL
    );

    btnFit = CreateWindowW(
        L"BUTTON",
        L"FIT",
        WS_CHILD | WS_VISIBLE | WS_TABSTOP | BS_PUSHBUTTON,
        110, 8, 55, 30,
        hwnd,
        (HMENU)(INT_PTR)ID_FIT,
        NULL,
        NULL
    );

    SendMessageW(btnZoomOut, WM_SETFONT, (WPARAM)font, TRUE);

    SendMessageW(btnZoomIn, WM_SETFONT, (WPARAM)font, TRUE);

    SendMessageW(btnFit, WM_SETFONT, (WPARAM)font, TRUE);
}

// =====================================
// CREATE APPLICATION MENU
// =====================================

void CreateAppMenu(HWND hwnd)
{
    HMENU menu = CreateMenu();

    HMENU fileMenu = CreatePopupMenu();

    AppendMenuW(
        fileMenu,
        MF_STRING,
        ID_OPEN,
        L"Open Image...\tCtrl+O"
    );

    AppendMenuW(
        fileMenu,
        MF_SEPARATOR,
        0,
        NULL
    );

    AppendMenuW(
        fileMenu,
        MF_STRING,
        ID_PREVIOUS,
        L"Previous Image\tLeft"
    );

    AppendMenuW(
        fileMenu,
        MF_STRING,
        ID_NEXT,
        L"Next Image\tRight"
    );

    AppendMenuW(
        menu,
        MF_POPUP,
        (UINT_PTR)fileMenu,
        L"File"
    );

    SetMenu(hwnd, menu);
}

// =====================================
// WINDOWS MESSAGE HANDLER
// =====================================

LRESULT CALLBACK WindowProc(
    HWND hwnd,
    UINT msg,
    WPARAM wParam,
    LPARAM lParam)
{
    switch (msg)
    {
        case WM_CREATE:
        {
            CreateAppMenu(hwnd);

            CreateToolbar(hwnd);

            return 0;
        }

        case WM_COMMAND:
        {
            switch (LOWORD(wParam))
            {
                case ID_OPEN:
                    OpenImage(hwnd);
                    break;

                case ID_PREVIOUS:
                    PreviousImage(hwnd);
                    break;

                case ID_NEXT:
                    NextImage(hwnd);
                    break;

                case ID_ZOOM_IN:
                    ZoomIn(hwnd);
                    break;

                case ID_ZOOM_OUT:
                    ZoomOut(hwnd);
                    break;

                case ID_FIT:
                    FitImage(hwnd);
                    break;
            }

            return 0;
        }

        case WM_KEYDOWN:
        {
            if ((GetKeyState(VK_CONTROL) & 0x8000) &&
                wParam == 'O')
            {
                OpenImage(hwnd);
            }
            else if (wParam == VK_LEFT)
            {
                PreviousImage(hwnd);
            }
            else if (wParam == VK_RIGHT)
            {
                NextImage(hwnd);
            }
            else if (wParam == VK_ADD ||
                     wParam == VK_OEM_PLUS)
            {
                ZoomIn(hwnd);
            }
            else if (wParam == VK_SUBTRACT ||
                     wParam == VK_OEM_MINUS)
            {
                ZoomOut(hwnd);
            }
            else if (wParam == '0')
            {
                FitImage(hwnd);
            }

            return 0;
        }

        case WM_PAINT:
        {
            PAINTSTRUCT ps;

            HDC hdc = BeginPaint(hwnd, &ps);

            DrawPhoto(hwnd, hdc);

            EndPaint(hwnd, &ps);

            return 0;
        }

        case WM_DESTROY:
        {
            currentImage.reset();

            PostQuitMessage(0);

            return 0;
        }
    }

    return DefWindowProcW(
        hwnd,
        msg,
        wParam,
        lParam
    );
}

// =====================================
// MAIN APPLICATION
// =====================================

int WINAPI WinMain(
    HINSTANCE hInstance,
    HINSTANCE hPrevInstance,
    LPSTR lpCmdLine,
    int nCmdShow)
{
    GdiplusStartupInput gdiplusStartupInput;

    if (GdiplusStartup(
        &gdiplusToken,
        &gdiplusStartupInput,
        NULL) != Ok)
    {
        MessageBoxW(
            NULL,
            L"Unable to initialize GDI+!",
            L"MiniPhoto Viewer",
            MB_OK | MB_ICONERROR
        );

        return 1;
    }

    WNDCLASSW wc = {};

    wc.lpfnWndProc = WindowProc;

    wc.hInstance = hInstance;

    wc.lpszClassName = L"MiniPhotoViewerWindow";

    wc.hCursor = LoadCursor(NULL, IDC_ARROW);

    wc.hbrBackground = (HBRUSH)GetStockObject(BLACK_BRUSH);

    wc.style = CS_HREDRAW | CS_VREDRAW;

    if (!RegisterClassW(&wc))
    {
        GdiplusShutdown(gdiplusToken);

        return 1;
    }

    HWND hwnd = CreateWindowW(
        L"MiniPhotoViewerWindow",
        L"MiniPhoto Viewer 1.0",
        WS_OVERLAPPEDWINDOW,
        CW_USEDEFAULT,
        CW_USEDEFAULT,
        1000,
        700,
        NULL,
        NULL,
        hInstance,
        NULL
    );

    if (!hwnd)
    {
        GdiplusShutdown(gdiplusToken);

        return 1;
    }

    ShowWindow(hwnd, nCmdShow);

    UpdateWindow(hwnd);

    // Read image path passed by Windows Explorer

    int argc = 0;

    LPWSTR* argv = CommandLineToArgvW(
        GetCommandLineW(),
        &argc
    );

    if (argv != NULL)
    {
        if (argc >= 2)
        {
            LoadImageFile(hwnd, argv[1]);
        }

        LocalFree(argv);
    }

    MSG msg;

    while (GetMessageW(&msg, NULL, 0, 0) > 0)
    {
        TranslateMessage(&msg);

        DispatchMessageW(&msg);
    }

    currentImage.reset();

    GdiplusShutdown(gdiplusToken);

    return (int)msg.wParam;
}
