#include "Server.h"
#include "Crypto.h"
#include "Utils.h"

#include <iostream>
#include <thread>
#include <atomic>

using namespace std;

Server::Server(
    asio::io_context& ioContext,
    int roomPort,
    string password,
    string user
)
    : io(ioContext),
    acceptor(ioContext),
    socket(ioContext),
    port(roomPort),
    roomPassword(password),
    username(user)
{
    randombytes_buf(
        salt,
        sizeof salt
    );

    createKey(
        roomPassword,
        salt,
        key
    );
}

bool Server::startServer()
{
    try
    {
        asio::ip::tcp::endpoint endpoint(
            asio::ip::tcp::v4(),
            port
        );

        acceptor.open(
            endpoint.protocol()
        );

        acceptor.set_option(
            asio::ip::tcp::acceptor::
            reuse_address(true)
        );

        acceptor.bind(endpoint);

        acceptor.listen();

        clearScreen();

        cout << "<SYSTEM> Room created. Waiting for client to join..." << endl;

        return true;
    }
    catch (const exception& e)
    {
        cout << "Failed to start server: " << e.what() << endl;
        return false;
    }
}

bool Server::sendRaw(string message)
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

string Server::receiveRaw()
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

bool Server::sendEncrypted(string message)
{
    return sendRaw(
        encryptMessage(
            message,
            key
        )
    );
}

string Server::receiveEncrypted()
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

bool Server::waitForClient()
{
    try
    {
        acceptor.accept(socket);

        // Send salt to joining user
        sendRaw(
            bytesToHex(
                salt,
                sizeof salt
            )
        );

        // Send encrypted challenge
        sendEncrypted(
            "CRYPTMESH_AUTH"
        );

        // Client must decrypt challenge
        string response =
            receiveEncrypted();

        if (
            response !=
            "CRYPTMESH_AUTH_OK"
            )
        {
            cout << endl;
            cout << "Incorrect password." << endl;

            socket.close();

            return false;
        }

        // Receive username encrypted
        string clientUsername =
            receiveEncrypted();

        clearScreen();

        cout << "<SYSTEM> "
            << clientUsername
            << " joined the room."
            << endl;

        return true;
    }
    catch (const exception& e)
    {
        cout << "Connection failed: "
            << e.what()
            << endl;

        return false;
    }
}

void Server::runChat()
{
    atomic<bool> connected = true;

    // Receive messages on separate thread
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

                    cout << "<SYSTEM> User disconnected." << endl;

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
