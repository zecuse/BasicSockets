#include <BasicSockets.h>
#include <Utilities/strings.h>
#ifdef WINDOWS
/*
 * Note to self
 * INVALID_SOCKET = int max - 1 (32 or 64 bit)
 * SOCKET_ERROR = -1
 */

void ProcessArgs(int argc, char *argv[], Config *options)
{
	typedef struct option
	{
		const char *name;	// Long name of option
		int hasArg;			// -1 = none, 0 = optional, 1 = required
		int val;			// Short name for option
	} Option;
	Option longOpts[] =
	{
		{"setup", 1, 's'},
		{"ip", 1, 'i'},
		{"port", 1, 'p'},
		{"type", 1, 't'},
	};

	if (argc < 2)
		printf("You are using the default parameters\n");
	else if (argc == 2)
	{
		char *mode = argv[1];
		strLwr(mode);
		if (strcmp(mode, "server") == 0)
			options->mode = "server";
		else if (strcmp(mode, "client") != 0)
			printf("%s is an unrecognized mode. Reverting to 'client'.\n", mode);
	}
	else
	{
		for (int i = 1; i < argc; ++i)
		{
			char *split[2];
			char *delim = "=";
			if (strchr(argv[i], '=') != NULL)
			{
				char *next = NULL;
				split[0] = strtok_s(argv[i], delim, &next);
				strLwr(split[0]);
				split[1] = strtok_s(NULL, delim, &next);
				strLwr(split[1]);
			}
			else
			{
				split[0] = argv[i++];
				strLwr(split[0]);
				split[1] = argv[i];
				strLwr(split[1]);
			}

			char opt = split[0][1];
			if (opt == '-')
			{
				int len = sizeof(longOpts) / sizeof(Option);
				split[0] = split[0] + 2;
				for (int i = 0; i < len && opt == '-'; ++i)
					if (strcmp(split[0], longOpts[i].name) == 0)
						opt = longOpts[i].val;
			}

			struct sockaddr_in addr;
			switch (opt)
			{
			case 's':
				if (strcmp(split[1], "server") == 0)
					options->mode = "server";
				else if (strcmp(split[1], "client") != 0)
					printf("%s is an unrecognized mode. Reverting to 'client'.\n", split[1]);
				break;
			case 'i':
				if (strchr(split[1], '.') == NULL)
					options->family = AF_INET6;
				if (inet_pton(options->family, split[1], &addr.sin_addr))
					options->ip = split[1];
				else
					printf("%s is an invalid IP address. Reverting to localhost.\n", split[1]);
				break;
			case 'p':
				if (atoi(split[1]) < 65536)
					options->port = split[1];
				else
					printf("%s is an invalid port number. Reverting to loopback.\n", split[1]);
				break;
			case 't':
				if (strcmp(split[1], "udp") == 0)
				{
					options->type = SOCK_DGRAM;
					options->protocol = IPPROTO_UDP;
				}
				else
					printf("%s is an unrecognized type. Reverting to 'TCP'.\n", split[1]);
				break;
			default:
				printf("'%s' and/or '%s' are unrecognized. These have been ignored.\n", split[0], split[1]);
			}
		}
	}

	printf("The following options are being used:\n");
	printf("      Mode:  %s\n", options->mode);
	printf("IP address:  %s\n", options->ip);
	printf("      Port:  %s\n", options->port);
	printf("  Protocol:  %s\n", options->type == SOCK_STREAM ? "TCP" : "UDP");
}

void HandleError(int check, const char *func, int err, struct addrinfo **addr, Socket *sock)
{
	char msg[256] = { 0 };
	switch (check)
	{
	case 4:
	case 3:
		closesocket(*sock);
	case 2:
		if (check < 4)
			freeaddrinfo(*addr);
	case 1:
		WSACleanup();
	case 0:
		FormatMessage(FORMAT_MESSAGE_FROM_SYSTEM | FORMAT_MESSAGE_IGNORE_INSERTS,
			NULL, err, MAKELANGID(LANG_NEUTRAL, SUBLANG_DEFAULT),
			msg, sizeof(msg), NULL);
		printf("%s failed with error %d:\n%s\n", func, err, msg);
	}
	return;
}

int init(struct addrinfo *hints, struct sockaddr_in *addr, Config *def)
{
	WSADATA data;
	int result = WSAStartup(MAKEWORD(2, 2), &data);
	if (result != 0)
	{
		HandleError(0, "WSAStartup", result, NULL, NULL);
		return 0;
	}
	ZeroMemory(addr, sizeof(*addr));
	ZeroMemory(hints, sizeof(*hints));
	hints->ai_family = def->family;
	hints->ai_socktype = def->type;
	hints->ai_protocol = def->protocol;
	hints->ai_flags = AI_PASSIVE;
	return 1;
}

int GetServerInfo(struct addrinfo *hints, struct addrinfo **addr, struct sockaddr_in *theiraddr, Config *def)
{
	if (getaddrinfo(def->ip, def->port, hints, addr) != 0)
	{
		HandleError(1, "getaddrinfo", WSAGetLastError(), NULL, NULL);
		return 0;
	}
	theiraddr->sin_family = def->family;
	theiraddr->sin_port = htons(atoi(def->port));
	inet_pton(def->family, def->ip, &theiraddr->sin_addr.s_addr);
	return 1;
}

int CreateSocket(struct addrinfo **addr, Socket *sock)
{
	*sock = WSASocketW((*addr)->ai_family, (*addr)->ai_socktype, (*addr)->ai_protocol, NULL, 0, 0);
	if (*sock == INVALID_SOCKET)
	{
		HandleError(2, "socket", WSAGetLastError(), addr, NULL);
		return 0;
	}
	return 1;
}

int NameSocket(Socket *sock, struct addrinfo **addr)
{
	if (bind(*sock, (*addr)->ai_addr, (int)(*addr)->ai_addrlen) == SOCKET_ERROR)
	{
		HandleError(3, "bind", WSAGetLastError(), addr, sock);
		return 0;
	}
	return 1;
}

int ListenSocket(Socket *sock, int backlog)
{
	backlog = backlog <= 0 ? SOMAXCONN : backlog;
	if (listen(*sock, backlog) == SOCKET_ERROR)
	{
		HandleError(4, "listen", WSAGetLastError(), NULL, sock);
		return 0;
	}
	return 1;
}

Socket AcceptSocket(Socket *sock, struct sockaddr_in *addr)
{
	socklen_t addrlen = sizeof(struct sockaddr);
	Socket fd = accept(*sock, (struct sockaddr *)addr, &addrlen);
	if (fd == SOCKET_ERROR)
		HandleError(4, "accept", WSAGetLastError(), NULL, sock);
	return fd;
}

int ConnectSocket(Socket *sock, struct addrinfo **addr)
{
	if (connect(*sock, (*addr)->ai_addr, (int)(*addr)->ai_addrlen) == SOCKET_ERROR)
	{
		HandleError(3, "connect", WSAGetLastError(), addr, sock);
		return 0;
	}
	return 1;
}

int SelectConnections(int nfds, fd_set *readset, fd_set *writeset, fd_set *exceptset, struct timeval *timeout, Socket *sock)
{
	int connections = select(nfds, readset, writeset, exceptset, timeout);
	if (connections == SOCKET_ERROR)
		HandleError(4, "select", WSAGetLastError(), NULL, sock);
	return connections;
}

int SendMsg(Socket sock, char *buffer, int flags, int type, struct sockaddr_in *to, socklen_t addrlen)
{
	int bytes = SOCKET_ERROR, size = (int)min(strlen(buffer), BUFSIZ - 1);
	if (type == SOCK_DGRAM)
		bytes = sendto(sock, buffer, size, flags, (struct sockaddr *)to, addrlen);
	else
		bytes = send(sock, buffer, size, flags);
	if (bytes == SOCKET_ERROR)
		HandleError(0, "send", WSAGetLastError(), NULL, NULL);
	return bytes;
}

int RecvMsg(Socket sock, char *buffer, int flags, int type, struct sockaddr_in *from, socklen_t *addrlen)
{
	int bytes = SOCKET_ERROR;
	if (type == SOCK_DGRAM)
		bytes = recvfrom(sock, buffer, BUFSIZ - 1, flags, (struct sockaddr *)from, addrlen);
	else
		bytes = recv(sock, buffer, BUFSIZ - 1, flags);
	if (bytes == SOCKET_ERROR)
		HandleError(0, "recv", WSAGetLastError(), NULL, NULL);
	else
		buffer[bytes] = '\0';
	return bytes;
}

int CloseSocket(Socket *sock)
{
	if (closesocket(*sock) == SOCKET_ERROR)
	{
		HandleError(1, "close", WSAGetLastError(), NULL, NULL);
		return 0;
	}
	return 1;
}

void CleanUp()
{
	WSACleanup();
}
#endif