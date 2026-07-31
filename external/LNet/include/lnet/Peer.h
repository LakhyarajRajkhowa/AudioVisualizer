#pragma once

#include <chrono>
#include <cstdint>
#include <string>

namespace LNet {

// A remote UDP endpoint that has recently sent this Sender/Receiver a
// datagram. Generic — not "receiver" or "host" specific, since a Sender's
// peers and a Receiver's peer are conceptually the same kind of thing.
struct Peer {
    std::string ip;
    uint16_t port;
    std::chrono::steady_clock::time_point lastSeen;
};

} // namespace LNet
