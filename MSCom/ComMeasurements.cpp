
#include "ComMeasurements.h"

#include <iostream>

namespace msc {

    ComMeasurements::ComMeasurements(int flags_)
    :flags{flags_} {}

    void ComMeasurements::setFlags(int flags_) noexcept {
        flags = flags_;
    }

    void ComMeasurements::run() {
        if (flags & COM_MES_TIME)
            start = std::chrono::high_resolution_clock::now();
    }

    void ComMeasurements::stop() {
        if (flags & COM_MES_TIME)
            end = std::chrono::high_resolution_clock::now();
    }

    void ComMeasurements::show() const {
        if (flags & COM_MES_BYTES) {
            std::cout   << "\tInput size:\t\t"
                        << bytesIn / 1024 << "kb" << std::endl
                        << "\tOutput size:\t"
                        << bytesOut / 1024 << "kb" << std::endl
                        << "\tRatio out/in:\t"
                        << 100.0 * bytesOut / bytesIn << "%" << std::endl;
        }
        if (flags & COM_MES_TIME) {
            std::chrono::duration<float> duration{end - start};
            std::cout << "\tTime:\t\t\t"
                        << duration.count() << 's' << std::endl;
        }
    }

    void ComMeasurements::setBytesIn(uint32_t b) noexcept {
        bytesIn = b;
    }

    void ComMeasurements::setBytesOut(uint32_t b) noexcept {
        bytesOut = b;
    }
}