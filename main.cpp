#include "MSCom/MSCom.h"

int main(int argc, char *argv[]) {
    msc::MSCom &compressor = msc::MSCom::getInstance();
    compressor.processArguments(argc, argv);
    compressor.run();
    return 0;
}
