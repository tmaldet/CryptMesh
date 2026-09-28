find_package(asio CONFIG REQUIRED)
find_package(unofficial-sodium CONFIG REQUIRED)

add_executable(
    CryptMesh
    "main.cpp"
    "Server.cpp"
    "Server.h"
    "Client.cpp"
    "Client.h"
    "Crypto.cpp"
    "Crypto.h"
    "Utils.cpp"
    "Utils.h"
)

set_property(TARGET CryptMesh PROPERTY CXX_STANDARD 20)

target_link_libraries(
    CryptMesh PRIVATE
    asio::asio
    unofficial-sodium::sodium
)
