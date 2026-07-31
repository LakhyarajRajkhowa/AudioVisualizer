#pragma once

#include "lnet/Config.h"
#include "lnet/ClockSync.h"
#include "lnet/Codec.h"
#include "lnet/JitterBuffer.h"
#include "lnet/Socket.h"
#include "lnet/Utils.h"

#include <atomic>
#include <chrono>
#include <optional>
#include <string>
#include <thread>

namespace LNet {

// Pulls payloads of type T from a single specific host, jitter-buffers them,
// and hands them to the app in arrival-safe timestamp order. T must have a
// Deserialize(BinaryReader&) -> T free function visible via ADL (see
// Codec.h). Sends a periodic zero-payload handshake/heartbeat packet so the
// corresponding Sender<T> knows this Receiver is alive.
template <typename T>
class Receiver {
public:
    ~Receiver() { Shutdown(); }

    // `protocolMagic` must match the Sender's magic; packets with any other
    // magic, or arriving from any IP other than `hostIp`, are ignored.
    // `targetDelayUs` is passed through to the internal JitterBuffer.
    bool Init(const std::string& hostIp, uint16_t hostPort, uint32_t protocolMagic,
              uint64_t targetDelayUs = 80000) {
        this->hostIp = hostIp;
        this->hostPort = hostPort;
        this->magic = protocolMagic;

        jitterBuffer = std::make_unique<JitterBuffer<T>>(targetDelayUs);
        if (!socket.Open(0)) return false; // ephemeral port; sender sees us via recvfrom

        SendHandshake();

        running = true;
        networkThread = std::thread(&Receiver::PollLoop, this);
        return true;
    }

    // Call once per frame/tick. Returns a payload if one is ready to
    // consume, otherwise nullopt — callers should keep using their last
    // received payload rather than gating their own loop on this.
    std::optional<T> TryGetFrame() {
        return jitterBuffer->PopReadyPacket(Utils::GetTimestampUs());
    }

    void Shutdown() {
        if (running) {
            running = false;
            if (networkThread.joinable()) networkThread.join();
            socket.Close();
        }
    }

    size_t PendingCount() const { return jitterBuffer->PendingCount(); }
    uint32_t DroppedLateCount() const { return jitterBuffer->DroppedLateCount(); }
    const std::string& HostIp() const { return hostIp; }

private:
    void SendHandshake() {
        PacketHeader header;
        header.magic = magic;
        header.version = kDefaultProtocolVersion;
        header.type = 0; // handshake/heartbeat carries no payload; type is unused
        header.sequenceNum = 0;
        header.timestampUs = Utils::GetTimestampUs();

        BinaryWriter w;
        WriteHeader(w, header);
        socket.SendTo(hostIp, hostPort, w.Data().data(), w.Data().size());
    }

    void PollLoop() {
        uint8_t buffer[16384];
        std::string fromIp;
        uint16_t fromPort;
        auto lastHeartbeat = std::chrono::steady_clock::now();

        while (running) {
            int received = socket.RecvFrom(buffer, sizeof(buffer), fromIp, fromPort);

            // Only accept packets that came from OUR host, even if another
            // sender on the LAN is broadcasting the same protocol.
            if (received > 0 && fromIp == hostIp) {
                try {
                    BinaryReader r(buffer, static_cast<size_t>(received));
                    PacketHeader header = ReadHeader(r);

                    if (header.magic == magic && r.Remaining() > 0) {
                        T payload = Deserialize(r, TypeTag<T>{}); // ADL: app-provided codec for T
                        uint64_t translatedTs = clockSync.Translate(header.timestampUs);
                        jitterBuffer->Push(payload, translatedTs);
                    }
                } catch (const std::exception&) {
                    // Malformed/truncated datagram — drop silently.
                }
            }

            auto now = std::chrono::steady_clock::now();
            if (now - lastHeartbeat > std::chrono::seconds(2)) {
                SendHandshake(); // reused as heartbeat too — same tiny packet
                lastHeartbeat = now;
            }

            if (received <= 0) std::this_thread::sleep_for(std::chrono::milliseconds(2));
        }
    }

    std::string hostIp;
    uint16_t hostPort = 0;
    uint32_t magic = 0;

    Socket socket;
    std::unique_ptr<JitterBuffer<T>> jitterBuffer;
    ClockSync clockSync;
    std::thread networkThread;
    std::atomic<bool> running{false};
};

} // namespace LNet
