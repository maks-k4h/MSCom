
#ifndef MSCOM_COMMEASUREMENTS_H_
#define MSCOM_COMMEASUREMENTS_H_

// Facility to measure compression/decompression rates:
//      — Time
//      — Bytes in/out
// Shows information to std::cout

#include <chrono>

#define COM_MES_TIME    0x1
#define COM_MES_BYTES   0x2

namespace msc {

    class ComMeasurements {
    public:
        ComMeasurements() = default;
        explicit ComMeasurements(int flags);

        ~ComMeasurements() = default;

        void setFlags(int flags) noexcept;
        void run();
        void stop();

        void setBytesIn(uint32_t) noexcept;
        void setBytesOut(uint32_t) noexcept;

        void show() const;

    private:
        int flags {0};

        std::chrono::time_point<std::chrono::steady_clock> start, end;

        uint32_t bytesIn {0};
        uint32_t bytesOut {0};

    };

}


#endif //MSCOM_COMMEASUREMENTS_H_
