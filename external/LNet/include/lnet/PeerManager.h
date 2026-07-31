#pragma once

#include "lnet/Peer.h"

#include <algorithm>
#include <chrono>
#include <mutex>
#include <string>
#include <vector>

#include "logging/Log.h"

namespace LNet {

// Tracks the set of remote endpoints currently considered "alive" for a
// Sender or Receiver, based on any inbound datagram from them (LNet doesn't
// care whether that datagram was a handshake, heartbeat, or data packet —
// receiving anything at all is proof of liveness). Used internally by
// Sender<T> for fan-out and by Receiver<T> for its single upstream peer.
class PeerManager {
public:
    // Called whenever any datagram arrives from a given endpoint.
    void RegisterOrRefresh(const std::string& ip, uint16_t port) {
        std::lock_guard<std::mutex> lock(mutex);

        auto it = std::find_if(peers.begin(), peers.end(),
            [&](const Peer& p) { return p.ip == ip && p.port == port; });

        if (it != peers.end()) {
            it->lastSeen = std::chrono::steady_clock::now();
        } else {
            peers.push_back({ip, port, std::chrono::steady_clock::now()});
            LOG_INFO("LNET", "New peer connected: " + ip + ":" + std::to_string(port));
        }
    }

    // Call periodically (e.g. once a second) to drop peers that have gone
    // quiet for longer than `timeout`.
    void RemoveStale(std::chrono::milliseconds timeout = std::chrono::seconds(5)) {
        std::lock_guard<std::mutex> lock(mutex);
        auto now = std::chrono::steady_clock::now();

        auto before = peers.size();
        peers.erase(
            std::remove_if(peers.begin(), peers.end(),
                [&](const Peer& p) {
                    return (now - p.lastSeen) > timeout;
                }),
            peers.end());

        if (peers.size() != before) {
            LOG_INFO("LNET", "Removed " + std::to_string(before - peers.size()) + " stale peer(s)");
        }
    }

    // Returns a copy of the current active peer list, safe to iterate
    // outside the lock (e.g. for fan-out sending).
    std::vector<Peer> GetActivePeers() const {
        std::lock_guard<std::mutex> lock(mutex);
        return peers;
    }

    size_t Count() const {
        std::lock_guard<std::mutex> lock(mutex);
        return peers.size();
    }

private:
    mutable std::mutex mutex;
    std::vector<Peer> peers;
};

} // namespace LNet
