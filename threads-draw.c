/**
  @file     threads-draw.c
  @brief    Source file for THREADS application drawing routines
  @author   suendisra
*/
#include "threads.h"

#define FONT_NAME                   L"Consolas"
#define FONT_SIZE                   24

#define SCALE_MAIN_VIEW             0.925   // shrink main drawing quad a little bit
#define SCALE_COMMAND_VIEW          0.30    // shrink command panel quad for display of commands
#define SCALE_THREAD_LINE           0.95    // amount to shrink each line which represents a thread

static GPH      gph = {0};
static CLRPOOL  pool = {0};
static QUAD     area = {0};
static QUAD     cmds = {0};

// STATIC PROTOTYPES
static void DrawCommands(void);
static void DrawScreen(void);

/* draw primary application screen */
void Draw(const HWND hwnd)
{
    if(GphPaint(gph, hwnd))
    {
        // clear the background and info bar
        GphClear(gph, NULL, GBLACK);

        DrawScreen();
        DrawCommands();

        // blit and display
        GphBlit(gph);
        GphPaint(gph, hwnd);
    }
}

/* draw the list of commands available to the user */
static void DrawCommands(void)
{
    wchar_t commands[STR_MEDIUM] = {0};

    // clear a space for the information and draw it
    GphStock(gph, GBLACK, GWHITE);
    GphRect(gph, cmds, 0);
    if(app.started)
    {
        GphFontColor(gph, GGRAY, GTRANS);
    }else{
        GphFontColor(gph, (((ULONG)(app.tcount * app.desired) == app.iterations) ? GGREEN : GRED), GTRANS);
    }
    GphText(gph, &cmds, ALIGN_DEFAULT, strings[STRING_TOTAL], app.iterations);

    // build the command string
    StrFormat(commands, sizeof(commands), L"%s\n%s\n%s\n%s\n%s\n%s",
        strings[STRING_CONCURRENT], strings[STRING_ITERATIONS], strings[STRING_LANGUAGE],
        strings[STRING_THREADS], ((app.started == TRUE) ? strings[STRING_STOP] : strings[STRING_START]), strings[STRING_EXIT]);

    // draw standard-color text for command list
    GphFontColor(gph, GGRAY, GTRANS);
    GphText(gph, &cmds, (ALIGN_LEFT | ALIGN_VCENTER), commands,
        ((app.concurrent == TRUE) ? strings[STRING_TRUE] : strings[STRING_FALSE]), app.desired, app.tcount);
}

/* draw the primary screen geometries to show off the threading progress */
static void DrawScreen(void)
{
    QUAD    txt = {0};

    POINT   dims = {0};
    POINT   pt = {0};

    double  theta = 0.0;
    double  delta = (TWOPI / app.tcount);

    long    count = 0;
    long    radius = 0;
    long    n = 0;

    for(n = 0; (n < app.tcount); ++n)
    {
        // find textual dims for the count value
        count = app.current[n];
        if(FontDims(font, GphDC(gph), &dims.x, &dims.y, L" %ld ", count))
        {
            // find the point to draw at
            radius = area.rl;
            Polar(area.mx, area.my, radius, radius, theta, &pt.x, &pt.y);
            QuadCentered(pt.x, pt.y, dims.x, dims.y, &txt);

            // back point in towards center a bit so text won't overlap line
            radius = (long)(SCALE_THREAD_LINE * area.rl);
            Polar(area.mx, area.my, radius, radius, theta, &pt.x, &pt.y);

            // select a color for this thread
            GphStock(gph, GTRANS, PoolColor((n % PoolCount(pool)), pool));
            GphLine(gph, area.mx, area.my, pt.x, pt.y);
            GphText(gph, &txt, ALIGN_CENTER2D, L"%ld", count);
        }

        // increment theta
        theta += delta;
    }
}

/* kick up or destroy the graphical support */
BOOL Graphics(const BOOL start)
{
    BOOL    success = FALSE;

    if(start)
    {
        // create the Windows GDI object
        if(Gph(wnd.handl, wnd.client, &gph))
        {
            // scale down the drawing area a little for clean drawing
            MemCopy(&area, &wnd.client, sizeof(area));
            QuadScale(TRUE, SCALE_MAIN_VIEW, SCALE_MAIN_VIEW, &area);

            // scale down the drawing area for commands available to user
            MemCopy(&cmds, &area, sizeof(cmds));
            QuadScale(TRUE, SCALE_COMMAND_VIEW, SCALE_COMMAND_VIEW, &cmds);

            // create primary font for display
            font = FontGDI(FONT_NAME, FONT_SIZE, FALSE, FALSE, FALSE);
            if(font >= 0)
            {
                // set the font text and color
                GphFontSet(gph, font);
                GphFontColor(gph, GGRAY, GTRANS);

                // set up the graphics color pool
                Pool(GTRANS, GTRANS, GTRANS, 0, &pool);
                PoolDefault(pool);
                success = TRUE;
            }
        }
    }else{
        // destroy the font and graphics objects
        FontKillGDI(font);
        PoolKill(pool);
        GphKill(&gph);
        success = TRUE;
    }

    return(success);
}
