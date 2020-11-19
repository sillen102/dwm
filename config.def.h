/* See LICENSE file for copyright and license details. */

/* for XF86 Media Keys */
#include <X11/XF86keysym.h>

/* appearance */
static const unsigned int borderpx  = 1;        /* border pixel of windows */
static const unsigned int snap      = 32;       /* snap pixel */
static const int swallowfloating    = 1;        /* 1 means swallow floating windows by default */
static const unsigned int gappih    = 10;       /* horiz inner gap between windows */
static const unsigned int gappiv    = 10;       /* vert inner gap between windows */
static const unsigned int gappoh    = 10;       /* horiz outer gap between windows and screen edge */
static const unsigned int gappov    = 10;       /* vert outer gap between windows and screen edge */
static const int smartgaps          = 1;        /* 1 means no outer gap when there is only one window */
static const unsigned int systraypinning = 1;   /* 0: sloppy systray follows selected monitor, >0: pin systray to monitor X */
static const unsigned int systrayspacing = 2;   /* systray spacing */
static const int systraypinningfailfirst = 1;   /* 1: if pinning fails, display systray on the first monitor, False: display systray on the last monitor*/
static const int showsystray        = 1;        /* 0 means no systray */
static const int showbar            = 1;        /* 0 means no bar */
static const int topbar             = 1;        /* 0 means bottom bar */
static const char *fonts[]          = { "SauceCodePro Nerd Font:pixelsize=12:antialias=true:autohint=true" };
static const char dmenufont[]       = "monospace:size=10";
static const char col_gray1[]       = "#222222"; /* background color */
static const char col_gray2[]       = "#444444"; /* inactive border color */
static const char col_gray3[]       = "#bbbbbb"; /* font color */
static const char col_gray4[]       = "#eeeeee"; /* current tag and current window font color */
static const char col_cyan[]        = "#005577"; /* top bar second color (default blue) and active window border color */
static const char *colors[][3]      = {
    /*               fg         bg         border   */
    [SchemeNorm] = { col_gray3, col_gray1, col_gray2 },
    [SchemeSel]  = { col_gray4, col_cyan,  col_cyan  },
};

static const unsigned int baralpha = 0xd0;
static const unsigned int borderalpha = OPAQUE;
static const unsigned int alphas[][3]      = {
    /*               fg      bg        border     */
    [SchemeNorm] = { OPAQUE, baralpha, borderalpha },
    [SchemeSel]  = { OPAQUE, baralpha, borderalpha },
};

static const char *const autostart[] = {
    "autorandr", "--change", NULL,
    "lxpolkit", NULL,
    "picom", "--experimental-backends", NULL,
    "feh", "--bg-scale",
        ".config/wallpapers/girl-with-katana-at-moonlight_3840x2160_xtrafondos.com.jpg", NULL,
    "dunst", NULL,
    "blueman-applet", NULL,
    "pamac-tray", NULL,
    "statusbar", NULL,
    "nm-applet", NULL,
    "redshift-gtk", NULL,
    "clipit", NULL,
    "libinput-gestures-setup", "start", NULL,
    "caffeine", NULL,
    "numlockx", NULL,
    "dropbox", "start", "-i", NULL,
    "skypeforlinux", NULL,
    "pasystray", NULL,
    "xfce4-power-manager", NULL,
    "spotifyd", NULL
};

/* tagging */
static char *tags[] = { "1  ", "2  ", "3  ", "4  ", "5  ", "6  ", "7  ", "8  ", "9  " };

static const Rule rules[] = {
    /* xprop(1):
     *	WM_CLASS(STRING) = instance, class
     *	WM_NAME(STRING) = title
     */
    /* class            instance  title           tags mask  isfloating  isterminal  noswallow      monitor */
    { "Gimp",           NULL,     NULL,           0,         1,          0,           0,            -1 },
    { "Firefox",        NULL,     NULL,           0,         0,          0,          -1,            -1 },
    { "Skype",          NULL,     NULL,           7,         0,          0,          -1,            -1 },
    { "st-256color",    NULL,     NULL,           0,         0,          1,           0,            -1 },
    { "Alacritty",      NULL,     NULL,           0,         0,          1,           0,            -1 },   
    { "Gnome-terminal", NULL,     NULL,           0,         0,          1,           0,            -1 },
    { "URxvt",          NULL,     NULL,           0,         0,          1,           0,            -1 },
    { NULL,             NULL,     "Event Tester", 0,         0,          0,           1,            -1 }, /* xev */
};

/* Media keys */
static const char *upvol[]   = { "pactl", "set-sink-volume", "@DEFAULT_SINK@", "+5%",     NULL };
static const char *downvol[] = { "pactl", "set-sink-volume", "@DEFAULT_SINK@", "-5%",     NULL };
static const char *mutevol[] = { "pactl", "set-sink-mute",   "@DEFAULT_SINK@", "toggle",  NULL };
static const char *playpause[] = { "playerctl", "play-pause",  NULL };
static const char *playnext[] = { "playerctl", "next", NULL };
static const char *playprevious[] = { "playerctl", "previous", NULL };
static const char *downbright[] = { "brightnessctl", "set", "5%-", NULL };
static const char *upbright[] = { "brightnessctl", "set", "+5%", NULL };

/* layout(s) */
static const float mfact     = 0.55; /* factor of master area size [0.05..0.95] */
static const int nmaster     = 1;    /* number of clients in master area */
static const int resizehints = 1;    /* 1 means respect size hints in tiled resizals */
static const int attachdirection = 4;    /* 0 default, 1 above, 2 aside, 3 below, 4 bottom, 5 top */

static const Layout layouts[] = {
    /* symbol     arrange function */
    { "[]=",      tile },    /* first entry is default */
    { "><>",      NULL },    /* no layout function means floating behavior */
    { "[M]",      monocle },
    { "=M=",      centeredmaster },
    { ">M>",      centeredfloatingmaster },
    { "[D]",      deck },
    { "HHH",      gaplessgrid },
    { NULL,       NULL },
};

/* key definitions */
#define MODKEY Mod4Mask
#define ALTKEY Mod1Mask
#define PrintScreenDWM 0x0000ff61
#define TAGKEYS(KEY,TAG) \
    { MODKEY,                       KEY,      view,           {.ui = 1 << TAG} }, \
    { MODKEY|ControlMask,           KEY,      toggleview,     {.ui = 1 << TAG} }, \
    { MODKEY|ShiftMask,             KEY,      tag,            {.ui = 1 << TAG} }, \
    { MODKEY|ControlMask|ShiftMask, KEY,      toggletag,      {.ui = 1 << TAG} },

/* helper for spawning shell commands in the pre dwm-5.0 fashion */
#define SHCMD(cmd) { .v = (const char*[]){ "/bin/sh", "-c", cmd, NULL } }

/* commands */
static char dmenumon[2] = "0"; /* component of dmenucmd, manipulated in spawn() */
static const char *dmenucmd[] =
            { "dmenu_run", "-m", dmenumon, "-fn", dmenufont, "-nb", col_gray1, "-nf", col_gray3, "-sb", col_cyan, "-sf", col_gray4, NULL };
static const char *termcmd[]  = { "xfce4-terminal", NULL };
static const char *browsercmd[] = { "firefox", NULL };
static const char *secondbrowsercmd[] = { "chromium", NULL };
static const char *filemanagercmd[] = { "pcmanfm", NULL };
static const char *editorcmd[] = { "mousepad", NULL };
static const char *secondeditorcmd[] = { "code", NULL };
static const char *rofimenucmd[] = { "rofi", "-show", "drun", "-theme", "clean", NULL };
static const char *rofiwindowcmd[] = { "rofi", "-show", "window", "-theme", "clean_window", NULL };
static const char *rofipowercmd[] = { "powermenu_launch.sh", NULL };
static const char *lockscreen[] = { "slock", NULL };
static const char *screenshot[] = { "xfce4-screenshooter", NULL };

#include "movestack.c"
static Key keys[] = {
    /* modifier                     key         function        argument */
    { MODKEY,                       XK_Return,  spawn,          {.v = termcmd } },
    { MODKEY,                       XK_b,       togglebar,      {0} },
    { MODKEY,                       XK_Up,      focusstack,     {.i = -1 } },
    { MODKEY,                       XK_Down,    focusstack,     {.i = +1 } },
    { MODKEY|ShiftMask,             XK_Down,    movestack,      {.i = +1 } },
    { MODKEY|ShiftMask,             XK_Up,      movestack,      {.i = -1 } },
    { MODKEY|ShiftMask,             XK_i,       incnmaster,     {.i = -1 } },
    { MODKEY,                       XK_i,       incnmaster,     {.i = +1 } },
    { MODKEY|ControlMask,           XK_Left,    setmfact,       {.f = -0.05} },
    { MODKEY|ControlMask,           XK_Right,   setmfact,       {.f = +0.05} },
    { ALTKEY,                       XK_Tab,     focusstack,     {.i = +1 } },
    { MODKEY,                       XK_z,       zoom,           {0} },
    { MODKEY,                       XK_Tab,     view,           {0} },
    { MODKEY,                       XK_0,       view,           {.ui = ~0 } },
    { MODKEY,                       XK_q,       killclient,     {0} },
    { MODKEY|ShiftMask,             XK_r,       quit,           {1} }, 

    /* Layouts */
    { MODKEY,                       XK_t,       setlayout,      {.v = &layouts[0]} },
    { MODKEY,                       XK_f,       setlayout,      {.v = &layouts[1]} },
    { MODKEY,                       XK_m,       setlayout,      {.v = &layouts[2]} },
    { MODKEY,                       XK_c,       setlayout,      {.v = &layouts[3]} },
    { MODKEY|ShiftMask,             XK_c,       setlayout,      {.v = &layouts[4]} },
    { MODKEY,                       XK_d,       setlayout,      {.v = &layouts[5]} },
    { MODKEY,                       XK_g,       setlayout,      {.v = &layouts[6]} },
    { MODKEY|ControlMask,		    XK_Down,    cyclelayout,    {.i = -1 } },
    { MODKEY|ControlMask,           XK_Up,      cyclelayout,    {.i = +1 } },
    { MODKEY|ShiftMask,             XK_space,   togglefloating, {0} },

    /* Multiple monitors */
    { MODKEY,                       XK_comma,   focusmon,       {.i = -1 } },
    { MODKEY,                       XK_period,  focusmon,       {.i = +1 } },
    { MODKEY,                       XK_space,   focusmaster,    {0} },
    { MODKEY|ShiftMask,             XK_comma,   tagmon,         {.i = -1 } },
    { MODKEY|ShiftMask,             XK_period,  tagmon,         {.i = +1 } },

    /* Menus */
    { MODKEY,                       XK_r,       spawn,          {.v = dmenucmd } },
    { MODKEY,                       XK_p,       spawn,          {.v = rofimenucmd } },
    { MODKEY|ShiftMask,             XK_p,       spawn,          {.v = rofiwindowcmd } },
    { MODKEY|ShiftMask,             XK_q,       spawn,          {.v = rofipowercmd } },
    
    /* Gaps */
    { MODKEY,                       XK_minus,   incrgaps,       {.i = -5 } },
    { MODKEY,                       XK_plus,    incrgaps,       {.i = +5 } },
    { MODKEY|ShiftMask,             XK_g,       togglegaps,     {0} },
    { MODKEY|ControlMask,           XK_g,       defaultgaps,    {0} },

    /* Applications */
    { MODKEY,                       XK_w,       spawn,          {.v = browsercmd } },
    { MODKEY|ShiftMask,             XK_w,       spawn,          {.v = secondbrowsercmd } },
    { MODKEY|ShiftMask,             XK_Return,  spawn,          {.v = filemanagercmd } },
    { MODKEY,                       XK_e,       spawn,          {.v = editorcmd } },
    { MODKEY|ShiftMask,             XK_e,       spawn,          {.v = secondeditorcmd } },

    /* Media keys */
    { 0,            XF86XK_AudioLowerVolume,    spawn,          {.v = downvol } },
    { 0,            XF86XK_AudioMute,           spawn,          {.v = mutevol } },
    { 0,            XF86XK_AudioRaiseVolume,    spawn,          {.v = upvol   } },
    { 0,            XF86XK_AudioPlay,           spawn,          {.v = playpause } },
    { 0,            XF86XK_AudioNext,           spawn,          {.v = playnext } },
    { 0,            XF86XK_AudioPrev,           spawn,          {.v = playprevious } },
    { 0,            XF86XK_MonBrightnessDown,   spawn,          {.v = downbright } },
    { 0,            XF86XK_MonBrightnessUp,     spawn,          {.v = upbright } },
    
    /* Special keys */
    { 0,                    PrintScreenDWM,     spawn,          {.v = screenshot } },

    /* Lockscreen */
    { MODKEY,                       XK_l,       spawn,          {.v = lockscreen } },
        
    /* Tags */
    TAGKEYS(                        XK_1,                       0)
    TAGKEYS(                        XK_2,                       1)
    TAGKEYS(                        XK_3,                       2)
    TAGKEYS(                        XK_4,                       3)
    TAGKEYS(                        XK_5,                       4)
    TAGKEYS(                        XK_6,                       5)
    TAGKEYS(                        XK_7,                       6)
    TAGKEYS(                        XK_8,                       7)
    TAGKEYS(                        XK_9,                       8)
};

/* button definitions */
/* click can be ClkTagBar, ClkLtSymbol, ClkStatusText, ClkWinTitle, ClkClientWin, or ClkRootWin */
static Button buttons[] = {
    /* click                event mask      button          function        argument */
    { ClkLtSymbol,          0,              Button1,        setlayout,      {0} },
    { ClkWinTitle,          0,              Button2,        zoom,           {0} },
    { ClkStatusText,        0,              Button2,        spawn,          {.v = termcmd } },
    { ClkClientWin,         MODKEY,         Button1,        movemouse,      {0} },
    { ClkClientWin,         MODKEY,         Button2,        togglefloating, {0} },
    { ClkClientWin,         MODKEY,         Button3,        resizemouse,    {0} },
    { ClkTagBar,            0,              Button1,        view,           {0} },
    { ClkTagBar,            0,              Button3,        toggleview,     {0} },
    { ClkTagBar,            MODKEY,         Button1,        tag,            {0} },
    { ClkTagBar,            MODKEY,         Button3,        toggletag,      {0} },
};
