#include <BasicSockets.h>
#include <Utilities/strings.h>
#ifdef UNIX
void ProcessArgs(int argc, char *argv[], Config *options)
{
	struct option long_options[] = 
	{
		{"setup", required_argument, NULL, 's'},
		{"ip", required_argument, NULL, 'i'},
		{"port", required_argument, NULL, 'p'},
		{"type", required_argument, NULL, 't'},
		{NULL, 0, NULL, 0},
	};

	struct sockaddr_in addr;
	int c;
	while ((c = getopt_long(argc, argv, "", long_options, NULL)) != -1)
	{
		strLwr(optarg);
		switch (c)
		{
		case 's':
			if (strcmp(optarg, "server") == 0)
				options->mode = optarg;
			else
				printf("%s is an unrecognized mode. Reverting to client.\n", optarg);
			break;
		case 'i':
			if (strchr(optarg, '.') == NULL)
				options->family = AF_INET6;
			if (inet_pton(options->family, optarg, &addr.sin_addr))
				options->ip = optarg;
			else
				printf("%s is an invalid IP address. Reverting to localhost.\n", optarg);
			break;
		case 'p':
			if (atoi(optarg) < 65536)
				options->port = optarg;
			else
				printf("%s is an invalid port number. Reverting to loopback.\n", optarg);
			break;
		case 't':
			if (strcmp(optarg, "udp") == 0)
			{
				options->type = SOCK_DGRAM;
				options->protocol = IPPROTO_UDP;
			}
			else
				printf("%s is an unrecognized type. Reverting to 'TCP'.\n", optarg);
			break;
		default:
			printf("'%s' and/or '%s' are unrecognized. This has been ignored.\n", argv[optind - 1], optarg);
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
	err = err + 1; // This is to remove the unused parameter warning because this is only used on Windows.
	switch (check)
	{
	case 4:
	case 3:
		close(*sock);
		FALLTHROUGH;
	case 2:
		if (check < 4)
			freeaddrinfo(*addr);
		FALLTHROUGH;
	case 1:
	case 0:
		printf("%s failed.\n", func);
	}
	return;
}

int init(struct addrinfo *hints, struct sockaddr_in *addr, Config *def)
{
	memset(addr, 0, sizeof(struct sockaddr_in));
	memset(hints, 0, sizeof(struct addrinfo));
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
		HandleError(1, "getaddrinfo", 0, NULL, NULL);
		return 0;
	}
	theiraddr->sin_family = def->family;
	theiraddr->sin_port = htons(atoi(def->port));
	inet_pton(def->family, def->ip, &theiraddr->sin_addr.s_addr);
	return 1;
}

int CreateSocket(struct addrinfo **addr, Socket *sock)
{
	*sock = socket((*addr)->ai_family, (*addr)->ai_socktype, (*addr)->ai_protocol);
	if (*sock == -1)
	{
		HandleError(2, "socket", 0, addr, NULL);
		return 0;
	}
	return 1;
}

int NameSocket(Socket *sock, struct addrinfo **addr)
{
	if (bind(*sock, (*addr)->ai_addr, (int)(*addr)->ai_addrlen) == -1)
	{
		HandleError(3, "bind", 0, addr, sock);
		return 0;
	}
	return 1;
}

int ListenSocket(Socket *sock, int backlog)
{
	backlog = backlog <= 0 ? 10 : backlog;
	if (listen(*sock, backlog) == -1)
	{
		HandleError(4, "listen", 0, NULL, sock);
	}
	return 1;
}

Socket AcceptSocket(Socket *sock, struct sockaddr_in *addr)
{
	socklen_t addrlen = sizeof(struct sockaddr);
	Socket fd = accept(*sock, (struct sockaddr *)addr, &addrlen);
	if (fd == -1)
		HandleError(4, "accept", 0, NULL, sock);
	return fd;
}

int ConnectSocket(Socket *sock, struct addrinfo **addr)
{
	if (connect(*sock, (*addr)->ai_addr, (int)(*addr)->ai_addrlen) == -1)
	{
		HandleError(3, "connect", 0, addr, sock);
		return 0;
	}
	return 1;
}

int SelectConnections(int nfds, fd_set *readset, fd_set *writeset, fd_set *exceptset, struct timeval *timeout, Socket *sock)
{
	int connections = select(nfds, readset, writeset, exceptset, timeout);
	if (connections == -1)
		HandleError(4, "select", 0, NULL, sock);
	return connections;
}

int SendMsg(Socket sock, char *buffer, int flags, int type, struct sockaddr_in *to, socklen_t addrlen)
{
	int bytes = -1, size = strlen(buffer);
	size = size > BUFSIZ - 1 ? BUFSIZ - 1 : size;
	if (type == SOCK_DGRAM)
		bytes = sendto(sock, buffer, size, flags, (struct sockaddr *)to, addrlen);
	else
		bytes = send(sock, buffer, size, flags);
	if (bytes == -1)
		HandleError(0, "send", 0, NULL, NULL);
	return bytes;
}

int RecvMsg(Socket sock, char *buffer, int flags, int type, struct sockaddr_in *from, socklen_t *addrlen)
{
	int bytes = -1;
	if (type == SOCK_DGRAM)
		bytes = recvfrom(sock, buffer, BUFSIZ - 1, flags, (struct sockaddr *)from, addrlen);
	else
		bytes = recv(sock, buffer, BUFSIZ - 1, flags);
	if (bytes == -1)
		HandleError(0, "recv", 0, NULL, NULL);
	else
		buffer[bytes] = '\0';
	return bytes;
}

int CloseSocket(Socket *sock)
{
	if (close(*sock) == -1)
	{
		HandleError(1, "close", 0, NULL, NULL);
		return 0;
	}
	return 1;
}

void CleanUp()
{
	return;
}
#endif
