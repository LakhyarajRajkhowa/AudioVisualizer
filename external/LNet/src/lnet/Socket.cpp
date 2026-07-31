#include "lnet/Socket.h"
#include "logging/Log.h"

#include <cstring>

#if defined(_WIN32)
    #include <winsock2.h>
    #include <ws2tcpip.h>
    #pragma comment(lib, "ws2_32.lib")
#else
    #include <sys/socket.h>
    #include <netinet/in.h>
    #include <arpa/inet.h>
    #include <unistd.h>
    #include <fcntl.h>
    #include <errno.h>
#endif

namespace LNet {

#if defined(_WIN32)
static bool g_wsaInitialized = false;

static void EnsureWinsockInit()
{
    if (!g_wsaInitialized)
    {
        WSADATA wsaData;
        if (WSAStartup(MAKEWORD(2, 2), &wsaData) == 0)
        {
            g_wsaInitialized = true;
            LOG_SUCCESS("LNET", "Winsock initialized.");
        }
        else
        {
            LOG_ERROR("LNET", "Failed to initialize Winsock.");
        }
    }
}
#endif

Socket::Socket()
{
#if defined(_WIN32)
    EnsureWinsockInit();
#endif
}

Socket::~Socket()
{
    Close();
}

bool Socket::Open(uint16_t localPort)
{
#if defined(_WIN32)
    socketHandle = static_cast<int>(socket(AF_INET, SOCK_DGRAM, IPPROTO_UDP));

    if (socketHandle == static_cast<int>(INVALID_SOCKET))
    {
        LOG_ERROR("LNET", "Failed to create UDP socket.");
        return false;
    }
#else
    socketHandle = socket(AF_INET, SOCK_DGRAM, IPPROTO_UDP);

    if (socketHandle < 0)
    {
        LOG_ERROR("LNET",
            "Failed to create UDP socket: " + std::string(strerror(errno)));
        return false;
    }
#endif

    sockaddr_in addr{};
    addr.sin_family = AF_INET;
    addr.sin_addr.s_addr = INADDR_ANY;
    addr.sin_port = htons(localPort);

    if (bind(socketHandle,
             reinterpret_cast<sockaddr*>(&addr),
             sizeof(addr)) != 0)
    {
        LOG_ERROR(
            "LNET",
            "Failed to bind UDP socket to port " +
            std::to_string(localPort));

        Close();
        return false;
    }

    SetNonBlocking();

    isOpen = true;

    LOG_SUCCESS(
        "LNET",
        "UDP socket opened on port " +
        std::to_string(localPort));

    return true;
}

void Socket::SetNonBlocking()
{
#if defined(_WIN32)
    u_long mode = 1;
    ioctlsocket(socketHandle, FIONBIO, &mode);
#else
    int flags = fcntl(socketHandle, F_GETFL, 0);
    fcntl(socketHandle, F_SETFL, flags | O_NONBLOCK);
#endif

    LOG_INFO("LNET", "UDP socket set to non-blocking mode.");
}

bool Socket::SendTo(const std::string& ip,
                     uint16_t port,
                     const uint8_t* data,
                     size_t len)
{
    if (!isOpen)
    {
        LOG_ERROR("LNET", "Attempted to send on a closed UDP socket.");
        return false;
    }

    sockaddr_in destAddr{};
    destAddr.sin_family = AF_INET;
    destAddr.sin_port = htons(port);

    if (inet_pton(AF_INET, ip.c_str(), &destAddr.sin_addr) != 1)
    {
        LOG_ERROR(
            "LNET",
            "Invalid destination IP: " + ip);

        return false;
    }

    int sent = sendto(
        socketHandle,
        reinterpret_cast<const char*>(data),
        static_cast<int>(len),
        0,
        reinterpret_cast<sockaddr*>(&destAddr),
        sizeof(destAddr));

    if (sent != static_cast<int>(len))
    {
        LOG_ERROR(
            "LNET",
            "Failed to send UDP packet to " +
            ip + ":" + std::to_string(port));

        return false;
    }

    return true;
}

int Socket::RecvFrom(uint8_t* buffer,
                      size_t maxLen,
                      std::string& outIp,
                      uint16_t& outPort)
{
    if (!isOpen)
    {
        LOG_ERROR("LNET", "Attempted to receive on a closed UDP socket.");
        return -1;
    }

    sockaddr_in srcAddr{};

#if defined(_WIN32)
    int addrLen = sizeof(srcAddr);
#else
    socklen_t addrLen = sizeof(srcAddr);
#endif

    int received = recvfrom(
        socketHandle,
        reinterpret_cast<char*>(buffer),
        static_cast<int>(maxLen),
        0,
        reinterpret_cast<sockaddr*>(&srcAddr),
        &addrLen);

    if (received < 0)
    {
#if defined(_WIN32)
        int err = WSAGetLastError();

        if (err == WSAEWOULDBLOCK)
            return 0;
#else
        if (errno == EWOULDBLOCK || errno == EAGAIN)
            return 0;
#endif

        LOG_ERROR("LNET", "UDP receive failed.");
        return -1;
    }

    char ipStr[INET_ADDRSTRLEN];

    inet_ntop(AF_INET,
              &srcAddr.sin_addr,
              ipStr,
              sizeof(ipStr));

    outIp = ipStr;
    outPort = ntohs(srcAddr.sin_port);

    return received;
}

void Socket::Close()
{
    if (socketHandle >= 0)
    {
#if defined(_WIN32)
        closesocket(socketHandle);
#else
        close(socketHandle);
#endif

        socketHandle = -1;

        LOG_INFO("LNET", "UDP socket closed.");
    }

    isOpen = false;
}

} // namespace LNet
