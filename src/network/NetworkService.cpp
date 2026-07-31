#include "network/NetworkService.h"

bool NetworkService::StartHost(uint16_t port) {
    if (hosting && currentHostPort == port) return true; // already hosting here

    if (hosting) StopHost(); // switching ports — tear down old one first

    if (!sender.Init(port, VIZ_PROTOCOL_MAGIC, static_cast<uint8_t>(VizMsgType::VizData))) {
        return false;
    }

    hosting = true;
    currentHostPort = port;
    return true;
}

void NetworkService::StopHost() {
    if (!hosting) return;
    sender.Shutdown();
    hosting = false;
    currentHostPort = 0;
}

void NetworkService::Broadcast(int32_t audioId, float bass, float mid, float treble,
                                const std::vector<float>& spectrum,
                                const std::vector<float>& smoothSpectrum,
                                const std::vector<float>& logSpectrum) {
    if (!hosting) return;

    VizPacket pkt;
    pkt.audioId = audioId;
    pkt.bass = bass;
    pkt.mid = mid;
    pkt.treble = treble;
    pkt.spectrum = spectrum;
    pkt.smoothSpectrum = smoothSpectrum;
    pkt.logSpectrum = logSpectrum;

    sender.Broadcast(pkt);
}

bool NetworkService::StartReceiver(const std::string& hostIp, uint16_t hostPort, uint64_t targetDelayUs) {
    if (receiving && currentReceiverIp == hostIp && currentReceiverPort == hostPort)
        return true; // already receiving from this endpoint

    if (receiving) StopReceiver(); // switching target — tear down old one first

    if (!receiver.Init(hostIp, hostPort, VIZ_PROTOCOL_MAGIC, targetDelayUs)) {
        return false;
    }

    receiving = true;
    currentReceiverIp = hostIp;
    currentReceiverPort = hostPort;
    return true;
}

void NetworkService::StopReceiver() {
    if (!receiving) return;
    receiver.Shutdown();
    receiving = false;
    currentReceiverIp.clear();
    currentReceiverPort = 0;
}

std::optional<VizPacket> NetworkService::TryGetFrame() {
    if (!receiving) return std::nullopt;
    return receiver.TryGetFrame();
}

void NetworkService::Shutdown() {
    StopHost();
    StopReceiver();
}
