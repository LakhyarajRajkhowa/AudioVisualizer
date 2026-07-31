#pragma once

#include <cstdint>

// Library-wide constants only. LNet deliberately does NOT define any
// protocol magic numbers, message-type enums, or default ports here —
// those are protocol identity and belong to the application using LNet
// (e.g. the audio visualizer's VizProtocol.h). Two apps built on LNet
// should never accidentally interoperate just because they share a
// hardcoded constant.

namespace LNet {

constexpr uint8_t kDefaultProtocolVersion = 1;

} // namespace LNet
