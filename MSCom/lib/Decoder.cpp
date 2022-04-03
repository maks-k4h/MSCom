
#include "Decoder.h"

#include <iostream>
namespace msc {

    Decoder::Decoder(const uint8_t *data_, size_t size)
            : data{data_} {
        if (data)
            dSizeBits = size * 8;
    }

    uint64_t Decoder::bitsDone() const noexcept {
        return bitc;
    }

    uint64_t Decoder::bitsInQueue() const noexcept {
        return dSizeBits - bitc;
    }

    void Decoder::roundBitCount() noexcept {
        while (bitc % 8 != 0)
            ++bitc;
    }

    void Decoder::resetBitCount() noexcept {
        bitc = 0;
    }

    const uint8_t *Decoder::getData() const noexcept {
        return data;
    }

    bool Decoder::hasData() const noexcept {
        return static_cast<bool>(data);
    }

    void Decoder::setData(uint8_t *data_, size_t sz) noexcept {
        data = data_;
        bitc = 0;
        if (data)
            dSizeBits = sz * 8;
    }

    bool Decoder::getBit() {
        if (bitc >= dSizeBits)
            throw std::out_of_range(
                    "Decoder::getBit : "
                    "no bits left."
            );

        //            | byte |             | bit |
        bool r = data[bitc / 8] & (0x80 >> bitc % 8);
        ++bitc;
        return r;
    }

} // namespace msc
