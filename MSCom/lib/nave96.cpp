//
// Created by Maks Konevych on 20.02.2022.
//

#include "nave96.h"

#include <iostream>

namespace msc {

    void nave96::compress(char *in, unsigned int sz, Encoder &out) {
        if (!in || !sz) return;

        for (int bn = 0; bn * BK_SZ < sz; ++bn) {
            unsigned bsz = std::min(unsigned(BK_SZ), sz - bn * BK_SZ);
            auto dp = in + bn * BK_SZ;
            CBlock bl(dp, bsz, out);
            bl.compress();
        }
    }

    nave96::CBlock::CBlock(char *data, unsigned int sz, Encoder &ec)
    :ec_{ec} {
        if (!data) throw std::logic_error("Block cannot read not-existing data!");
        if (!sz) throw std::range_error("Block cannot have size of 0!");

        hashes_.reserve(PB_SZ);
        data_ = data;
        data_sz_ = sz;
        ec_ = ec;
    }


    void nave96::CBlock::Pattern::swap(nave96::CBlock::Pattern &that) {
        std::swap(this->t_, that.t_);
        std::swap(this->l_, that.l_);
        std::swap(this->markers_, that.markers_);
        std::swap(this->effect_, that.effect_);
        std::swap(this->pos_, that.pos_);
    }

    nave96::CBlock::Pattern::Pattern(nave96::CBlock::Pattern &&that) noexcept
            : Pattern() {
        this->swap(that);
    }

    inline int nave96::CBlock::Pattern::getEfficiency() {
        return effect_ = ((t_ - 1) * l_ * 8) - (PL_BITS + TU_BITS + t_ * M_BITS);
    }

    nave96::CBlock::Pattern &nave96::CBlock::Pattern::operator=(Pattern &&that)
    noexcept {
        swap(that);
        return *this;
    }

// COMPRESSION -----------------------------------------------------------------

    void nave96::CBlock::compress() {

        findAllPatterns();
        rmBadPatterns();
        sortPatterns();
        makeMarkers();
        encode();

    }

    void nave96::CBlock::findAllPatterns() {
        int pattNextApp[PB_SZ];
        for (int i = 0; i < data_sz_ - 1; ++i) {
            int hash = data_[i];
            for (int length = 2; length <= PL_MAX &&
                                    i + length <= data_sz_; ++length) {
                hash = hash * HSN_N + data_[i + length - 1] + length * length;
                int p = hasPattern(i, length, hash);
                if (p == -1) {
                    addPattern(i, length, hash);
                    pattNextApp[ptsI_] = i + length;
                } else {
                    if (i < pattNextApp[p])
                        continue;
                    pattNextApp[p] = i + length;
                    pts_[p].used();
                }
            }
        }
    }

    inline int nave96::CBlock::hasPattern(int pos, int len, int hash) {
        if (hashes_.find(hash) == hashes_.end())
            return -1;

        for (int pn : hashes_[hash]) {
            if (len != pts_[pn].l_){
                continue;
            }
            if (memcmp(&data_[pos], &data_[pts_[pn].pos_], len) == 0) {
                return pn;
            }
        }
        return -1;
    }

    inline void nave96::CBlock::addPattern(int pos, int len, int hash) {
        hashes_[hash].emplace_back(ptsI_);
        pts_[ptsI_].pos_ = pos;
        pts_[ptsI_].l_ = len;
        pts_[ptsI_].t_ = 1;
        ++ptsI_;
    }

// moves ineffective patterns to the end of the buffer, updates ptsI_
    void nave96::CBlock::rmBadPatterns() {
        int slow = 0;
        for (int i = 0; i < ptsI_; ++i) {
            if (pts_[i].getEfficiency() > 0) {
                pts_[slow] = std::move(pts_[i]);
                ++slow;
            }
        }
        ptsI_ = slow;
    }

    void nave96::CBlock::sortPatterns() {
        std::sort(pts_,
                  pts_ + ptsI_,
                  [](const Pattern &v1, const Pattern &v2) -> bool {
                      return v1.effect_ > v2.effect_;
                  });
    }


    /*
     * makeMarkers goes through all the patterns from the most effective
     * to the less effective using those patterns whenever it can
     * */
    void nave96::CBlock::makeMarkers() {
        int slow = 0;

        for (int p = 0; p < ptsI_ && p < NP_MAX; ++p) { // pattern

            // making markers
            for (int pos = 0; pos < data_sz_
                              && pts_[p].markers_.size() < TU_MAX;) {
                if (canApplyPattern(p, pos)) {
                    pts_[p].markers_.push_back(pos);
                    pos += pts_[p].l_;
                } else {
                    pos++;
                }
            }

            pts_[p].t_ = (int) pts_[p].markers_.size();


            // evaluating the effectiveness

            if (pts_[p].getEfficiency() > 0) {
                for (int m = 0; m < pts_[p].markers_.size(); ++m) {
                    for (int i = 0; i < pts_[p].l_; ++i) {
                        takenBits[pts_[p].markers_[m] + i] = true;
                    }
                }

                pts_[slow] = std::move(pts_[p]);
                ++slow;
            }
        }
        ptsI_ = slow;
    }

    bool nave96::CBlock::canApplyPattern(int p, int pos) {
        for (int i = 0; i < pts_[p].l_; ++i) {
            if (pos + i >= data_sz_ ||
                takenBits[pos + i] ||
                data_[pos + i] != data_[pts_[p].pos_ + i])
                return false;
        }
        return true;
    }

    void nave96::CBlock::encode() {
        // HEADER PART
        // <patterns_num>
        ec_.put<NP_BITS>(ptsI_);

        for (int i = 0; i < ptsI_; ++i) {

            // <pattern_size>
            if (pts_[i].l_ > PL_MAX) {
                throw std::range_error("Pattern length cannot be grater than "
                                       + std::to_string(PL_MAX) + '!');
            }
            if (PL_MAX - PL_MIN + 1 > std::pow(2, PL_BITS)) {
                throw std::range_error("Pattern Length Range Error!");
            }

            ec_.put<PL_BITS>(pts_[i].l_ - PL_MIN);

            // <pattern>
            for (int j = 0; j < pts_[i].l_; ++j) {
                ec_.put<8>(data_[pts_[i].pos_ + j]);
            }

            // <times_used>
            if (pts_[i].t_ > TU_MAX || pts_[i].t_ < TU_MIN) {
                throw std::range_error("Pattern was used "
                                       + std::to_string(pts_[i].t_)
                                       + " times while range is ["
                                       + std::to_string(TU_MIN)
                                       + "," + std::to_string(TU_MAX) + "]!");
            }
            ec_.put<TU_BITS>(pts_[i].t_ - TU_MIN);

            // <marker>
            if (pts_[i].t_ != pts_[i].markers_.size()) {
                throw std::logic_error("TU != markers.size in pattern #"
                                       + std::to_string(i) + '!');
            }
            for (int j = 0; j < pts_[i].t_; ++j) {
                ec_.put<M_BITS>(pts_[i].markers_[j]);
            }

        }

        // BODY PART
        for (int i = 0; i < data_sz_; ++i) {
            if (!takenBits[i]) {
                ec_.put<8>(data_[i]);
            }
        }

        // IMPORTANT !!!
        ec_.roundBitCount();
    }

// DECOMPRESSION ---------------------------------------------------------------

    bool nave96::decompress(Decoder &in, Encoder &out, int blocks) {
        if (blocks < 1)
            blocks = INT32_MAX;

        for (int blocksDone {0};
                blocksDone < blocks && in.bitsInQueue() > 0;
                ++blocksDone)
        {
            DBlock block(in, out);
            block.decompress();
        }

        return true;
    }

    nave96::DBlock::DBlock(Decoder &in, Encoder &out)
    : dc{in}, ec{out}, blockBeginBit{out.bitsDone()} { }

    void nave96::DBlock::decompress() {
        // HEADER DECODING

        int pnum; // number of patterns
        if (!dc.get<NP_BITS>(pnum)) return;

        // Decrypting patterns
        for (int i = 0; i < pnum; ++i) {
            int plength;
            dc.get<PL_BITS>(plength);
            plength += PL_MIN;

            // reading pattern
            patterns_.emplace_back("");
            for (int j = 0, ch; j < plength; ++j) {
                if (!dc.get<8>(ch))
                    throw std::length_error(
                            "Pattern information cannot be decoded!");
                patterns_[i] += ch;
            }


            int mnum; // number of pattern's markers
            dc.get<TU_BITS>(mnum);
            mnum += TU_MIN;
            for (int j = 0, m; j < mnum; ++j) {
                dc.get<M_BITS>(m);
                markers_.push_back({m, i});
            }
        }
        sortMarkers();


        // BODY DECODING
        // using markers
        for (int m = 0; m < markers_.size(); ++m) {
            uint8_t temp;
            while (getDecompressedBitsNumber() / 8 < markers_[m].pos_) {
                if (!dc.get<8>(temp)) {
                    throw std::length_error("Block body cannot be decoded!");
                }
                ec.put<8>(temp);
            }
            for (int pi = 0; pi < patterns_[markers_[m].pattern_].size(); ++pi) {
                ec.put<8>(patterns_[markers_[m].pattern_][pi]);
            }
        }

        // reading rest of the body
        char c;
        while (getDecompressedBitsNumber() / 8 < BK_SZ && dc.get<8>(c))
            ec.put<8>(c);

        dc.roundBitCount();
    }

    void nave96::DBlock::sortMarkers() {
        std::sort(markers_.begin(),
                  markers_.end(),
                  [](const Marker &v1, const Marker &v2) -> bool {
            return v1.pos_ < v2.pos_;
        });
    }

    uint64_t nave96::DBlock::getDecompressedBitsNumber() const noexcept {
        return ec.bitsDone() - blockBeginBit;
    }

} // namespace msc

