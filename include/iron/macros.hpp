#pragma once

#if defined(__linux__) || defined(LINUX) || defined(__unix__) || defined(UNIX)
    #define Linux
#endif

#if defined(_WIN32) || defined(WIN32)
    #define Windows
    #ifdef IronEngine_EXPORTS
        #define IronDLL __declspec(dllexport)
    #else
        #define IronDLL __declspec(dllimport)
    #endif
#endif