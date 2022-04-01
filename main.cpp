#include <iostream>
#include <bitset>

#include "MSCom/MSCom.h"

void printBinary(char *arr, unsigned sz) {
    std::cout << "Number of bytes: " << sz << std::endl;
    for (auto i = 0u; i < sz; ++i) {
        std::cout << std::bitset<8>(arr[i]) << ' ';
    }
    std::cout << std::endl;
}

int main(int argc, char *argv[]) {
    MSCom bmp(argc, argv);
    return 0;
}
