
#include "Decoder.h"

namespace msc {

    Decoder::Decoder(const uint8_t *data_, size_t sz)
    : data{data_} {
        if (data)
            dSizeBits = sz * 8;
    }

    void Decoder::setData(const uint8_t *data_, size_t sz) noexcept {
        data = data_;
        bitc = 0;
        if (data)
            dSizeBits = sz * 8;
    }

    const uint8_t *Decoder::getData() const noexcept {
        return data;
    }

    bool Decoder::hasData() const noexcept {
        return static_cast<bool>(data);
    }

    // TODO : remove out_of_range throw (with time)
    // out_of_range error on fail, must never emerge
    bool Decoder::getBit() {
        if (bitc >= dSizeBits)
            throw std::out_of_range("Decoder::getBit : no bits left.");

        //            | byte |             | bit |
        bool r = data[bitc / 8] & (0x80 >> bitc % 8);
        ++bitc;
        return r;
    }


} // namespace msc
