#ifndef COMPRESSOR_NAVE96_H
#define COMPRESSOR_NAVE96_H

/*
 * <header> ::= <patterns_num>{<pattern_info>}
 * <pattern_info> ::= <pattern_size><pattern><times_used>{<marker>}
 *
 * ----------------------------------------------------------------------------
 *
 * nave96 compresses information by blocks of 4096 bytes
 *
 * ----------------------------------------------------------------------------
 *
 * You can see how it actually works by looking in definition of
 * nave96::compress and nave96::CBlock::compress functions in nave96.cpp.
 *
 * Decompression logic is implemented in nave96::decompress function.
 *
 * */

// size of blocks
#define BK_SZ 4096

// bits dedicated to Mark pattern insertion positions
#define M_BITS 12

// TU — times used, how many times the pattern was used
#define TU_BITS 8
#define TU_MAX  257
#define TU_MIN  2

// PL — pattern length
// bits dedicated to represent the length of the pattern
#define PL_BITS 4
#define PL_MAX 17
#define PL_MIN 2

// NP - number of patterns
#define NP_BITS 8
#define NP_MAX 255

// Patterns buffer
#define PB_SZ BK_SZ*(PL_MAX-PL_MIN+1)

// Hashing number
#define HSN_N 12345


#include <vector>
#include <array>
#include <cmath>
#include <exception>
#include <unordered_map>

#include "Encoder.h"

class nave96 {

    struct CBlock { // Compression

        class Pattern {
        public:
            Pattern() = default;

            Pattern(int pos, int length, int timesUsed)
                    : pos_{pos}, l_{length}, t_{timesUsed} {};

            Pattern(Pattern &&);

            Pattern &operator=(Pattern that);

            Pattern(const Pattern &) = delete;

            ~Pattern() = default;

            int getEffect();

            void used() { t_ = (t_ < TU_MAX) ? t_ + 1 : t_; };

            void swap(Pattern &that);

            friend class CBlock;

        private:
            // times was used
            int t_; // 1-256

            // position in the block of data
            int pos_; // 0-4095

            // length of the pattern
            int l_; // 2-17

            // set in rmBadPatterns(), represented by maximum bits saved
            int effect_ = 0;

            //
            std::vector<int> markers_;

        }; // END OF PATTERN DECLARATION

        CBlock(char *data, unsigned sz, Encoder *);

        ~CBlock() = default;

        // origin information
        char *data_;        // | no destructor
        unsigned data_sz_;  // up to 4096 bytes

        // encoder
        Encoder *ec_ = nullptr;

        // Patterns
        std::array<Pattern, PB_SZ> pts_;
        int ptsI_ = 0;
        std::unordered_map<int, std::vector<int>> hashes_;

        // PATTERNS INTERFACE
        // find all patterns in data_
        void findAllPatterns();

        // returns index of the pattern or -1
        // int hasPattern(int pos, int len);
        int hasPattern(int pos, int len, int hash); // if case we have the hash
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
        bool takenBits[BK_SZ]{};

        // Searches for patterns, uses them in effective way and writes
        // compressed data to encoder
        void compress();

    }; // END OF CBLOCK DECLARATION

    struct DBlock { // Decompression

        struct Marker {
            int pos_;
            int pattern_;
        };

        DBlock() = default;

        DBlock(Encoder *ec, char *out)
                : ec_{ec}, ddata_{out} {};

        void decompress();

        void sortMarkers();

        // decompressed data
        char *ddata_;               // buffer | no destructor
        int ddI = 0;                // index

        // input (compressed) data
        Encoder *ec_ = nullptr;     // | no destructor

        std::vector<std::string> patterns_;


        std::vector<Marker> markers_; // buffer
        int mI = 0;                   // index

    };// END OF DBLOCK DECLARATION

public:
    Encoder *compress(char *in, unsigned sz);

    void compress(char *in, unsigned sz, Encoder *ec);

    // returns number of bytes decoded
    unsigned decompress(Encoder *in, char *out, int blockNumber = 0);

}; // END OF NAVE96 DECLARATION


#endif //COMPRESSOR_NAVE96_H
