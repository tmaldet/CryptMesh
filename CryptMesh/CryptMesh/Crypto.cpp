#include "Crypto.h"

#include <sodium.h>
#include <string>
#include <vector>

using namespace std;

string bytesToHex(const unsigned char* data, size_t length)
{
	string hex;
	hex.resize(length * 2 + 1);

	sodium_bin2hex(
		hex.data(),
		hex.size(),
		data,
		length
	);

	hex.resize(length * 2);

	return hex;
}

vector<unsigned char> hexToBytes(string hex)
{
	vector<unsigned char> bytes(hex.length() / 2);

	size_t outputLength;

	if (sodium_hex2bin(
		bytes.data(),
		bytes.size(),
		hex.c_str(),
		hex.length(),
		nullptr,
		&outputLength,
		nullptr
	) != 0)
	{
		return {};
	}

	bytes.resize(outputLength);

	return bytes;
}

bool createKey(
	string password,
	const unsigned char salt[crypto_pwhash_SALTBYTES],
	unsigned char key[crypto_secretbox_KEYBYTES]
)
{
	if (crypto_pwhash(
		key,
		crypto_secretbox_KEYBYTES,
		password.c_str(),
		password.length(),
		salt,
		crypto_pwhash_OPSLIMIT_INTERACTIVE,
		crypto_pwhash_MEMLIMIT_INTERACTIVE,
		crypto_pwhash_ALG_DEFAULT
	) != 0)
	{
		return false;
	}

	return true;
}

string encryptMessage(
	string message,
	unsigned char key[crypto_secretbox_KEYBYTES]
)
{
	unsigned char nonce[crypto_secretbox_NONCEBYTES];

	randombytes_buf(
		nonce,
		sizeof nonce
	);

	vector<unsigned char> encrypted(
		message.length() +
		crypto_secretbox_MACBYTES
	);

	crypto_secretbox_easy(
		encrypted.data(),
		reinterpret_cast<const unsigned char*>(message.c_str()),
		message.length(),
		nonce,
		key
	);

	string nonceHex =
		bytesToHex(
			nonce,
			sizeof nonce
		);

	string messageHex =
		bytesToHex(
			encrypted.data(),
			encrypted.size()
		);

	return nonceHex + ":" + messageHex;
}

string decryptMessage(
	string encryptedMessage,
	unsigned char key[crypto_secretbox_KEYBYTES]
)
{
	size_t separator =
		encryptedMessage.find(':');

	if (separator == string::npos)
	{
		return "";
	}

	string nonceHex =
		encryptedMessage.substr(
			0,
			separator
		);

	string messageHex =
		encryptedMessage.substr(
			separator + 1
		);

	vector<unsigned char> nonce =
		hexToBytes(nonceHex);

	vector<unsigned char> cipher =
		hexToBytes(messageHex);

	if (nonce.size() != crypto_secretbox_NONCEBYTES)
	{
		return "";
	}

	if (cipher.size() < crypto_secretbox_MACBYTES)
	{
		return "";
	}

	vector<unsigned char> decrypted(
		cipher.size() -
		crypto_secretbox_MACBYTES
	);

	if (crypto_secretbox_open_easy(
		decrypted.data(),
		cipher.data(),
		cipher.size(),
		nonce.data(),
		key
	) != 0)
	{
		return "";
	}

	return string(
		decrypted.begin(),
		decrypted.end()
	);
}
