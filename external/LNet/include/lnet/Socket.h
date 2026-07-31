#pragma once

#include <cstdint>
#include <string>

namespace LNet {

// Thin cross-platform (Winsock/BSD) non-blocking UDP socket. Pure transport —
// has no knowledge of packets, headers, or any LNet protocol concept above
// raw bytes. Used internally by Sender<T>/Receiver<T>, but is a standalone
// usable primitive on its own if an app just wants raw UDP datagrams.
class Socket {
public:
    Socket();
    ~Socket();

    Socket(const Socket&) = delete;
    Socket& operator=(const Socket&) = delete;

    // Binds to a local port to receive on. Pass 0 to let the OS pick an
    // ephemeral port (fine for a pure sender that doesn't need to receive).
    bool Open(uint16_t localPort);

    // Sends a datagram to a specific destination. Returns false on failure.
    bool SendTo(const std::string& ip, uint16_t port,
                const uint8_t* data, size_t len);

    // Non-blocking receive. Returns number of bytes received (0 if none
    // available right now), or -1 on error.
    int RecvFrom(uint8_t* buffer, size_t maxLen,
                 std::string& outIp, uint16_t& outPort);

    void Close();

    bool IsOpen() const { return isOpen; }

private:
    int socketHandle = -1; // int works fine on both platforms via SOCKET cast
    bool isOpen = false;

    void SetNonBlocking();
};

} // namespace LNet
