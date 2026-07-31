#pragma once

#include <chrono>
#include <cstdint>

namespace LNet {
namespace Utils {

// Monotonic microsecond timestamp. Used throughout LNet for packet
// timestamping, jitter-buffer scheduling, and clock-offset calibration.
// Not wall-clock time — only comparable to other calls on the same machine.
inline uint64_t GetTimestampUs() {
    using namespace std::chrono;
    return duration_cast<microseconds>(
        steady_clock::now().time_since_epoch()
    ).count();
}

} // namespace Utils
} // namespace LNet
