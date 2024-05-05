#include <BasicSockets.h>

int main(int argc, char *argv[])
{
	// Defaults should no args be supplied.
	Config reverts =
	{
		"client",    // Client mode
		AF_INET,     // IPv4
		"127.0.0.1", // Localhost
		"8080",      // Loopback
		SOCK_STREAM, // TCP
		IPPROTO_TCP  // TCP
	};

	struct addrinfo hints, *myaddr;
	struct sockaddr_in theiraddr;
	socklen_t addrlen = sizeof(theiraddr);
	Socket MySocket, TheirSocket;
	char buffer[BUFSIZ] = { '\0' };

	ProcessArgs(argc, argv, &reverts);
	// Prepare addresses
	if (!init(&hints, &theiraddr, &reverts)) return 1;
	if (!GetServerInfo(&hints, &myaddr, &theiraddr, &reverts)) return 1;

	// Create socket
	if (!CreateSocket(&myaddr, &MySocket)) return 1;

	// server mode
	if (strcmp(reverts.mode, "server") == 0)
	{
		// Bind socket to address
		if (!NameSocket(&MySocket, &myaddr)) return 1;
		freeaddrinfo(myaddr);
		// TCP
		if (reverts.type == SOCK_STREAM)
		{
			// Begin listening for connections
			if (!ListenSocket(&MySocket, 1)) return 1;
			// Accept a connection
			TheirSocket = AcceptSocket(&MySocket, &theiraddr);
			if (TheirSocket == -1) return 1;

			// Demonstrate we're connected
			strcpy_s(buffer, BUFSIZ, "You've connected!\n");
			SendMsg(TheirSocket, buffer, 0, 0, 0, 0);
			RecvMsg(TheirSocket, buffer, 0, 0, 0, 0);
			printf("CLIENT> %s", buffer);
		}
		// UDP
		else
		{
			// Demonstrate we're connected
			RecvMsg(MySocket, buffer, 0, reverts.type, &theiraddr, &addrlen);
			printf("CLIENT> %s", buffer);
			strcpy_s(buffer, BUFSIZ, "Understood.\n");
			SendMsg(MySocket, buffer, 0, reverts.type, &theiraddr, addrlen);
		}
	}
	// client mode
	else
	{
		// TCP
		if (reverts.type == SOCK_STREAM)
		{
			// Try to connect to the server
			if (!ConnectSocket(&MySocket, &myaddr)) return 1;

			// Demonstrate we're connected
			RecvMsg(MySocket, buffer, 0, 0, 0, 0);
			printf("SERVER> %s", buffer);
			strcpy_s(buffer, BUFSIZ, "Understood.\n");
			SendMsg(MySocket, buffer, 0, 0, 0, 0);
		}
		// UDP
		else
		{
			// Demonstrate we're connected
			strcpy_s(buffer, BUFSIZ, "I connected.\n");
			SendMsg(MySocket, buffer, 0, reverts.type, &theiraddr, addrlen);
			RecvMsg(MySocket, buffer, 0, reverts.type, &theiraddr, &addrlen);
			printf("SERVER> %s", buffer);
		}
	}

	if (!CloseSocket(&MySocket)) return 1;
	WSACleanup();
	return 0;
}
