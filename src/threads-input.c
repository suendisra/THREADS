/**
  @file     threads.c
  @brief    Source file for THREADS application input routines
*/
#include "threads.h"

static KEYBOARD keyboard = {0};
static MOUSE    mouse = {0};

// STATIC PROTOYPES
static long MapIndex(const enum STRINGS str);

/* handle user input */
void Input(const WORD key, const BOOL down)
{
    long    idx = 0;
    long    temp = 0;
    BOOL    input = FALSE;

    if(KeySet(key, down, &keyboard))
    {
        // toggle concurrency display
        idx = MapIndex(STRING_CONCURRENT);
        if(keyboard.up[strings[STRING_CONCURRENT][idx]] && !app.started)
        {
            app.concurrent ^= TRUE;
            input = TRUE;
        }

        // prompt user for number of desired iterations
        idx = MapIndex(STRING_ITERATIONS);
        if(keyboard.up[strings[STRING_ITERATIONS][idx]] && !app.started)
        {
            // prompt user for desired amount of iterations each thread should perform
            if(PromptLong(wnd.handl, strings[STRING_PROMPT_ITERATIONS], app.desired, &temp) == RESULT_SUCCESS)
            {
                if(temp > 0)
                {
                    // set in the received value
                    app.desired = temp;
                }
            }

            input = TRUE;
        }

        // alter language
        idx = MapIndex(STRING_LANGUAGE);
        if(keyboard.up[strings[STRING_LANGUAGE][idx]])
        {
            app.language = ((app.language + 1) % LANGUAGE_LAST);
        }

        // start or stop threading
        idx = MapIndex(STRING_START);
        if(keyboard.up[strings[STRING_START][idx]])
        {
            // toggle threads running
            ThreadRun(!app.started);
            input = TRUE;
        }

        // prompt user for desired threads
        idx = MapIndex(STRING_THREADS);
        if(keyboard.up[strings[STRING_THREADS][idx]] && !app.started)
        {
            // prompt user for desired amount of threads to create
            if(PromptLong(wnd.handl, strings[STRING_PROMPT_THREAD_COUNT], app.tcount, &temp) == RESULT_SUCCESS)
            {
                // ensure successful run of prompt routine did not end in cancellation
                if((temp > 0) && (temp <= MAXIMUM_THREAD_COUNT))
                {
                    // set in the received value
                    app.tcount = temp;
                }else if(temp > MAXIMUM_THREAD_COUNT){
                    // only THREAD_MAX_COUNT number of threads can be spun up
                    Message(MSG_ERR, wnd.handl, strings[STRING_TOO_MANY_THREADS], MAXIMUM_THREAD_COUNT);
                }
            }

            input = TRUE;
        }

        // allow the user to exit the application just from Esc or 'X'
        idx = MapIndex(STRING_EXIT);
        if(keyboard.up[strings[STRING_EXIT][1]] || keyboard.up[VK_ESCAPE])
        {
            Quit(wnd.handl);
        }

        // redraw screen after user input
        if(input)
        {
            Refresh(wnd.handl);
        }

        // up-state of key(s) should not be persistent
        MemClear(&keyboard.up, sizeof(keyboard.up));
    }
}

/* identify the index of the string in the indices map */
static long MapIndex(const enum STRINGS str)
{
    long    temp = (long)str;
    long    idx = -1;

    MapPop(indices, &idx, &temp, sizeof(temp));
    return(idx + 1);
}
