#pragma once

#include "lnet/Config.h"
#include "lnet/Codec.h"
#include "lnet/PeerManager.h"
#include "lnet/Socket.h"
#include "lnet/Utils.h"

#include "logging/Log.h"

#include <atomic>
#include <chrono>
#include <thread>

namespace LNet {

// Broadcasts payloads of type T to every peer that has recently contacted
// this Sender. T must have a Serialize(BinaryWriter&, const T&) free
// function visible via ADL (see Codec.h). Peers are discovered implicitly:
// any inbound datagram (typically from a matching Receiver<T>'s handshake/
// heartbeat) registers that endpoint for future fan-out — Sender doesn't
// need or parse the datagram's contents to do this.
template <typename T>
class Sender {
public:
    // Starts listening on `listenPort` for peer handshakes/heartbeats.
    // `protocolMagic` is written into every outgoing packet header — pick an
    // app-specific value so unrelated LNet traffic on the same network can't
    // be mistaken for this protocol. `payloadType` is an app-defined tag
    // (e.g. an enum cast to uint8_t) identifying this payload kind in the
    // header; LNet does not interpret it.
    bool Init(uint16_t listenPort, uint32_t protocolMagic, uint8_t payloadType,
              uint8_t protocolVersion = kDefaultProtocolVersion) {
        if (!socket.Open(listenPort)) {
            return false;
        }

        magic = protocolMagic;
        type = payloadType;
        version = protocolVersion;

        running = true;
        listenThread = std::thread(&Sender::ListenLoop, this);

        LOG_SUCCESS("LNET", "Sender started.");
        return true;
    }

    // Serializes `payload` and sends it to every currently registered peer.
    // Call once per frame/tick per logical stream.
    void Broadcast(const T& payload) {
        PacketHeader header;
        header.magic = magic;
        header.version = version;
        header.type = type;
        header.sequenceNum = sequenceCounter++;
        header.timestampUs = Utils::GetTimestampUs();

        BinaryWriter w;
        WriteHeader(w, header);
        Serialize(w, payload); // ADL: app-provided codec for T

        auto activePeers = peers.GetActivePeers(); // one lock, then iterate freely
        for (const auto& p : activePeers) {
            socket.SendTo(p.ip, p.port, w.Data().data(), w.Data().size());
        }
    }

    size_t ConnectedPeerCount() const { return peers.Count(); }

    void Shutdown() {
        if (!running) return;

        LOG_INFO("LNET", "Shutting down sender...");
        running = false;

        if (listenThread.joinable())
            listenThread.join();

        socket.Close();

        LOG_SUCCESS("LNET", "Sender stopped.");
    }

    ~Sender() { Shutdown(); }

private:
    void ListenLoop() {
        uint8_t buffer[256];
        std::string fromIp;
        uint16_t fromPort;
        auto lastCleanup = std::chrono::steady_clock::now();

        while (running) {
            int received = socket.RecvFrom(buffer, sizeof(buffer), fromIp, fromPort);
            if (received > 0) {
                // Any datagram at all — handshake, heartbeat, or otherwise —
                // is proof of a live peer. Contents are irrelevant here.
                peers.RegisterOrRefresh(fromIp, fromPort);
            }

            auto now = std::chrono::steady_clock::now();
            if (now - lastCleanup > std::chrono::seconds(1)) {
                peers.RemoveStale();
                lastCleanup = now;
            }

            std::this_thread::sleep_for(std::chrono::milliseconds(5));
        }
    }

    Socket socket;
    PeerManager peers;
    std::thread listenThread;
    std::atomic<bool> running{false};

    uint32_t magic = 0;
    uint8_t type = 0;
    uint8_t version = kDefaultProtocolVersion;
    uint32_t sequenceCounter = 0;
};

} // namespace LNet
