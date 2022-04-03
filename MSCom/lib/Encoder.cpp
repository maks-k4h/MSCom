
#include "Encoder.h"

namespace msc {

    Encoder::Encoder(uint8_t *data_, size_t sz)
    : data{data_} {
        if (data)
            dSizeBits = sz * 8;
    }

    void Encoder::setData(uint8_t *data_, size_t sz) noexcept {
        data = data_;
        bitc = 0;
        if (data)
            dSizeBits = sz * 8;
    }

    uint8_t *Encoder::getData() const noexcept {
        return data;
    }

    bool Encoder::hasData() const noexcept {
        return static_cast<bool>(data);
    }

    // TODO : remove out_of_range throw (with time)
    // out_of_range error on fail, must never emerge
    void Encoder::putBit(bool b) {
        if (bitc >= dSizeBits)
            throw std::out_of_range("Decoder::getBit : no bits left.");

        //          |  byte  |               |  bit  |
        if (b)  data[bitc / 8] |=  (0x80 >> (bitc % 8));
        else    data[bitc / 8] &= ~(0x80 >> (bitc % 8));
        ++bitc;
    }


} // namespace msc
