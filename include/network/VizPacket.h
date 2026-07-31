#pragma once

#include "lnet/BinaryReader.h"
#include "lnet/BinaryWriter.h"
#include "lnet/Codec.h"

#include <cstdint>
#include <vector>

// The audio-analysis payload streamed from VisualizerHost to VisualizerReceiver(s).
// This struct and its Serialize/Deserialize free functions are the only place
// in the app that LNet's Sender<T>/Receiver<T> reach into via ADL — LNet
// itself never sees these fields.
struct VizPacket {
    int32_t audioId = 0;

    float bass = 0.0f;
    float mid = 0.0f;
    float treble = 0.0f;

    std::vector<float> spectrum;
    std::vector<float> smoothSpectrum;
    std::vector<float> logSpectrum;
};

// Produces a "nothing playing" packet — zeroed levels, empty spectrum arrays.
// Handy as the initial value for a cached last-received packet (e.g. before
// the first real frame arrives, or once a receiver disconnects), so render
// code can always read *some* VizPacket unconditionally rather than
// special-casing a missing one.
inline VizPacket MakeSilentPacket(int32_t audioId) {
    VizPacket pkt;
    pkt.audioId = audioId;
    pkt.bass = 0.0f;
    pkt.mid = 0.0f;
    pkt.treble = 0.0f;
    // spectrum/smoothSpectrum/logSpectrum default-construct empty already.
    return pkt;
}
inline void Serialize(LNet::BinaryWriter& w, const VizPacket& pkt) {
    w.WriteI32(pkt.audioId);

    w.WriteFloat(pkt.bass);
    w.WriteFloat(pkt.mid);
    w.WriteFloat(pkt.treble);

    w.WriteArray(pkt.spectrum);
    w.WriteArray(pkt.smoothSpectrum);
    w.WriteArray(pkt.logSpectrum);
}

// Required by LNet::Receiver<VizPacket>::PollLoop() (called via
// Deserialize(reader, LNet::TypeTag<VizPacket>{})).
inline VizPacket Deserialize(LNet::BinaryReader& r, LNet::TypeTag<VizPacket>) {
    VizPacket pkt;

    pkt.audioId = r.ReadI32();

    pkt.bass = r.ReadFloat();
    pkt.mid = r.ReadFloat();
    pkt.treble = r.ReadFloat();

    pkt.spectrum = r.ReadArray<float>();
    pkt.smoothSpectrum = r.ReadArray<float>();
    pkt.logSpectrum = r.ReadArray<float>();

    return pkt;
}