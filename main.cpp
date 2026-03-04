#include "Core/App.h"


// _In_ などの注釈を追加して警告を消す
int WINAPI WinMain(
    _In_ HINSTANCE hInstance,
    _In_opt_ HINSTANCE,
    _In_ LPSTR,
    _In_ int nCmdShow)
{
    App app;

    if (!app.Init(hInstance, 1280, 720))
        return -1;

    MSG msg = {};
    while (msg.message != WM_QUIT)
    {
        if (PeekMessage(&msg, nullptr, 0, 0, PM_REMOVE))
        {
            TranslateMessage(&msg);
            DispatchMessage(&msg);
        }
        else
        {
            app.Update();
            app.Draw();
        }
    }

    return 0;
}

