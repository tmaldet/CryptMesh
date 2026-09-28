#pragma once

#include <string>
#include <vector>

std::string bytesToHex(const unsigned char* data, size_t length);
std::vector<unsigned char> hexToBytes(std::string hex);

bool createKey(
	std::string password,
	const unsigned char salt[crypto_pwhash_SALTBYTES],
	unsigned char key[crypto_secretbox_KEYBYTES]
);

std::string encryptMessage(
	std::string message,
	unsigned char key[crypto_secretbox_KEYBYTES]
);

std::string decryptMessage(
	std::string encryptedMessage,
	unsigned char key[crypto_secretbox_KEYBYTES]
);
