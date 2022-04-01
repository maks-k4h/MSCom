#include <iostream>
#include <bitset>

#include "MSComp/MSComp.h"

void printBinary(char *arr, unsigned sz) {
    std::cout << "Number of bytes: " << sz << std::endl;
    for (auto i = 0u; i < sz; ++i) {
        std::cout << std::bitset<8>(arr[i]) << ' ';
    }
    std::cout << std::endl;
}

int main(int argc, char *argv[]) {
    MSComp bmp(argc, argv);
    return 0;
}
