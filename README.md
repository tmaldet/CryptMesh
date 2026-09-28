# CryptMesh

CryptMesh is a peer-to-peer encrypted chat room program written in C++.

## Features

- Create and join chat rooms
- Username support
- Password protected rooms
- Encrypted messaging using libsodium
- TCP networking using Asio
- Simple terminal-based interface

## Libraries

- Asio
- libsodium

## Building

This project uses CMake and vcpkg for dependency management.

Required packages:

```bash
vcpkg install asio:x64-windows libsodium:x64-windows
```

## Usage

Run CryptMesh and choose to either:

1. Join a room
2. Create a room
3. Exit

When creating a room, share the displayed IP address, port, and password with the person you want to connect with.

## Status

CryptMesh is currently in development.

