#pragma once

#include "lnet/Receiver.h"
#include "lnet/Sender.h"

#include "network/VizPacket.h"
#include "network/VizProtocol.h"

#include <optional>
#include <string>

// Unified façade over LNet::Sender<VizPacket> / LNet::Receiver<VizPacket>.
//
// Design notes:
//  - Host and Receiver roles use independent sockets/ports (sender listens
//    on VIZ_DATA_PORT, receiver opens an ephemeral port), so they are NOT
//    mutually exclusive. NetworkService exposes them as independent flags
//    rather than a single enum, so a machine can broadcast its own audio
//    AND mirror another host at the same time.
//  - This class owns no threads of its own — LNet::Sender/Receiver already
//    run their own background threads. NetworkService only manages
//    construction/destruction and gives the app one call surface.
//  - All setters are safe to call every frame from an ImGui panel; they
//    no-op if the requested state already matches current state.
class NetworkService {
public:
    ~NetworkService() { Shutdown(); }

    // Starts broadcasting on the given port. Safe to call again to no-op if
    // already hosting on the same port.
    bool StartHost(uint16_t port = VIZ_DATA_PORT);
    void StopHost();
    bool IsHosting() const { return hosting; }

    // Call once per frame per active audio stream, only while IsHosting().
    void Broadcast(int32_t audioId, float bass, float mid, float treble,
                   const std::vector<float>& spectrum,
                   const std::vector<float>& smoothSpectrum,
                   const std::vector<float>& logSpectrum);

    size_t ConnectedReceiverCount() const {
        return hosting ? sender.ConnectedPeerCount() : 0;
    }

    // ---- Receiver control ----
    bool StartReceiver(const std::string& hostIp,
                        uint16_t hostPort = VIZ_DATA_PORT,
                        uint64_t targetDelayUs = 80000);
    void StopReceiver();
    bool IsReceiving() const { return receiving; }

    // Call once per frame while IsReceiving(). Returns nullopt if no new
    // frame is ready yet — caller should keep rendering its last cached
    // packet rather than gating render on this returning a value.
    std::optional<VizPacket> TryGetFrame();

    // Diagnostics for an ImGui network panel.
    size_t   PendingReceiverPackets() const { return receiving ? receiver.PendingCount() : 0; }
    uint32_t DroppedLateCount()       const { return receiving ? receiver.DroppedLateCount() : 0; }
    const std::string& ReceiverHostIp() const { return currentReceiverIp; }

    // ---- Global ----
    void Shutdown();

private:
    LNet::Sender<VizPacket>   sender;
    LNet::Receiver<VizPacket> receiver;

    bool hosting = false;
    bool receiving = false;

    uint16_t    currentHostPort = 0;
    std::string currentReceiverIp;
    uint16_t    currentReceiverPort = 0;
};
