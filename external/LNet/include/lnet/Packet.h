#pragma once

#include <cstdint>

namespace LNet {

// Generic wire header prepended to every datagram sent by Sender<T>/Receiver<T>.
// LNet does not assign meaning to `magic` or `type` — the app chooses its own
// magic number (protocol identity, e.g. to reject foreign traffic on the same
// port) and its own `type` enum (message kinds, e.g. Handshake/Data/Heartbeat).
// A zero-payload packet (just this header) is a valid datagram and is how
// Receiver<T> announces itself / sends heartbeats without needing a payload.
struct PacketHeader {
    uint32_t magic       = 0;
    uint8_t  version     = 0;
    uint8_t  type        = 0;
    uint32_t sequenceNum = 0;
    uint64_t timestampUs = 0;
};

} // namespace LNet
