#include<iostream>
#include <map>
#include <cstring>
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

const char msg[8] = {0x9, 0xC, 0x6, 0x3, 0x9, 0xC, 0x6, 0x3};
/* const char msg[4] = {0x9, 0xC, 0x6, 0x3}; */
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
    
    newBlk[0] = mult(C[0][0], blk[0]) ^ mult(C[0][1], blk[1]);
    newBlk[1] = mult(C[1][0], blk[0]) ^ mult(C[1][1], blk[1]);
    newBlk[2] = mult(C[0][0], blk[2]) ^ mult(C[0][1], blk[3]);
    newBlk[3] = mult(C[1][0], blk[2]) ^ mult(C[1][1], blk[3]);

    for (size_t i = 0; i < blkSize; i++) {
        if (newBlk[i] & 0x10) {
            newBlk[i] = newBlk[i] ^ 0x13;
        }
    }
    
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

char* aes_encrypt_block(const char* message, const char* key) {
    char* keyAdd;
    char* substituted;
    char* shifted;
    char* multiplied;

    keyAdd = addKey(message, key);
    substituted = sub(keyAdd);
    shifted = shift(substituted);
    multiplied = mMult(shifted);
    if (isVerbose) {
        std::cout << "message: ";
        printHexArray(message, blkSize);

        std::cout << "key: ";
        printHexArray(key, blkSize);
        std::cout << "addKey(message, key): ";
        printHexArray(keyAdd, blkSize);
        std::cout << "sub(blk): ";
        printHexArray(substituted, blkSize);
        std::cout << "shift(blk): ";
        printHexArray(shifted, blkSize);
        std::cout << "mMult(blk): ";
        printHexArray(multiplied, blkSize);
    }
    delete[] keyAdd;
    delete[] substituted;
    delete[] shifted;

    return multiplied;
}

char* zeroPad(const char* message, size_t msgLen, size_t &paddedLength) {
    size_t paddingSize = (blkSize - (msgLen % blkSize)) % blkSize;
    paddedLength = msgLen + paddingSize;
    
    // Allocate memory for the padded message
    char* paddedMessage = new char[paddedLength];
    
    // Copy the original message into the padded message
    std::memcpy(paddedMessage, message, msgLen);
    
    // Add zero padding to the remaining bytes
    std::memset(paddedMessage + msgLen, 0, paddingSize);
    
    return paddedMessage;
}

char* aes_encrypt(const char* msg, size_t msgLen, const char* key, size_t iterations) {
    char* newKey = new char[blkSize];
    char* encrypted_msg;

    // copy key
    for (size_t i = 0; i < blkSize; i++) {
        newKey[i] = key[i];
    }

    size_t paddedLen = 0;
    encrypted_msg = zeroPad(msg, msgLen, paddedLen);

    for (size_t i = 0; i < iterations; i++) {
        if (isVerbose) {
            std::cout << "iteration " << i << " ===========" << std::endl;
        }

        for (size_t blkNum = 0; blkNum < paddedLen/blkSize; blkNum++) {
            if (isVerbose) {
                std::cout << "block " << blkNum << std::endl;
            }
            char* blk = new char[blkSize];
            // extract next block
            for (size_t j = 0; j < blkSize; j++) {
                blk[j] = encrypted_msg[j + (blkNum * blkSize)];
            }
            blk = aes_encrypt_block(blk, newKey);
            // copy block to encrypted message
            for (size_t j = 0; j < blkSize; j++) {
                encrypted_msg[j + (blkNum * blkSize)] = blk[j];
            }
            delete[] blk;
        }

        if (i < iterations - 1) {
            newKey = nextKey(newKey, i);
        }

        if (isVerbose) {
            std::cout << "nextKey(key, " << i << "): ";
            printHexArray(newKey, blkSize);
        }
    }
    delete[] newKey;

    return encrypted_msg;
}

int main() {
    char* encrypted = aes_encrypt(msg, 8, key, 3);
    printHexArray(encrypted, 8);
    delete[] encrypted;
    return 0;
}
