#include "Utils.h"

#include <iostream>

using namespace std;

void clearScreen()
{
#ifdef _WIN32
	system("cls");
#else
	system("clear");
#endif
}

string getLocalIP(asio::io_context& io)
{
	try
	{
		asio::ip::udp::resolver resolver(io);

		auto results =
			resolver.resolve(
				asio::ip::host_name(),
				""
			);

		for (auto& result : results)
		{
			auto address =
				result.endpoint().address();

			if (address.is_v4() &&
				!address.is_loopback())
			{
				return address.to_string();
			}
		}
	}
	catch (...)
	{
	}

	return "Unable to determine IP";
}

void menu(char& outChoice)
{
	cout << "================================================================================================================" << endl;

	cout << R"(
  /$$$$$$                                  /$$     /$$      /$$                     /$$
 /$$__  $$                                | $$    | $$$    /$$$                    | $$
| $$  \__/  /$$$$$$  /$$   /$$  /$$$$$$  /$$$$$$  | $$$$  /$$$$  /$$$$$$   /$$$$$$$| $$$$$$$
| $$       /$$__  $$| $$  | $$ /$$__  $$|_  $$_/  | $$ $$/$$ $$ /$$__  $$ /$$_____/| $$__  $$
| $$      | $$  \__/| $$  | $$| $$  \ $$  | $$    | $$  $$$| $$| $$$$$$$$|  $$$$$$ | $$  \ $$
| $$    $$| $$      | $$  | $$| $$  | $$  | $$ /$$| $$\  $ | $$| $$_____/ \____  $$| $$  | $$
|  $$$$$$/| $$      |  $$$$$$$| $$$$$$$/  |  $$$$/| $$ \/  | $$|  $$$$$$$ /$$$$$$$/| $$  | $$
 \______/ |__/       \____  $$| $$\n                     /$$  | $$| $$\n                    |  $$$$$$/| $$\n                     \______/ |__/
	)" << endl;

	cout << "================================================================================================================" << endl;

	cout << "1. Join a room" << endl;
	cout << "2. Create a room" << endl;
	cout << "3. Exit" << endl;

	char choice = '0';

	while (
		choice != '1' &&
		choice != '2' &&
		choice != '3'
		)
	{
		cout << ">>> ";
		cin >> choice;
	}

	outChoice = choice;
}
