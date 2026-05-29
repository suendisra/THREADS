/**
  @file     threads.c
  @brief    Source file for THREADS application logic
  @author   suendisra
*/
#include "threads.h"

#define COMMAND_CHAR                L'['

#define DEFAULT_DESIRED_ITERS       5000
#define DEFAULT_THREAD_COUNT        20

#define SEMAPHORE_GATES             1   // number of threads that will be allowed to run at a single time

#define TIMER_REFRESH_ID            9168
#define TIMER_REFRESH_PERIOD        50

#define WAIT_INFINITE               -1

enum ACTIONTYPE
{
    ACTION_INVAL = -1,
    ACTION_CHECK,
    ACTION_START,
    ACTION_STOP,
    ACTION_LAST
};

typedef struct
{
    enum ACTIONTYPE type;
    void            *result;
}ACTION;

static QUEUE    tq = NULL; // queue for spinning off threads
static SEM      sq = NULL; // semaphore to protect queue work
static THREAD   threads[MAXIMUM_THREAD_COUNT] = {0}; // thread array to store thread handles

// STATIC PROTOTYPES
static void ThreadAction(const long index, const void *value, const void *userdata);
static void ThreadConcurrent(void);
static void ThreadProc(THREAD thread);
static BOOL ThreadSpool(void);

/* set in the application defaults */
void Defaults(void)
{
    // set the defaults
    MemClear(&app, sizeof(app));
    app.desired = DEFAULT_DESIRED_ITERS;
    app.tcount = DEFAULT_THREAD_COUNT;
}

/* load in language strings from string table */
BOOL Language(void)
{
    const long  baseID = (IDS_EN_TITLE + (app.language * STRING_LAST));
    long        idx = -1;
    long        n = 0;
    BOOL        success = FALSE;

    // load in the strings
    if(Map(&indices) && StrList(baseID, STRING_LAST, sizeof(strings[0]), &strings[0][0]))
    {
        // build the map for this language
        for(n = STRING_TITLE; (n < STRING_LAST); ++n)
        {
            idx = CharFind(strings[n], sizeof(strings[n]), COMMAND_CHAR, 1);
            if(idx >= 0)
            {
                MapPush(indices, &n, sizeof(n), &idx, sizeof(idx));
            }
        }

        success = TRUE;
    }

    return(success);
}

/* kick up or shut down the logic of the application */
BOOL Logic(const BOOL start)
{
    wchar_t     path[STR_PATH] = {0};
    BOOL        success = FALSE;

    if(start)
    {
        // identify the path for the app log and app file
        if(FolderApp(path, sizeof(path), APP_LOG))
        {
            // start up logging needs
            if(Log(win.instance, NULL, path) && Language())
            {
                // create the queue and the semaphore for the thread work
                success = Queue(sizeof(INDEX), &tq) && Semaphore(SEMAPHORE_GATES, &sq);
            }
        }
    }else{
        // stop the threads, destroy the queue
        ThreadRun(FALSE);
        QueueKill(&tq);
        SemaphoreKill(&sq);

        // destroy language map and log recorder
        MapKill(&indices);
        LogKill();
        success = TRUE;
    }

    return(success);
}

/* start or stop the threads in the queue */
static void ThreadAction(const long index, const void *value, const void *userdata)
{
    ACTION  *action = ((ACTION*)userdata);
    INDEX   tidx = *((INDEX*)value);

    if((action != NULL) && (index >= 0) && Clamped(tidx, -1, MAXIMUM_THREAD_COUNT) && Clamped(action->type, ACTION_INVAL, ACTION_LAST))
    {
        // perform the specified action on each node of the queue
        switch(action->type)
        {
            case ACTION_CHECK:
                *((BOOL*)action->result) |= !ThreadCancelled(threads[tidx]);
                break;

            case ACTION_START:
                *((long*)action->result) += ThreadSuspend(FALSE, threads[tidx]);
                break;

            case ACTION_STOP:
                *((long*)action->result) += ThreadKill(FALSE, &threads[tidx]);
                break;
        }
    }
}

/* check if any threads are running, if so, shut them down */
void ThreadCheck(const HWND hwnd, const UINT timerID)
{
    ACTION  action = {0};
    BOOL    any = FALSE;

    if((hwnd != NULL) && (timerID > 0))
    {
        // assume no threads are running and run through queue to see if any threads are running
        action.result = &any;
        action.type = ACTION_CHECK;
        if(SemaphoreTake(sq, WAIT_INFINITE))
        {
            QueueForEach(tq, FALSE, &action, ThreadAction);
            if(!any)
            {
                // perform cleanup
                ThreadRun(FALSE);
            }

            SemaphoreGive(sq);
        }

        // redraw the screen
        Refresh(hwnd);
    }
}

/* work threads concurrently */
static void ThreadConcurrent(void)
{
    INDEX   tidx = -1;
    long    *count = NULL;

    // attempt to take the semaphore
    if(SemaphoreTake(sq, WAIT_INFINITE))
    {
        // pop first thread ID that should run and perform its work
        if(QueuePop(&tidx, tq))
        {
            // work is the ID of the thread associated with this queue data
            count = (long*)ThreadData(threads[tidx]);
            if((NULL != count) && (*count < app.desired) && !ThreadCancelled(threads[tidx]))
            {
                // do the current thread work
                ++(*count);
                ++app.iterations;

                // push work for the thread back onto the queue
                QueuePush(&tidx, tq);
            }
        }

        // release the semaphore
        SemaphoreGive(sq);
    }
}

/* actual thread function to be performed for each thread */
static void ThreadProc(THREAD thread)
{
    long    *count = NULL;

    // continuously perform the work until this thread has reached sufficient iterations or it gets cancelled
    count = (long*)ThreadData(thread);
    while((NULL != count) && (*count < app.desired) && !ThreadCancelled(thread))
    {
        // is app running in concurrent mode?
        if(app.concurrent)
        {
            ThreadConcurrent();
        }else{
            // do the current thread work
            ++(*count);
            ++app.iterations;
        }
    }
}

/* spin up the threads and start demo or stop them */
void ThreadRun(const BOOL start)
{
    ACTION  action = {0};
    long    count = 0;

    action.result = &count;
    action.type = ACTION_START;
    if(start)
    {
        // reset counters and track struct
        MemClear(app.current, sizeof(app.current));
        app.iterations = 0;

        // spool up the queue and get the threads started
        ThreadSpool();
        QueueForEach(tq, FALSE, &action, ThreadAction);
        Timer(wnd.handl, TIMER_REFRESH_ID, TIMER_REFRESH_PERIOD);
    }else{
        action.type = ACTION_STOP;
        QueueForEach(tq, TRUE, &action, ThreadAction);
        Timer(wnd.handl, TIMER_REFRESH_ID, 0);
    }

    app.started ^= TRUE;
}

/* spool up or despool the threads into/out of the queue */
static BOOL ThreadSpool(void)
{
    long    n = 0;
    BOOL    success = FALSE;

    // create the threads and push into the queue
    for(success = TRUE, n = 0; ((n < app.tcount) && success); ++n)
    {
        success = Thread(ThreadProc, &app.current[n], &threads[n]);
        success &= QueuePush(&n, tq);
    }

    return(success);
}
