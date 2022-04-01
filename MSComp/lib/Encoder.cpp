//
// Created by Maks Konevych on 25.02.2022.
//

#include "Encoder.h"

bool Encoder::getbit(bool &b) {
    if (buff_.size() * 8 <= bitOutCount_) return false;

    auto byte = bitOutCount_ / 8;
    auto bit = bitOutCount_ % 8;

    b = buff_[byte] & (0x80 >> bit);
    ++bitOutCount_;
    return true;
}

bool Encoder::getc(char &ch) {
    if (bitCount_ < bitOutCount_ + 8) return false;

    ch = 0;
    for (int i = 0; i < 8; ++i) {
        bool bit;
        getbit(bit);
        if (!bit) continue;
        ch |= 0x80 >> i;
    }

    return true;
}


void Encoder::putc(char ch, unsigned bits) {
    if (bits > 8) {
        std::string message = "ENCODER: Cannot take "
                              + std::to_string(bits) + " bits"
                                                       " while only 8 are given!";
        throw std::overflow_error(message);
    }

    for (int i = 8 - bits; i < 8; ++i) {
        putbit(bool(ch & (0x80 >> i)));
    }
}

void Encoder::putint(int val, unsigned bits) {
    int is = sizeof(val) * 8;
    if (bits > is) {
        std::string message = "ENCODER: Cannot take "
                              + std::to_string(bits) + " bits"
                                                       " while only 8 are given!";
        throw std::overflow_error(message);
    }

    for (int i = is - bits; i < is; ++i) {
        putbit(bool(val & (0x80000000 >> i)));
    }
}

void Encoder::putbit(bool b) {
    if (buff_.empty()) buff_.emplace_back(0);
    auto byte = bitCount_ / 8;
    auto bit = bitCount_ % 8;
    while (byte >= buff_.size()) buff_.push_back(0);

    if (b) {
        char mask = 0x80 >> bit;
        buff_[byte] = buff_[byte] | mask;
    } else {
        char mask = 0x80 >> bit;
        buff_[byte] &= ~mask;
    }
    ++bitCount_;
}

void Encoder::reset(bool resBuff) {
    if (resBuff) buff_.erase(buff_.begin(), buff_.end());
    bitOutCount_ = 0;
    bitCount_ = 0;
}

bool Encoder::getc(char &ch, unsigned int bits) {
    if (bitCount_ < bitOutCount_ + bits || bits == 0 || bits > 8) return false;
    ch = 0;
    for (int i = 8 - bits; i < 8; ++i) {
        bool bit;
        getbit(bit);
        if (!bit) continue;
        ch |= (0x80 >> i);
    }
    return true;
}

bool Encoder::getint(int &val, unsigned int bits) {
    int is = sizeof(val) * 8;
    if (bitCount_ < bitOutCount_ + bits || bits == 0 || bits > is) return false;
    val = 0;
    for (int i = is - bits; i < is; ++i) {
        bool bit;
        getbit(bit);
        if (!bit) continue;
        val |= (1 << (is - i - 1));;
    }
    return true;
}

void Encoder::roundBitCount() {
    while (bitCount_ % 8) ++bitCount_;
}

void Encoder::roundOutBitCount() {
    while (bitOutCount_ % 8) ++bitOutCount_;
}


