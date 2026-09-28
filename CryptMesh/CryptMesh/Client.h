#pragma once

#include <asio.hpp>
#include <string>
#include <mutex>

class Client
{
private:
	asio::io_context& io;
	asio::ip::tcp::socket socket;
	std::string username;
	unsigned char key[crypto_secretbox_KEYBYTES];
	std::mutex consoleMutex;

public:
	Client(
		asio::io_context& ioContext,
		std::string user
	);

	bool connectToServer(
		std::string ipAddress,
		int port,
		std::string password
	);

	bool sendRaw(std::string message);
	std::string receiveRaw();

	bool sendEncrypted(std::string message);
	std::string receiveEncrypted();

	void runChat();
};
