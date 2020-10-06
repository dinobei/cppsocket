#pragma once

#if defined(__WIN32__) || defined(WIN32) || defined(_WIN32) || defined(_WIN64)
    #define __CPPSOCKET_WINDOWS__
#endif

#if defined(__linux__) || defined(__linux)
    #define __CPPSOCKET_LINUX__
#endif

#if defined(__ANDROID__)
    #define __CPPSOCKET_ANDROID__
#endif

#if defined(__APPLE__)
    #define __CPPSOCKET_APPLE__
    #import "TargetConditionals.h"
    #if TARGET_OS_IPHONE
        #define __CPPSOCKET_IOS__
    #else
        #define __CPPSOCKET_MACOS__
    #endif
#endif


#if defined(__CPPSOCKET_WINDOWS__)
    #define WIN32_LEAN_AND_MEAN
    #include <Windows.h>
    #include <WS2tcpip.h>

	typedef unsigned short ushort;
	typedef unsigned int uint;
#else
    #include <sys/time.h>
    #include <unistd.h>
    #include <pthread.h>
    #include <sys/socket.h>
    #include <arpa/inet.h>
    #include <netinet/tcp.h>
    #include <netdb.h>
#endif

#if defined(__CPPSOCKET_WINDOWS__)
    #define     __msleep(ms)          Sleep(ms)
    #define     __sleep(sec)          Sleep((sec)*1000)
#else
    #define     __msleep(ms)          usleep((ms)*1000)
    #define     __sleep               sleep
#endif

#if defined(_MSC_VER)
#include <BaseTsd.h>
typedef SSIZE_T ssize_t;
#endif
