#ifndef SYS_PRINT_H
#define SYS_PRINT_H

#include <stdio.h>
#define DEBUG_VERSION

#define RED          "\033[0;32;31m"
#define LIGHT_RED    "\033[1;31m"
#define GREEN        "\033[0;32;32m"
#define LIGHT_GREEN  "\033[1;32m"
#define BLUE         "\033[0;32;34m"
#define LIGHT_BLUE   "\033[1;34m"
#define DARY_GRAY    "\033[1;30m"
#define CYAN         "\033[0;36m"
#define LIGHT_CYAN   "\033[1;36m"
#define PURPLE       "\033[0;35m"
#define LIGHT_PURPLE "\033[1;35m"
#define BROWN        "\033[0;33m"
#define YELLOW       "\033[1;33m"
#define LIGHT_GRAY   "\033[0;37m"
#define WHITE        "\033[1;37m"
#define NO_CLOR      "\033[0m"

#define DBG_DBG 0
#define DBG_ERROR 1
#define DBG_WARN  2 
#define DBG_STEP  3
#define DBG_DATA  4
#define DBG_DBG_DATA 5
#define SW_DBG_LEVEL 3
extern signed char *g_colorMap[];
extern signed char g_dbgSwitch;
#ifdef DEBUG_VERSION
    #define SW_DBG(dbgType, fomat, args...) do {\
    if (g_dbgSwitch - (dbgType) >= 0){printf("%s", g_colorMap[(dbgType)]);printf("%s(line:%d):", __FUNCTION__, __LINE__);\
        printf(fomat, ##args); printf(NO_CLOR);}\
    }while(0)
#else 
    #define SW_DBG(dbgType, fomat,args...) do{}while(0)
#endif

#endif
