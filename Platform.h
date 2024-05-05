#pragma once
#if defined(_WIN32) && !defined(__unix__)
#define WINDOWS
#define WIN32_LEAN_AND_MEAN

#include <WinSock2.h>
#include <WS2tcpip.h>
#include <Windows.h>

typedef SOCKET Socket;
#endif

#if defined(__unix__) && !defined(_WIN32)
#define UNIX
#include <sys/socket.h>
#include <sys/select.h>
#include <netdb.h>
#include <unistd.h>

typedef int Socket;
#endif

#if defined(__APPLE__) && defined(__MACH__)
#define APPLE
#endif