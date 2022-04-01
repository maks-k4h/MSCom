#ifndef BOOMPRESSOR_H
#define BOOMPRESSOR_H

#include "lib/nave96.h"

#include <exception>
#include <fstream>

#define NAVE96_BK_IN_BUF 64
#define NAVE96_B_SIZE BK_SZ*NAVE96_BK_IN_BUF
#define NAVE96_F_EXTENSION ".nave96"
#define NAVE96_F_EXTENSION_L 7 // counting dot too

class MSComp {
    enum class alg {
        nave96
    };

    std::ifstream input;
    std::ofstream output;

    int argc_;
    char **argv_ = nullptr; // | no destructor

    bool fCompressNave96();

    bool fDecompressNave96();

    bool compressFile(alg, const std::string &inpath, std::string outpath = "");

    bool decompressFile(alg, const std::string &inpath, std::string outpath = "");

    void processArguments();

    void printHelp();

public:
    MSComp(int argc, char *argv[]);

    MSComp(const MSComp &) = delete;

    MSComp &operator=(const MSComp &) = delete;

    ~MSComp() = default;


};

#endif //BOOMPRESSOR_H
