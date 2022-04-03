#ifndef MSCOM_MSCOM_H_
#define MSCOM_MSCOM_H_

#include "lib/nave96.h"
#include "ComMeasurements.h"

#include <fstream>

#define NAVE96_BK_IN_BUF 64
#define NAVE96_B_SIZE BK_SZ*NAVE96_BK_IN_BUF

namespace msc {

    class MSCom {
    public:
        static MSCom &getInstance() noexcept;
        MSCom &operator=(const MSCom &) = delete;
        ~MSCom() = default;

        void processArguments(int argc, char *argv[]);

        bool run();

    private:
        // private constructor to implement singleton
        MSCom() = default;

        enum class FileFormat {
            nave96,
            uncompressed
        };

        enum class State {
            DEFAULT,
            INFO,
            COMPRESS,
            DECOMPRESS
        };

        const std::vector<std::tuple<std::string, FileFormat>> fileExtensions
        {
            {".nave96", FileFormat::nave96},
        };

        State state{State::DEFAULT};
        FileFormat inputFormat{FileFormat::uncompressed};
        FileFormat outputFormat{FileFormat::uncompressed};
        std::string inFilePath;
        std::string outFilePath;
        std::ifstream inputFile;
        std::ofstream outputFile;
        ComMeasurements measurements;


        bool compressFile();
        bool decompressFile();
        bool fCompressNave96();
        bool fDecompressNave96();

        FileFormat getFileFormat(const std::string &path) const;
        std::string getExtensionByFormat(FileFormat) const;
        std::string removeExtension(std::string path) const;

        bool openFiles();

        void showHelp() const noexcept;
    };

} // namespace msc

#endif //MSCOM_MSCOM_H_
