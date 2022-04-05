
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


} // namespace msc
