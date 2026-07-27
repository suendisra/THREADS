/**
  @file     threads.c
  @brief    Source file for THREADS application
  @author   suendisra
*/
#include "threads.h"

// static prototypes
static BOOL Start(void);
static void Stop(void);

/* wWinMain */
int WINAPI wWinMain(HINSTANCE instance, HINSTANCE previnst, LPWSTR cmd, int show)
{
    QUAD    dims = {0};

    // create window and loop it
    WinInit(instance, previnst, cmd, show);
    QuadCentered(win.client.mx, win.client.my, win.client.cl, win.client.cl, &dims);
    if(Window(APP_TITLE, dims, NULL, &wnd))
    {
        WindowConfig(Start, NULL, Stop, &wnd);
        WindowFunc(WM_PAINT, (void*)Draw, &wnd);
        WindowFunc(WM_KEYDOWN, (void*)Input, &wnd);
        WindowFunc(WM_KEYUP, (void*)Input, &wnd);
        WindowFunc(WM_TIMER, (void*)ThreadCheck, &wnd);
        Loop(wnd);
    }

    return(0);
}

/* start application */
static BOOL Start(void)
{
    BOOL    success = FALSE;

    // set in the application defaults
    Defaults();

    // load and set window icon
    TextSet(wnd.handl, 0, L"%s - %s", APP_TITLE, APP_VERSION);
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
