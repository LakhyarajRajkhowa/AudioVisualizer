#pragma once

#include <cstdint>
#include <cstring>
#include <vector>

namespace LNet {

// Generic append-only byte buffer builder. Has no knowledge of any specific
// packet or payload type — apps write their own Serialize(BinaryWriter&, T)
// free functions using these primitives (see Codec.h).
class BinaryWriter {
public:
    void WriteU8(uint8_t v)   { Append(&v, sizeof(v)); }
    void WriteU16(uint16_t v) { Append(&v, sizeof(v)); }
    void WriteU32(uint32_t v) { Append(&v, sizeof(v)); }
    void WriteU64(uint64_t v) { Append(&v, sizeof(v)); }
    void WriteI32(int32_t v)  { Append(&v, sizeof(v)); }
    void WriteFloat(float v)  { Append(&v, sizeof(v)); }

    // Length-prefixed array of trivially-copyable elements (e.g. float
    // spectrum data). Writes a uint32_t count followed by raw element bytes.
    template <typename T>
    void WriteArray(const std::vector<T>& arr) {
        static_assert(std::is_trivially_copyable<T>::value,
                      "WriteArray requires a trivially copyable element type");
        WriteU32(static_cast<uint32_t>(arr.size()));
        if (!arr.empty())
            Append(arr.data(), arr.size() * sizeof(T));
    }

    // Raw byte blob, length-prefixed. Useful for opaque/variable payloads.
    void WriteBytes(const uint8_t* data, size_t size) {
        WriteU32(static_cast<uint32_t>(size));
        if (size > 0) Append(data, size);
    }

    const std::vector<uint8_t>& Data() const { return buffer; }

private:
    void Append(const void* src, size_t size) {
        const uint8_t* bytes = static_cast<const uint8_t*>(src);
        buffer.insert(buffer.end(), bytes, bytes + size);
    }
    std::vector<uint8_t> buffer;
};

} // namespace LNet
