#include "MSCom.h"

#include <iostream>

MSCom::MSCom(int argc, char **argv) {
    argc_ = argc;
    argv_ = argv;
    processArguments();
}


bool MSCom::compressFile(MSCom::alg a,
                         const std::string &inpath,
                         std::string outpath) {
    if (outpath.empty()) {
        outpath = inpath + NAVE96_F_EXTENSION;
    }
    input.open(inpath, std::ios_base::in);
    if (!input.is_open()) {
        std::cout << "File cannot be opened!\n";
        return false;
    }
    output.open(outpath, std::ios_base::out);
    if (!output.is_open()) {
        std::cout << "Cannot create output file!\n";
        return false;
    }

    switch (a) {
        case alg::nave96: {
            return fCompressNave96();
        }
    }
}

bool MSCom::decompressFile(MSCom::alg a,
                           const std::string &
                           inpath, std::string outpath) {
    if (outpath.empty()) {
        outpath = inpath.substr(0, inpath.length() - NAVE96_F_EXTENSION_L);
    }
    input.open(inpath, std::ios_base::in);
    if (!input.is_open()) {
        std::cout << "File cannot be opened!\n";
        return false;
    }
    output.open(outpath, std::ios_base::out);
    if (!output.is_open()) {
        std::cout << "Cannot create output file!\n";
        return false;
    }

    switch (a) {
        case alg::nave96: {
            return fDecompressNave96();
        }
    }
}

bool MSCom::fCompressNave96() {
    char inbuff[NAVE96_B_SIZE];
    nave96 nave;

    unsigned long bytesin = 0;
    unsigned long bytesout = 0;

    try {
        std::cout << "Running compression!\n";
        Encoder ec(NAVE96_B_SIZE);
        while (!input.fail()) {
            unsigned p = 0;
            while (p < NAVE96_B_SIZE && input.get(inbuff[p])) {
                ++p, ++bytesin;
            }
            // now p is the length
            nave.compress(inbuff, p, &ec);;
            for (char c; ec.getc(c);) {
                output << c;
                bytesout++;
            }

            ec.reset(false);
            std::cout << "Done: " << bytesin / 1024 << "kb\n";
        }

        std::cout << "Compression is finished!\n";
        std::cout << "Compression info:\n"
                     "\tBytes on input: " << bytesin << "\n"
                                                        "\tBytes on output: " << bytesout << "\n"
                                                                                             "\tRatio: "
                  << 100.0 * bytesout / bytesin << "%\n";
    }
    catch (std::exception &e) {
        std::cout << e.what() << std::endl;
        return false;
    }
    return true;
}

bool MSCom::fDecompressNave96() {
    try {
        std::cout << "Running decompression!\n";

        nave96 nave;
        Encoder ec;
        for (char c; input.get(c); ec.putc(c));

        char buff[BK_SZ];
        while (ec.getBitsInQueue()) {
            unsigned sz = nave.decompress(&ec, buff, 1);
            for (int i = 0; i < sz; ++i) output << buff[i];
        }

        std::cout << "Decompression is finished!\n";
    }
    catch (std::exception &e) {
        std::cout << e.what() << std::endl;
        return false;
    }
    return true;
}

void MSCom::processArguments() {
    if (argc_ == 1 || !strcmp(argv_[1], "-h") || !strcmp(argv_[1], "--help")) {
        printHelp();
        return;
    }
    std::string path(argv_[1]);
    if (path.ends_with(NAVE96_F_EXTENSION)) {
        // decompression
        decompressFile(alg::nave96, path);
    } else {
        // compression
        compressFile(alg::nave96, path);
    }
}


void MSCom::printHelp() {
    std::cout << "BOOMPRESSOR v0.2.0b\n";
    std::cout << "Usage: [filepath]\n";
    std::cout << "File will be either compressed or decompressed\n"
                 "depending on its extension.\n";
}






