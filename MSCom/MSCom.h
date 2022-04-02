#ifndef MSCOM_MSCOM_H_
#define MSCOM_MSCOM_H_

#include "lib/nave96.h"

#include <exception>
#include <fstream>

#define NAVE96_BK_IN_BUF 64
#define NAVE96_B_SIZE BK_SZ*NAVE96_BK_IN_BUF
#define NAVE96_F_EXTENSION ".nave96"
#define NAVE96_F_EXTENSION_L 7 // counting dot too

// TODO Singleton

class MSCom {
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
    MSCom(int argc, char *argv[]);
    MSCom(const MSCom &) = delete;
    MSCom &operator=(const MSCom &) = delete;

    ~MSCom() = default;


};

#endif //MSCOM_MSCOM_H_
