/**
  @file     threads.c
  @brief    Source file for THREADS application
  @author   suendisra
*/
#include "threads.h"

// static prototypes
static LRESULT CALLBACK MainWndProc(HWND hwnd, UINT msg, WPARAM wp, LPARAM lp);
static BOOL Start(void);
static void Stop(void);

/* wWinMain */
int WINAPI wWinMain(HINSTANCE instance, HINSTANCE previnst, LPWSTR cmd, int show)
{
    QUAD    dims = {0};

    WinInit(instance, previnst, cmd, show);

    // create window and loop it
    QuadCentered(win.client.mx, win.client.my, win.client.cl, win.client.cl, &dims);
    if(Window(MainWndProc, APP_TITLE, dims, NULL, &wnd))
    {
        WindowConfig(Start, NULL, Stop, &wnd);
        Loop(wnd);
    }

    return(0);
}

/* MainWndProc */
LRESULT CALLBACK MainWndProc(HWND hwnd, UINT msg, WPARAM wp, LPARAM lp)
{
    long    rval = 0;

    switch(msg)
    {
        case WM_CREATE:
            TextSet(hwnd, 0, L"%s - %s", APP_TITLE, APP_VERSION);
            break;

        case WM_ACTIVATE:
            Focus(hwnd, 0);
            break;

        case WM_PAINT:
            Draw(hwnd);
            break;

        case WM_SYSCOMMAND:
            switch(LOWORD(wp))
            {
                case SC_CLOSE:
                case SC_DEFAULT:
                    PostQuitMessage(0);
                    break;

                default:
                    rval = DefWindowProc(hwnd, msg, wp, lp);
                    break;
            }
            break;

        case WM_TIMER:
            ThreadCheck(hwnd);
            break;

        case WM_KEYDOWN:
        case WM_KEYUP:
            Input(wp, msg);
            break;

        default:
            rval = DefWindowProc(hwnd, msg, wp, lp);
            break;
    }

    return(rval);
}

/* start application */
static BOOL Start(void)
{
    BOOL    success = FALSE;

    // set in the application defaults
    Defaults();

    // load and set window icon
    if(WindowIcon(IDI_THREADS, &wnd))
    {
        if(Logic(TRUE))
        {
            success = Graphics(TRUE);
        }
    }

    return(success);
}

/* stop application */
static void Stop(void)
{
    Logic(FALSE);
    Graphics(FALSE);
    WindowKill(&wnd);
}
