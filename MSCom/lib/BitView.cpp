//
// Created by Maks Konevych on 03.04.2022.
//

#include "BitView.h"

namespace msc {

    BitView::~BitView() = default;

    uint64_t BitView::bitsDone() const noexcept {
        return bitc;
    }

    uint64_t BitView::bitsDataSize() const noexcept {
        return dSizeBits;
    }

    uint64_t BitView::bitsInQueue() const noexcept {
        return dSizeBits - bitc;
    }

    void BitView::roundBitCount() noexcept {
        while (bitc % 8 != 0)
            ++bitc;
    }

    void BitView::resetBitCount() noexcept {
        bitc = 0;
    }



} // namespace msc