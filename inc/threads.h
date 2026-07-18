/**
  @file     threads.h
  @brief    Header file for THREADS application
*/
#ifndef _THREADS_H_
#define _THREADS_H_

#include <util.h>
#include <util-dsa.h>
#include <util-file.h>
#include <util-log.h>
#include <util-math.h>
#include <util-mem.h>
#include <util-std.h>
#include <util-str.h>
#include <util-sync.h>

#include <gph.h>
#include <gph-font.h>
#include <gph-pool.h>
#include <gph-shape.h>
#include <gph-std.h>

#include <win.h>
#include <win-prompt.h>

#include "resource.h"

// bring in appropriate version of library
#ifndef _DEBUG
#pragma comment(lib, "GPH.lib")
#pragma comment(lib, "UTIL.lib")
#pragma comment(lib, "WIN.lib")
#else
#pragma comment(lib, "GPHD.lib")
#pragma comment(lib, "UTILD.lib")
#pragma comment(lib, "WIND.lib")
#endif

#define APP_COPYRIGHT           2026
#define APP_LOG                 L"threads.log"
#define APP_TITLE               L"THREADS"
#define APP_VERSION             L"1.0.6"

#define MAXIMUM_THREAD_COUNT    64

// supported languages for multilingual display
enum LANGUAGE
{
    LANGUAGE_ENGLISH,
    LANGUAGE_DEUTSCH,
    LANGUAGE_FRENCH,
    LANGUAGE_RUSSIAN,
    LANGUAGE_LAST
};

// strings each language will contain
enum STRINGS
{
    STRING_TITLE,
    STRING_CONCURRENT,
    STRING_EXIT,
    STRING_FALSE,
    STRING_ITERATIONS,
    STRING_LANGUAGE,
    STRING_PROMPT_ITERATIONS,
    STRING_PROMPT_THREAD_COUNT,
    STRING_START,
    STRING_STOP,
    STRING_THREADS,
    STRING_TOO_MANY_THREADS,
    STRING_TOTAL,
    STRING_TRUE,
    STRING_LAST
};

struct
{
    enum LANGUAGE   language;                       // language in which to display application

    ULONG           iterations;                     // total number of iterations each thread has made

    long            current[MAXIMUM_THREAD_COUNT];  // current iteration with given thread
    long            tcount;                         // total number of threads to spin up
    long            desired;                        // desired number of iterations to see from thread

    BOOL            concurrent;                     // display acting on single object
    BOOL            started;                        // tracker for whether demo has been started or not
}app;

// window and associated graphical font
WNDW        wnd;
INDEX       font;

// gui text and indices to which the command key is present in them
MAP         indices;
wchar_t     strings[STRING_LAST][STR_NORM];

/**
  @fn               void Defaults(void)
  @brief            sets up the application with the default values
*/
void Defaults(void);

/**
  @fn               void Draw(const HWND hwnd)
  @brief            draw primary application screen
  @param[in]        hwnd handle to window on which to draw
*/
void Draw(const HWND hwnd);

/**
  @fn               BOOL Graphics(const BOOL start)
  @brief            kick up or destroy the graphical support
  @param[in]        start if TRUE, kicks up graphical support, FALSE shuts it down
  @return           TRUE if successful, FALSE otherwise
*/
BOOL Graphics(const BOOL start);

/**
  @fn               void Input(const WORD key, const BOOL down)
  @brief            handle user input
  @param[in]        wp the window WPARAM for which the keyboard event has occurred
  @param[in]        msg the windows processing message for WM_KEYDOWN or WM_KEYUP
*/
void Input(const WORD key, const BOOL down);

/**
  @fn               BOOL Language(void)
  @brief            load in language strings from string table
  @return           TRUE if language was loaded, FALSE otherwise
*/
BOOL Language(void);

/**
  @fn               BOOL Logic(const BOOL start)
  @brief            kick up or destroy the logical support
  @param[in]        start if TRUE, kicks up logical support, FALSE shuts it down
  @return           TRUE if successful, FALSE otherwise
*/
BOOL Logic(const BOOL start);

/**
  @fn               void ThreadCheck(const HWND hwnd, const UINT timerID)
  @brief            checks to see if any thread for the application is still running
  @param[in]        hwnd handle to window for which to check the threads
*/
void ThreadCheck(const HWND hwnd, const UINT timerID);

/**
  @fn               void ThreadRun(const BOOL start)
  @brief            starts or stops the application threads
  @brief            kick up or destroy the application threads
  @param[in]        start if TRUE, kicks up application threads, FALSE shuts it down
  @return           TRUE if successful, FALSE otherwise
*/
void ThreadRun(const BOOL start);

#endif
