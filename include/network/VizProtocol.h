#pragma once

#include <cstdint>

// Protocol identity for this app's use of LNet. These values are deliberately
// NOT part of LNet itself — they're what makes THIS protocol distinguishable
// from any other app built on LNet sharing the same network.

// Arbitrary 4-byte tag written into every packet header. Rejects stray UDP
// traffic from unrelated LNet-based apps on the same LAN/port.
constexpr uint32_t VIZ_PROTOCOL_MAGIC = 0x4C56495A; // "LVIZ"

// Default port VisualizerHost listens on.
constexpr uint16_t VIZ_DATA_PORT = 9500;

// LNet's PacketHeader::type field is opaque to LNet — this app only has one
// payload kind right now, but the enum leaves room to add more (e.g. a
// control-channel message) without changing the wire format.
enum class VizMsgType : uint8_t {
    VizData = 1,
};
