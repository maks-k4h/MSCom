
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


} // namespace msc
