#pragma once

#include "lnet/BinaryReader.h"
#include "lnet/BinaryWriter.h"
#include "lnet/Packet.h"

namespace LNet {

// Writes/reads the generic PacketHeader. Used internally by Sender<T>/
// Receiver<T> before delegating to the app's payload codec.
inline void WriteHeader(BinaryWriter& w, const PacketHeader& h) {
    w.WriteU32(h.magic);
    w.WriteU8(h.version);
    w.WriteU8(h.type);
    w.WriteU32(h.sequenceNum);
    w.WriteU64(h.timestampUs);
}

inline PacketHeader ReadHeader(BinaryReader& r) {
    PacketHeader h;
    h.magic       = r.ReadU32();
    h.version     = r.ReadU8();
    h.type        = r.ReadU8();
    h.sequenceNum = r.ReadU32();
    h.timestampUs = r.ReadU64();
    return h;
}

// Empty tag type purely to give Deserialize() an argument of a type related
// to T, so argument-dependent lookup (ADL) can find the app's overload.
// A function distinguished only by its return type isn't found via ADL.
template <typename T>
struct TypeTag {};

// ---------------------------------------------------------------------------
// Payload codec contract (documentation only — no base class required).
//
// Sender<T> and Receiver<T> call Serialize/Deserialize via argument-dependent
// lookup (ADL), so any payload type T is usable as long as the app defines,
// in T's own namespace:
//
//   void Serialize(LNet::BinaryWriter& w, const T& value);
//   T    Deserialize(LNet::BinaryReader& r, LNet::TypeTag<T>);
//
// The TypeTag<T> parameter is unused by the app's implementation — it exists
// only so ADL can locate the function from Receiver<T>'s call site. LNet
// never needs to see these definitions, only that they exist and are visible
// at the call site inside Sender.h/Receiver.h.
// ---------------------------------------------------------------------------

} // namespace LNet
