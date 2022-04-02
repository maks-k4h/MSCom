//
// Created by Maks Konevych on 20.02.2022.
//

#include "nave96.h"

namespace msc {

    Encoder *nave96::compress(char *in, unsigned int sz) {
        auto ec = new Encoder;
        compress(in, sz, ec);
        return ec;
    }

    void nave96::compress(char *in, unsigned int sz, Encoder *ec) {
        if (!in || !sz) return;

        for (int bn = 0; bn * BK_SZ < sz; ++bn) {
            unsigned bsz = std::min(unsigned(BK_SZ), sz - bn * BK_SZ);
            auto dp = in + bn * BK_SZ;
            CBlock bl(dp, bsz, ec);
            bl.compress();
        }
    }

    nave96::CBlock::CBlock(char *data, unsigned int sz, Encoder *ec) {
        if (!data) throw std::logic_error("Block cannot read not-existing data!");
        if (!sz) throw std::range_error("Block cannot have size of 0!");

        hashes_.reserve(PB_SZ);
        data_ = data;
        data_sz_ = sz;
        ec_ = ec;
    }

/*
nave96::CBlock::Pattern &nave96::CBlock::Pattern::operator=
        (nave96::CBlock::Pattern that) {
    this->swap(that);
    return *this;
}
*/

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

    int nave96::CBlock::Pattern::getEffect() {
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
            for (int length = 2; length <= PL_MAX && i + length <= data_sz_; ++length) {
                hash = hash * HSN_N + data_[i + length - 1] * length;
                int p = hasPattern(i, length, hash);
                if (p == -1) {
                    addPattern(i, length, hash);
                    pattNextApp[ptsI_] = i + length;
                } else {
                    if (i < pattNextApp[p]) continue;
                    pattNextApp[p] = i + length;
                    pts_[p].used();
                }
            }
        }
    }

    int nave96::CBlock::hasPattern(int pos, int len, int hash) {
        if (pos + len > data_sz_ || pos < 0 || len < 0) {
            throw std::overflow_error("Search parameters are incorrect!");
        }

        if (hashes_.find(hash) == hashes_.end()) return -1;

        for (int i = 0; i < hashes_[hash].size(); ++i) {
            auto pn = hashes_[hash][i];
            if (len != pts_[pn].l_) continue;
            for (int j = 0; j < len; ++j) {
                if (data_[pos + j] != data_[pts_[pn].pos_ + j]) break;
                if (j == len - 1) return pn;
            }
        }

        return -1;
    }

    void nave96::CBlock::addPattern(int pos, int len, int hash) {
        /*if(hasPattern(pos, len, hash)!=-1) {
            throw std::logic_error("Pattern was already added!");
        }*/
        if (pos + len > data_sz_) {
            throw std::overflow_error("Pattern of such length cannot be added!");
        }
        hashes_[hash].push_back(ptsI_);
        pts_[ptsI_].pos_ = pos;
        pts_[ptsI_].l_ = len;
        pts_[ptsI_].t_ = 1;
        ++ptsI_;
    }

// moves ineffective patterns to the end of the buffer, updates ptsI_
    void nave96::CBlock::rmBadPatterns() {
        int slow = 0;
        for (int i = 0; i < ptsI_; ++i) {
            if (pts_[i].getEffect() > 0) {
                pts_[slow] = std::move(pts_[i]);
                ++slow;
            }
        }
        ptsI_ = slow;
    }

    void nave96::CBlock::sortPatterns() {
        std::sort(pts_.begin(),
                  pts_.begin() + ptsI_,
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

            if (pts_[p].getEffect() > 0) {
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
        ec_->putint(int(ptsI_), NP_BITS);

        for (int i = 0; i < ptsI_; ++i) {

            // <pattern_size>
            if (pts_[i].l_ > PL_MAX) {
                throw std::range_error("Pattern length cannot be grater than "
                                       + std::to_string(PL_MAX) + '!');
            }
            if (PL_MAX - PL_MIN + 1 > std::pow(2, PL_BITS)) {
                throw std::range_error("Pattern Length Range Error!");
            }

            ec_->putint(pts_[i].l_ - PL_MIN, PL_BITS);

            // <pattern>
            for (int j = 0; j < pts_[i].l_; ++j) {
                ec_->putc(data_[pts_[i].pos_ + j]);
            }

            // <times_used>
            if (pts_[i].t_ > TU_MAX || pts_[i].t_ < TU_MIN) {
                throw std::range_error("Pattern was used "
                                       + std::to_string(pts_[i].t_)
                                       + " times while range is ["
                                       + std::to_string(TU_MIN)
                                       + "," + std::to_string(TU_MAX) + "]!");
            }
            ec_->putint(pts_[i].t_ - TU_MIN, TU_BITS);

            // <marker>
            if (pts_[i].t_ != pts_[i].markers_.size()) {
                throw std::logic_error("TU != markers.size in pattern #"
                                       + std::to_string(i) + '!');
            }
            for (int j = 0; j < pts_[i].t_; ++j) {
                ec_->putint(pts_[i].markers_[j], M_BITS);
            }

        }

        // BODY PART
        for (int i = 0; i < data_sz_; ++i) {
            if (!takenBits[i]) {
                ec_->putc(data_[i]);
            }
        }

        // IMPORTANT !!!
        ec_->roundBitCount();
    }

// DECOMPRESSION ---------------------------------------------------------------

    unsigned nave96::decompress(Encoder *in, char *out, int bn) {
        if (!bn) bn = INT32_MAX;
        unsigned dsize = 0;

        for (int b = 0; b < bn; ++b) {
            DBlock block(in, out + BK_SZ * b);
            block.decompress();
            if (!block.ddI) break;
            dsize += block.ddI;
        }

        return dsize;
    }


    void nave96::DBlock::decompress() {
        // HEADER DECODING

        int pnum; // number of patterns
        if (!ec_->getint(pnum, NP_BITS)) return;

        // Decrypting patterns
        for (int i = 0; i < pnum; ++i) {
            int plength;
            ec_->getint(plength, PL_BITS);
            plength += PL_MIN;

            // reading pattern
            patterns_.emplace_back("");
            for (int j = 0, ch; j < plength; ++j) {
                if (!ec_->getint(ch, 8))
                    throw std::length_error(
                            "Pattern information cannot be decoded!");
                patterns_[i] += ch;
            }


            int mnum; // number of pattern's markers
            ec_->getint(mnum, TU_BITS);
            mnum += TU_MIN;
            for (int j = 0, m; j < mnum; ++j) {
                ec_->getint(m, M_BITS);
                markers_.push_back({m, i});
            }
        }
        sortMarkers();


        // BODY DECODING
        // using markers
        for (int m = 0; m < markers_.size(); ++m) {
            while (ddI < markers_[m].pos_) {
                if (!ec_->getc(ddata_[ddI])) {
                    throw std::length_error("Block body cannot be decoded!");
                }
                ++ddI;
            }
            for (int pi = 0; pi < patterns_[markers_[m].pattern_].size(); ++pi) {
                ddata_[ddI] = patterns_[markers_[m].pattern_][pi];
                ++ddI;
            }
        }

        // reading rest of the body
        char c;
        while (ddI < BK_SZ && ec_->getc(c)) ddata_[ddI++] = c;


        // IMPORTANT, NOT roundBitCount!!! | spent half a day.....
        ec_->roundOutBitCount();
    }

    void nave96::DBlock::sortMarkers() {
        bool changes = true;
        while (changes) {
            changes = false;
            for (int i = 0; i < int(markers_.size()) - 1; ++i) {
                if (markers_[i].pos_ > markers_[i + 1].pos_) {
                    auto temp = markers_[i];
                    markers_[i] = markers_[i + 1];
                    markers_[i + 1] = temp;
                    changes = true;
                }
            }
        }
    }

} // namespace msc

