#pragma once

#include <asio.hpp>
#include <iostream>

void clearScreen();
std::string getLocalIP(asio::io_context& io);
void menu(char& outChoice);
