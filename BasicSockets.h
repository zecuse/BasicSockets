#pragma once

#include <Platform.h>
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>

typedef struct config
{
	const char *mode;
	int family;
	const char *ip;
	const char *port;
	int type;
	int protocol;
} Config;

void ProcessArgs(int argc, char *argv[], Config *options);
void HandleError(int check, const char *func, int err, struct addrinfo **addr, Socket *sock);
int init(struct addrinfo *hints, struct sockaddr_in *addr, Config *def);
int GetServerInfo(struct addrinfo *hints, struct addrinfo **addr, struct sockaddr_in *theiraddr, Config *def);
int CreateSocket(struct addrinfo **addr, Socket *sock);
int NameSocket(Socket *sock, struct addrinfo **addr);
int ListenSocket(Socket *sock, int backlog);
Socket AcceptSocket(Socket *sock, struct sockaddr_in *addr);
int ConnectSocket(Socket *sock, struct addrinfo **addr);
int SelectConnections(int nfds, fd_set *readset, fd_set *writeset, fd_set *exceptset, struct timeval const *timeout, Socket *sock);
int SendMsg(Socket sock, char *buffer, int flags, int type, struct sockaddr_in *to, socklen_t addrlen);
int RecvMsg(Socket sock, char *buffer, int flags, int type, struct sockaddr_in *from, socklen_t *addrlen);
int CloseSocket(Socket *sock);
