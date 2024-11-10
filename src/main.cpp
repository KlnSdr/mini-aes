#include<iostream>
#include <map>

const std::map<char, char> substitutions {
    {0x0, 0xE},
    {0x1, 0x4},
    {0x2, 0xD},
    {0x3, 0x1},
    {0x4, 0x2},
    {0x5, 0xF},
    {0x6, 0xB},
    {0x7, 0x8},
    {0x8, 0x3},
    {0x9, 0xA},
    {0xA, 0x6},
    {0xB, 0xC},
    {0xC, 0x5},
    {0xD, 0x9},
    {0xE, 0x0},
    {0xF, 0x7},
};

const char C[2][2] = {
    {0x3, 0x2},
    {0x2, 0x3}
};

const size_t blkSize = 4;

const char msg[4] = {0x9, 0xC, 0x6, 0x3};
const char key[blkSize] = {0xC, 0x3, 0xF, 0x0};

char* addKey(const char* blk, const char* key) {
    char* newBlk = new char[blkSize];
    for (size_t i = 0; i < blkSize; i++) {
        newBlk[i] = blk[i] ^ key[i];
    }
    return newBlk;
}

char* sub(const char* blk) {
    char* newBlk = new char[blkSize];
    for (size_t i = 0; i < blkSize; i++) {
        newBlk[i] =  substitutions.at(blk[i] & 0xF);
    }
    return newBlk;
}

char* shift(const char* blk) {
    char* shiftedBlk = new char[blkSize];
    
    // Copy the original block
    for (size_t i = 0; i < blkSize; i++) {
        shiftedBlk[i] = blk[i];
    }
    
    // Swap index 1 and index 3
    std::swap(shiftedBlk[1], shiftedBlk[3]);

    return shiftedBlk;
}

void printHexArray(const char* arr, size_t size) {
    for (size_t i = 0; i < size; i++) {
        std::cout << "0x" << std::hex << std::uppercase << (int)(unsigned char)arr[i] << " ";
    }
    std::cout << std::dec << std::endl;
}

int main() {
    std::cout << "msg: ";
    printHexArray(msg, blkSize);

    std::cout << "key: ";
    printHexArray(key, blkSize);

    char* result = addKey(msg, key);
    std::cout << "addKey(msg, key): ";
    printHexArray(result, blkSize);

    char* substituted = sub(result);
    std::cout << "sub(blk): ";
    printHexArray(substituted, blkSize);

    char* shifted = shift(substituted);
    std::cout << "shift(blk): ";
    printHexArray(shifted, blkSize);

    // Clean up dynamically allocated memory
    delete[] result;
    delete[] substituted;
    delete[] shifted;

    return 0;
}
