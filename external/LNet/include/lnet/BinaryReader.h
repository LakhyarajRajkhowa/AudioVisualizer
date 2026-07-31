#pragma once

#include <cstdint>
#include <cstring>
#include <stdexcept>
#include <type_traits>
#include <vector>

namespace LNet {

// Generic cursor over a received byte buffer. Has no knowledge of any
// specific packet or payload type — apps write their own
// Deserialize(BinaryReader&) -> T free functions using these primitives
// (see Codec.h). Throws std::runtime_error on buffer underrun so a
// malformed/truncated datagram fails loudly instead of reading garbage.
class BinaryReader {
public:
    BinaryReader(const uint8_t* data, size_t size) : ptr(data), remaining(size) {}

    uint8_t  ReadU8()   { return ReadRaw<uint8_t>(); }
    uint16_t ReadU16()  { return ReadRaw<uint16_t>(); }
    uint32_t ReadU32()  { return ReadRaw<uint32_t>(); }
    uint64_t ReadU64()  { return ReadRaw<uint64_t>(); }
    int32_t  ReadI32()  { return ReadRaw<int32_t>(); }
    float    ReadFloat(){ return ReadRaw<float>(); }

    template <typename T>
    std::vector<T> ReadArray() {
        static_assert(std::is_trivially_copyable<T>::value,
                      "ReadArray requires a trivially copyable element type");
        uint32_t count = ReadU32();
        std::vector<T> result(count);
        if (count > 0) {
            size_t bytes = count * sizeof(T);
            if (bytes > remaining) throw std::runtime_error("BinaryReader: buffer underrun");
            std::memcpy(result.data(), ptr, bytes);
            ptr += bytes;
            remaining -= bytes;
        }
        return result;
    }

    std::vector<uint8_t> ReadBytes() {
        return ReadArray<uint8_t>();
    }

    size_t Remaining() const { return remaining; }

private:
    template <typename T>
    T ReadRaw() {
        if (sizeof(T) > remaining) throw std::runtime_error("BinaryReader: buffer underrun");
        T value;
        std::memcpy(&value, ptr, sizeof(T));
        ptr += sizeof(T);
        remaining -= sizeof(T);
        return value;
    }

    const uint8_t* ptr;
    size_t remaining;
};

} // namespace LNet
