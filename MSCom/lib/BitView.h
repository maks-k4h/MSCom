
#ifndef MSCOM_LIB_BITVIEW_H_
#define MSCOM_LIB_BITVIEW_H_


#include <cstdlib>
#include <concepts>
#include <stdexcept>

namespace msc {

    class BitView {
    public:
        BitView()                               = default;
        BitView(const BitView &)                = default;
        BitView &operator=(const BitView &)     = default;
        virtual ~BitView()                      = 0;

        uint64_t bitsDone() const noexcept;
        uint64_t bitsDataSize() const noexcept;
        uint64_t bitsInQueue() const noexcept;
        void roundBitCount() noexcept;
        void resetBitCount() noexcept;

    protected:

        uint64_t bitc {0};
        uint64_t dSizeBits {0};
    };

} // namespace msc

#endif //MSCOM_LIB_BITVIEW_H_
