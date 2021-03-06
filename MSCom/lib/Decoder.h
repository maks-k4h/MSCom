
#ifndef MSCOM_LIB_DECODER_H_
#define MSCOM_LIB_DECODER_H_

/*
 * Decoder.
 * Description:
 * — Objects of this class do not own data.
 * — They provide tools to read existing data.
 * — Namely, those tools allow you to read it by arbitrary amounts of bits
 *      into arbitrary integral data type.
 * — Data can only be read, not written (see Encoder class to write by bits).
 * — Instances can be copied in constant time. Bit-counter value is copied too.
 * — Bit-counters can be reset to reread data.
 * — Bit-counters can be rounded to start reading from the first bit in a byte.
 *      For instance, you read some bits and now must be sure
 *      that you continue reading from a start of some byte: 21 bits are done,
 *      now next byte must be processed —> call to round —> 3 bits are skipped,
 *      bit-counter is 24, i.e., 3 bytes of actual data are done, you continue
 *      reading from new byte, that is 4th. If bit-count is 24
 *      it will remain so.
 * — Methods used to read data are generated by templates. Those take number
 *      of bits to read as a template parameter. Compile-time check on these
 *      parameters is performed. Number of bits to read cannot be bigger than
 *      the number of bits of a type of variable that will store reading result.
 * */

#include "BitView.h"

namespace msc {

    class Decoder : public BitView {
    public:
        Decoder() = default;
        Decoder(const Decoder&) = default;
        Decoder(const uint8_t *, size_t);
        Decoder &operator=(const Decoder &) = default;
        ~Decoder() override = default;

        void setData(const uint8_t *, size_t) noexcept;
        const uint8_t *getData() const noexcept;
        bool hasData() const noexcept;

        template<size_t Bits, std::integral I>
        bool get(I &val);

    private:
        const uint8_t *data {nullptr};
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
            //      |  byte  |           |  bit  |
            if (data[bitc / 8] & (0x80 >> bitc % 8))
                val |= 1 << bits;
            ++bitc;
        }
        return true;
    }


} // namespace msc

#endif // MSCOM_LIB_DECODER_H_
