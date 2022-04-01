#ifndef ENCODER_H
#define ENCODER_H

// Encoder class dedicated to create sequences on bit level

#include <vector>
#include <exception>
#include <string>

/*
 * TODO
 *  1) Change buff_ datatype to more effective one (queue)
 * */

class Encoder {
    std::vector<char> buff_;
    unsigned bitCount_ = 0;

    unsigned bitOutCount_ = 0;

public:
    Encoder() = default;

    Encoder(unsigned buffsz)
            : buff_(buffsz) {}

    Encoder(const Encoder &ec) = default;

    Encoder &operator=(const Encoder &) = default;

    ~Encoder() = default;

    void putbit(bool);

    // bits are counted from right to left
    void putc(char, unsigned bits = 8);

    void putint(int, unsigned bits = 8);

    // takes one char from buffer or return false
    bool getc(char &);

    // fills 'bits' last bits in ch
    bool getc(char &, unsigned bits);

    bool getint(int &, unsigned bits);

    bool getbit(bool &);

    unsigned getBitsInQueue() const { return bitCount_ - bitOutCount_; }
    //unsigned getActualBufferSize()  const {return buff_.size();}

    // rounds bitCount so that the last byte in buffer is full
    void roundBitCount();

    void roundOutBitCount();

    void reset(bool resetBuffer = true);

};


#endif //ENCODER_H
