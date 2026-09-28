#include "Client.h"
#include "Crypto.h"
#include "Utils.h"

#include <iostream>
#include <thread>
#include <atomic>

using namespace std;

Client::Client(
	asio::io_context& ioContext,
	string user
)
	: io(ioContext),
	socket(ioContext),
	username(user)
{
}

bool Client::connectToServer(
	string ipAddress,
	int port,
	string password
)
{
	try
	{
		asio::ip::tcp::endpoint endpoint(
			asio::ip::make_address(
				ipAddress
			),
			port
		);

		socket.connect(endpoint);

		// Receive room salt
		string saltHex =
			receiveRaw();

		vector<unsigned char> salt =
			hexToBytes(saltHex);

		if (
			salt.size() !=
			crypto_pwhash_SALTBYTES
			)
		{
			return false;
		}

		if (!createKey(
			password,
			salt.data(),
			key
		))
		{
			return false;
		}

		// Receive encrypted challenge
		string challenge =
			receiveEncrypted();

		if (
			challenge !=
			"CRYPTMESH_AUTH"
		)
		{
			cout << "Incorrect lobby password." << endl;

			return false;
		}

		// Confirm password worked
		sendEncrypted(
			"CRYPTMESH_AUTH_OK"
		);

		// Send username encrypted
		sendEncrypted(
			username
		);

		clearScreen();

		cout << "<SYSTEM> Connected to room." << endl;

		return true;
	}
	catch (const exception& e)
	{
		cout << "Failed to connect." << endl;

		cout << "Error: "
			<< e.what()
			<< endl;

		return false;
	}
}

bool Client::sendRaw(string message)
{
	try
	{
		message += "\n";

		asio::write(
			socket,
			asio::buffer(message)
		);

		return true;
	}
	catch (...)
	{
		return false;
	}
}

string Client::receiveRaw()
{
	try
	{
		asio::streambuf buffer;

		asio::read_until(
			socket,
			buffer,
			'\n'
		);

		istream input(&buffer);

		string message;

		getline(
			input,
			message
		);

		return message;
	}
	catch (...)
	{
		return "";
	}
}

bool Client::sendEncrypted(string message)
{
	return sendRaw(
		encryptMessage(
			message,
			key
		)
	);
}

string Client::receiveEncrypted()
{
	string encrypted =
		receiveRaw();

	if (encrypted.empty())
	{
		return "";
	}

	return decryptMessage(
		encrypted,
		key
	);
}

void Client::runChat()
{
	atomic<bool> connected = true;

	thread receiveThread(
		[&]()
		{
			while (connected)
			{
				string message =
					receiveEncrypted();

				if (message.empty())
				{
					connected = false;

					lock_guard<mutex> lock(
						consoleMutex
					);

					cout << endl;

					cout << "<SYSTEM> Disconnected." << endl;

					break;
				}

				lock_guard<mutex> lock(
					consoleMutex
				);

				cout << endl;
				cout << message << endl;
				cout << ">>> " << flush;
			}
		}
	);

	string message;

	while (connected)
	{
		{
			lock_guard<mutex> lock(
				consoleMutex
			);

			cout << ">>> " << flush;
		}

		getline(
			cin,
			message
		);

		if (
			message == "/exit"
			)
		{
			connected = false;

			socket.close();

			break;
		}

		if (message.empty())
		{
			continue;
		}

		string formattedMessage =
			"<" +
			username +
			"> " +
			message;

		sendEncrypted(
			formattedMessage
		);

		lock_guard<mutex> lock(
			consoleMutex
		);

		cout << formattedMessage
			<< endl;
	}

	if (receiveThread.joinable())
	{
		receiveThread.join();
	}
}
