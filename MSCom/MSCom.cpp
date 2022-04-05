#include "MSCom.h"

#include <iostream>

namespace msc {

    MSCom &MSCom::getInstance() noexcept {
        static MSCom instance;
        return instance;
    }

    bool MSCom::run() {

        switch (state) {
            case State::INFO: {
                showHelp();
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
        for (eBegin = (int)path.size() - 1; eBegin >= 0; --eBegin) {
            if (path[eBegin] == '.') break;
        }
        if (eBegin < 0)
            return path;
        path.resize(eBegin);
        return path;
    }

    void MSCom::showHelp() const noexcept {
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

        bool success {true};
        measurements.setFlags(COM_MES_TIME | COM_MES_BYTES);
        measurements.run();

        switch (outputFormat) {
            case FileFormat::nave96: {
                success = fCompressNave96();
                break;
            }
            case FileFormat::uncompressed: {
                throw std::logic_error("MSCom::compressFile() : "
                                       "uncompressed output format.");
            }
        }

        measurements.stop();
        measurements.show();

        return success;
    }

    bool MSCom::decompressFile() {
        if (!openFiles())
            return false;

        bool success {true};
        measurements.setFlags(COM_MES_TIME);
        measurements.run();

        switch (inputFormat) {
            case FileFormat::nave96: {
                success = fDecompressNave96();
                break;
            }
            case FileFormat::uncompressed: {
                throw std::logic_error("MSCom::decompressFile() : "
                                       "uncompressed input format.");
            }
        }

        measurements.stop();
        measurements.show();

        return success;
    }

    bool MSCom::fCompressNave96() {
        static const size_t buffSize = BK_SZ * 64;
        char inbuff[buffSize];
        nave96 nave;

        uint32_t bytesIn = {0};
        uint32_t bytesOut {0};

        try {
            std::cout << "Running compression!\n";
            uint8_t outBuffer[buffSize];
            Encoder ec(outBuffer, sizeof(outBuffer));
            while (!inputFile.fail()) {
                unsigned p = 0;
                while (p < buffSize && inputFile.get(inbuff[p])) {
                    ++p;
                }
                bytesIn += p;

                nave.compress(inbuff, p, ec);

                for (int i = 0; i < ec.bitsDone() / 8; ++i)
                    outputFile << outBuffer[i];

                bytesOut += ec.bitsDone() / 8;
                ec.resetBitCount();
            }
            measurements.setBytesIn(bytesIn);
            measurements.setBytesOut(bytesOut);
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

            std::vector<uint8_t> inputData;
            for (char c; inputFile.get(c); inputData.emplace_back(c));

            nave96 nave;
            Decoder inputDecoder(&inputData[0], inputData.size());
            uint8_t buff[BK_SZ];
            Encoder outputEncoder(buff, sizeof(buff));
            while (inputDecoder.bitsInQueue() > 0) {
                outputEncoder.resetBitCount();
                nave.decompress(inputDecoder, outputEncoder, 1);
                for (int i = 0; i < outputEncoder.bitsDone() / 8; ++i)
                    outputFile.put(buff[i]);
            }

            std::cout << "Decompression is finished!\n";
        }
        catch (std::exception &e) {
            std::cerr << e.what() << std::endl;
            return false;
        }
        return true;
    }

} // namespace msc
