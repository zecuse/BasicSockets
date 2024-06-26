#pragma once
#if __cplusplus >= 201703L && defined(__has_cpp_attribute)
	#if __has_capp_attribute(fallthrough)
		#define FALLTHROUGH [[fallthrough]]
	#endif
#elif defined(__GNUC__)
	#define FALLTHROUGH __attribute__((fallthrough))
#else
	#define FALLTHROUGH
#endif

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
#include <arpa/inet.h>
#include <netdb.h>
#include <unistd.h>
#include <getopt.h>

typedef int Socket;
#endif

#if defined(__APPLE__) && defined(__MACH__)
#define APPLE
#endif
