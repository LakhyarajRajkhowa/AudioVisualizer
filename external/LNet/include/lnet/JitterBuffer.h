#pragma once

#include <cstdint>
#include <map>
#include <mutex>
#include <optional>

namespace LNet {

// Absorbs network jitter by holding received payloads for `targetDelayUs`
// before releasing them in timestamp order. Generic over payload type T
// (T must be copyable) — has no knowledge of what T actually contains.
// Used internally by Receiver<T>; exposed publicly in case an app wants to
// build a custom receive pipeline without the rest of Receiver<T>.
template <typename T>
class JitterBuffer {
public:
    // targetDelayUs: how long to hold packets before releasing them.
    // Start around 60,000-100,000 (60-100ms) and tune per use case.
    explicit JitterBuffer(uint64_t targetDelayUs = 80000)
        : targetDelay(targetDelayUs) {}

    // Called whenever a new payload arrives off the network (any order).
    void Push(const T& payload, uint64_t translatedTimestampUs) {
        std::lock_guard<std::mutex> lock(mutex);
        if (hasReleasedAny && translatedTimestampUs <= lastReleasedTimestamp) {
            droppedLateCount++;
            return;
        }
        buffer[translatedTimestampUs] = payload;
    }

    // Called every frame with the current render/consumption clock.
    // Returns a payload if one is "due" for consumption, otherwise nullopt.
    // Callers should keep using their last consumed value rather than
    // gating their own update/render loop on this returning a value.
    std::optional<T> PopReadyPacket(uint64_t renderClockUs) {
        std::lock_guard<std::mutex> lock(mutex);

        if (buffer.empty()) return std::nullopt;

        auto it = buffer.begin(); // earliest timestamp
        uint64_t packetTime = it->first;

        // Only release once enough delay has passed to reasonably assume
        // no earlier/out-of-order packet is still in flight.
        if (packetTime + targetDelay <= renderClockUs) {
            T payload = it->second;
            buffer.erase(it);

            lastReleasedTimestamp = packetTime;
            hasReleasedAny = true;

            return payload;
        }

        return std::nullopt;
    }

    size_t PendingCount() const {
        std::lock_guard<std::mutex> lock(mutex);
        return buffer.size();
    }

    uint32_t DroppedLateCount() const { return droppedLateCount; }

private:
    mutable std::mutex mutex;
    std::map<uint64_t, T> buffer; // sorted by timestamp automatically
    uint64_t targetDelay;

    bool hasReleasedAny = false;
    uint64_t lastReleasedTimestamp = 0;
    uint32_t droppedLateCount = 0;
};

} // namespace LNet
