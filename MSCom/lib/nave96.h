#ifndef MSCOM_LIB_NAVE96_H_
#define MSCOM_LIB_NAVE96_H_

// size of blocks
constexpr int BK_SZ     = 4096;

// bits dedicated to Mark pattern insertion positions
constexpr int M_BITS    = 12;

// TU — times used, how many times the pattern was used
constexpr int TU_BITS   = 8;
constexpr int TU_MAX    = 257;
constexpr int TU_MIN    = 2;

// PL — pattern length
// bits dedicated to represent the length of the pattern
constexpr int PL_BITS   = 4;
constexpr int PL_MAX    = 17;
constexpr int PL_MIN    = 2;

// NP - number of patterns
constexpr int NP_BITS   = 8;
constexpr int NP_MAX    = 255;

// Patterns buffer
constexpr int PB_SZ     = BK_SZ*(PL_MAX-PL_MIN+1);

// Hashing number
constexpr int HSN_N     = 123456841;


#include <vector>
#include <array>
#include <cmath>
#include <exception>
#include <unordered_map>

#include "EncoderOld.h"
#include "Encoder.h"
#include "Decoder.h"

namespace msc {

    class nave96 {
    public:
        void compress(char *in, unsigned int sz, Encoder &out);
        bool decompress(Decoder &in, Encoder &out, int blocks = 0);

    private:
        struct CBlock { // Compression

            class Pattern {
            public:
                Pattern() = default;
                Pattern(int pos, int length, int timesUsed)
                        : pos_{pos}, l_{length}, t_{timesUsed} {};

                Pattern(const Pattern &) = delete;
                Pattern(Pattern &&) noexcept;

                Pattern &operator=(Pattern &&) noexcept;
                Pattern &operator=(const Pattern &) = delete;

                ~Pattern() = default;

                void swap(Pattern &that);
                int getEfficiency();

                void used() { t_ = (t_ < TU_MAX) ? t_ + 1 : t_; };

                friend class CBlock;

            private:
                // times was used
                int t_; // 1-256

                // position in the block of data
                int pos_; // 0-4095

                // length of the pattern
                int l_; // 2-17

                int effect_ {0};
                std::vector<int> markers_;

            }; // END OF PATTERN DECLARATION

            CBlock(char *data, unsigned sz, Encoder &);

            ~CBlock() = default;

            // origin information
            char *data_;
            unsigned data_sz_;  // up to 4096 bytes

            // encoder
            Encoder &ec_;

            // Patterns
            Pattern pts_[PB_SZ];
            int ptsI_ {0};
            std::unordered_map<int, std::vector<int>> hashes_;

            // PATTERNS INTERFACE
            // find all patterns in data_
            void findAllPatterns();

            // returns index of the pattern or -1
            int hasPattern(int pos, int len, int hash);
            // adds pattern
            void addPattern(int pos, int len, int hash);

            // Remove patterns that take more memory than save
            void rmBadPatterns();

            // Sorting patterns in decreasing of effectivity way
            void sortPatterns();

            // APPLYING PATTERNS AND ENCODING
            void makeMarkers();

            bool canApplyPattern(int pNumber, int pos);

            // encodes header and rest of data
            void encode();

            // 0 if byte haven't been replaced yet, 1 otherwise
            bool takenBits[BK_SZ] {};

            // Searches for patterns, uses them in effective way and writes
            // compressed data to encoder
            void compress();

        }; // struct CBLOCK

        struct DBlock { // Decompression

            struct Marker {
                int pos_;
                int pattern_;
            };

            DBlock(Decoder &in, Encoder &out);

            void decompress();
            void sortMarkers();
            uint64_t getDecompressedBitsNumber() const noexcept;

            Encoder &ec;
            Decoder &dc;
            uint64_t blockBeginBit;

            std::vector<std::string> patterns_;

            std::vector<Marker> markers_; // buffer
            int mI = 0;                   // index

        };// struct DBLOCK

    }; // class NAVE96

} // namespace msc

#endif // MSCOM_LIB_NAVE96_H_
