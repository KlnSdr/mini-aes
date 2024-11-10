#include<iostream>
#include <map>
#include <stdexcept>

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

const bool isVerbose = true;

const char C[2][2] = {
    {0x3, 0x2},
    {0x2, 0x3}
};

const char keyConstants[2] = {0x1, 0x2};

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

char mult(char a, char b) {
    char result = 0;
    for (int i = 0; i < 4; ++i) {
        if ((a >> i) & 0x1) {
            char shiftedB = b << i;
            result ^= shiftedB;
        }
    }
    return result;
}

char* mMult(const char* blk) {
    char* newBlk = new char[blkSize];
    
    newBlk[0] = (mult(C[0][0], blk[0]) ^ mult(C[0][1], blk[1])) ^ 0x13;
    newBlk[1] = (mult(C[1][0], blk[0]) ^ mult(C[1][1], blk[1])) ^ 0x13;
    newBlk[2] = (mult(C[0][0], blk[2]) ^ mult(C[0][1], blk[3])) ^ 0x13;
    newBlk[3] = (mult(C[1][0], blk[2]) ^ mult(C[1][1], blk[3])) ^ 0x13;
    
    return newBlk;
}

char* nextKey(const char* k, size_t current_iteration) {
    if (current_iteration > 1) {
        throw std::invalid_argument("more iterations calculated then key constants implemented");
    }
    char* newKey = new char[blkSize];
    newKey[0] = (k[0] ^ substitutions.at(k[3])) ^ keyConstants[current_iteration];
    newKey[1] = k[1] ^ newKey[0];
    newKey[2] = k[2] ^ newKey[1];
    newKey[3] = k[3] ^ newKey[2];
    return newKey;
}

void printHexArray(const char* arr, size_t size) {
    for (size_t i = 0; i < size; i++) {
        std::cout << "0x" << std::hex << std::uppercase << (int)(unsigned char)arr[i] << " ";
    }
    std::cout << std::dec << std::endl;
}

char* aes_encrypt(const char* msg, const char* key, size_t iterations) {
    char* keyAdd;
    char* substituted;
    char* shifted;
    char* multiplied;
    char* newKey = new char[blkSize];

    for (size_t i = 0; i < blkSize; i++) {
        newKey[i] = key[i];
    }

    for (size_t i = 0; i < iterations; i++) {
        keyAdd = addKey(msg, newKey);
        substituted = sub(keyAdd);
        shifted = shift(substituted);
        multiplied = mMult(shifted);

        if (i < iterations - 1) {
            newKey = nextKey(newKey, i);
        }

        if (isVerbose) {
            std::cout << "iteration " << i << " ===========" << std::endl;
            std::cout << "msg: ";
            printHexArray(msg, blkSize);

            std::cout << "key: ";
            printHexArray(newKey, blkSize);
            std::cout << "addKey(msg, key): ";
            printHexArray(keyAdd, blkSize);
            std::cout << "sub(blk): ";
            printHexArray(substituted, blkSize);
            std::cout << "shift(blk): ";
            printHexArray(shifted, blkSize);
            std::cout << "mMult(blk): ";
            printHexArray(multiplied, blkSize);
            std::cout << "nextKey(key, " << i << "): ";
            printHexArray(newKey, blkSize);
        }
    }
    delete[] keyAdd;
    delete[] substituted;
    delete[] shifted;
    delete[] newKey;

    return multiplied;
}

int main() {
    char* encrypted = aes_encrypt(msg, key, 3);
    printHexArray(encrypted, blkSize);
    delete[] encrypted;
    return 0;
}
