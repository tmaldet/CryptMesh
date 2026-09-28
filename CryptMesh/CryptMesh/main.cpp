#include <sodium.h>
#include <asio.hpp>
#include <iostream>

#include "Server.h"
#include "Client.h"
#include "Utils.h"

using namespace std;

int main()
{
	// Initialize libsodium
	if (sodium_init() < 0)
	{
		cout << "Failed to initialize libsodium." << endl;

		return 1;
	}

	asio::io_context io;

	char choice;

	menu(choice);

	// Exit
	if (choice == '3')
	{
		return 0;
	}

	// Create room
	if (choice == '2')
	{
		string username;
		string password;

		int port;

		cout << "Create a username: ";
		cin >> username;

		cout << "Create a lobby password: ";
		cin >> password;

		cout << "Enter a port: ";
		cin >> port;

		cin.ignore(
			numeric_limits<streamsize>::max(),
			'\n'
		);

		Server server(
			io,
			port,
			password,
			username
		);

		if (server.startServer())
		{
			if (server.waitForClient())
			{
				server.runChat();
			}
		}
	}

	// Join room
	else if (choice == '1')
	{
		string username;
		string password;
		string ipAddress;

		int port;

		cout << "Create a username: ";
		cin >> username;

		cout << "Enter room IP: ";
		cin >> ipAddress;

		cout << "Enter room port: ";
		cin >> port;

		cout << "Enter lobby password: ";
		cin >> password;

		cin.ignore(
			numeric_limits<streamsize>::max(),
			'\n'
		);

		Client client(
			io,
			username
		);

		if (
			client.connectToServer(
				ipAddress,
				port,
				password
			)
			)
		{
			client.runChat();
		}
	}

	return 0;
}
