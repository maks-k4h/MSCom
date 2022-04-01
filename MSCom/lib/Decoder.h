
#ifndef MSCOM_LIB_DECODER_H_
#define MSCOM_LIB_DECODER_H_

/*
 * Decoder.
 * Description:
 * — Objects of this class do not own data.
 * — They provide tools to read existing data.
 * — Namely, those tools allow you to read it by arbitrary amounts of bits
 *      into arbitrary integral data type.
 * — Data can only be read, not written.
 * — Instances can be copied in constant time. Bit-counter value is copied too.
 * — Bit-counters can be reset to reread data.
 * — Bit-counters can be rounded to start reading from the first bit in a byte.
 *      For instance, you read some bits and now must be sure
 *      that you continue reading from a start of some byte: 21 bits are done,
 *      now next byte must be processed —> call to round —> 3 bits are skipped,
 *      bit-counter is 24, i.e., 3 bytes of actual data are done, you continue
 *      reading from new byte, that is 4th. If bit-count is 24
 *      it will remain so.
 * */

#include <cstdlib>
#include <concepts>
#include <stdexcept>

namespace msc {

    class Decoder {
    public:
        Decoder() = default;

        Decoder(const Decoder &) = default;

        Decoder(const uint8_t *, size_t);

        Decoder &operator=(const Decoder &) = default;

        ~Decoder() = default;

        uint64_t bitsDone() const noexcept;

        uint64_t bitsInQueue() const noexcept;

        void roundBitCount() noexcept;

        void resetBitCount() noexcept;

        const uint8_t *getData() const noexcept;

        bool hasData() const noexcept;

        void setData(uint8_t *, size_t) noexcept;

        template<size_t Bits, std::integral I>
        bool get(I &val);

    private:
        bool getBit();

        const uint8_t *data{nullptr};
        uint64_t bitc{0};
        uint64_t dSizeBits{0};
    };


    // Templates definitions

    template<size_t Bits, std::integral I>
    bool Decoder::get(I &val) {
        static_assert(Bits <= sizeof(I) * 8,
                      "Decoder::get() : "
                      "try to put a number of bits that the type cannot hold."
        );
        if (bitc + Bits > dSizeBits)
            return false;

        val = 0;
        int bits = static_cast<int>(Bits);
        while (0 < bits--) {
            if (!getBit()) continue;
            val |= 1 << bits;
        }
        return true;
    }

} // namespace msc

#endif // MSCOM_LIB_DECODER_H_
