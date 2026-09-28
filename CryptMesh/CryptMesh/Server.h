#pragma once

#include <asio.hpp>
#include <string>
#include <mutex>
#include <sodium.h>

class Server
{
private:
    asio::io_context& io;
    asio::ip::tcp::acceptor acceptor;
    asio::ip::tcp::socket socket;

    int port;

    std::string roomPassword;
    std::string username;

    unsigned char salt[crypto_pwhash_SALTBYTES];
    unsigned char key[crypto_secretbox_KEYBYTES];

    std::mutex consoleMutex;

public:
    Server(
        asio::io_context& ioContext,
        int roomPort,
        std::string password,
        std::string user
    );

    bool startServer();
    bool waitForClient();

    bool sendRaw(std::string message);
    std::string receiveRaw();

    bool sendEncrypted(std::string message);
    std::string receiveEncrypted();

    void runChat();
};