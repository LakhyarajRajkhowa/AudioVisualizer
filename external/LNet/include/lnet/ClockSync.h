#pragma once

#include "lnet/Utils.h"

#include <algorithm>
#include <cstdint>
#include <vector>

namespace LNet {

// Translates a remote sender's timestamps into the local clock's frame of
// reference, using the median of the first N observed (local - remote)
// offsets. Two independent machines' steady_clock epochs are unrelated, so
// timestamps embedded in packets are meaningless to the receiver until
// calibrated against arrival time. Used internally by Receiver<T>.
class ClockSync {
public:
    // Call once per received packet with its embedded (remote) timestamp.
    // Returns that timestamp translated into local clock terms — usable
    // immediately even before calibration completes (using a rough
    // per-sample estimate), and stable once calibrated.
    uint64_t Translate(uint64_t remoteTimestampUs) {
        if (!calibrated) {
            int64_t sample = static_cast<int64_t>(Utils::GetTimestampUs())
                            - static_cast<int64_t>(remoteTimestampUs);
            samples.push_back(sample);

            if (samples.size() >= kCalibrationSampleCount) {
                std::vector<int64_t> sorted = samples;
                std::sort(sorted.begin(), sorted.end());
                offset = sorted[sorted.size() / 2]; // median
                calibrated = true;
            } else {
                return remoteTimestampUs + static_cast<uint64_t>(sample);
            }
        }
        return remoteTimestampUs + static_cast<uint64_t>(offset);
    }

    bool IsCalibrated() const { return calibrated; }

private:
    static constexpr size_t kCalibrationSampleCount = 20;

    std::vector<int64_t> samples;
    int64_t offset = 0;
    bool calibrated = false;
};

} // namespace LNet
