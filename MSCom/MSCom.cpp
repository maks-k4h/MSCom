#include "MSCom.h"

#include <iostream>

MSCom &MSCom::getInstance() noexcept {
    static MSCom instance;
    return instance;
}

bool MSCom::run() {
    switch (state) {
        case State::INFO: {
            printHelp();
            return true;
        }
        case State::COMPRESS: {
            return compressFile();
        }
        case State::DECOMPRESS: {
            return decompressFile();
        }
        case State::DEFAULT: {
            return true;
        }
    }
}

MSCom::FileFormat MSCom::getFileFormat(const std::string &path) const {
    for (auto ext: fileExtensions)
        if (path.ends_with(std::get<0>(ext)))
            return std::get<1>(ext);

    return FileFormat::uncompressed;
}

std::string MSCom::getExtensionByFormat(MSCom::FileFormat format) const {
    for (auto ext: fileExtensions) {
        if (std::get<1>(ext) == format)
            return std::get<0>(ext);
    }
    return {};
}

std::string MSCom::removeExtension(std::string path) const {
    int eBegin{0};
    for (eBegin = static_cast<int>(path.size()) - 1; eBegin >= 0; --eBegin) {
        if (path[eBegin] == '.') break;
    }
    if (eBegin < 0)
        return path;
    path.resize(eBegin);
    return path;
}

void MSCom::printHelp() const noexcept {
    std::cout << "MSCom v0.3.0\n";
    std::cout << "Usage: [filepath]\n";
}

// MSComp::processArguments.
// Set state and needed values:
//  INFO:
//      No additional variables required
//  COMPRESS:
//      Input file format : uncompressed
//      Output file format
//      Input file path
//      Output file path
//  DECOMPRESS:
//      Input file format
//      Output file format : uncompressed
//      Input file path
//      Output file path
//  DEFAULT:
//      No work to do

void MSCom::processArguments(int argc, char *argv[]) {
    if (argc < 1 || !argv) {
        state = State::DEFAULT;
        return;
    }

    if (argc == 1 || !strcmp(argv[1], "-h") || !strcmp(argv[1], "--help")) {
        state = State::INFO;
        return;
    }

    inFilePath = argv[1];
    inputFormat = getFileFormat(inFilePath);
    if (inputFormat == FileFormat::uncompressed) {
        // compression
        state = State::COMPRESS;

        // nave96 format only
        outputFormat = FileFormat::nave96;

        // getting output file path
        outFilePath = inFilePath + getExtensionByFormat(outputFormat);

    } else {
        // decompression
        state = State::DECOMPRESS;
        outputFormat = FileFormat::uncompressed;

        outFilePath = removeExtension(inFilePath);
    }
}

bool MSCom::openFiles() {
    inputFile.open(inFilePath, std::ios_base::in);
    if (!inputFile.is_open()) {
        std::cout << "File cannot be opened!\n";
        return false;
    }
    outputFile.open(outFilePath, std::ios_base::out);
    if (!outputFile.is_open()) {
        std::cout << "Cannot create output file!\n";
        return false;
    }
    return true;
}


bool MSCom::compressFile() {
    if (!openFiles())
        return false;

    switch (outputFormat) {
        case FileFormat::nave96: {
            return fCompressNave96();
        }
        case FileFormat::uncompressed: {
            throw std::logic_error("MSCom::compressFile() : "
                                   "uncompressed output format.");
        }
    }
}

bool MSCom::decompressFile() {
    if (!openFiles())
        return false;

    switch (inputFormat) {
        case FileFormat::nave96: {
            return fDecompressNave96();
        }
        case FileFormat::uncompressed: {
            throw std::logic_error("MSCom::decompressFile() : "
                                   "uncompressed input format.");
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
        while (!inputFile.fail()) {
            unsigned p = 0;
            while (p < NAVE96_B_SIZE && inputFile.get(inbuff[p])) {
                ++p, ++bytesin;
            }
            // now p is the length
            nave.compress(inbuff, p, &ec);;
            for (char c; ec.getc(c);) {
                outputFile << c;
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
        for (char c; inputFile.get(c); ec.putc(c));

        char buff[BK_SZ];
        while (ec.getBitsInQueue()) {
            unsigned sz = nave.decompress(&ec, buff, 1);
            for (int i = 0; i < sz; ++i) outputFile << buff[i];
        }

        std::cout << "Decompression is finished!\n";
    }
    catch (std::exception &e) {
        std::cout << e.what() << std::endl;
        return false;
    }
    return true;
}
