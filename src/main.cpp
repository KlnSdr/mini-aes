#include<iostream>
#include <map>
#include <cstring>
#include <stdexcept>

// SCROLL DOWN TO SET MSG AND KEY

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

// TODO make it pretty
const std::map<char, char> invSubstitutions {
    {0xE, 0x0},
    {0x4, 0x1},
    {0xD, 0x2},
    {0x1, 0x3},
    {0x2, 0x4},
    {0xF, 0x5},
    {0xB, 0x6},
    {0x8, 0x7},
    {0x3, 0x8},
    {0xA, 0x9},
    {0x6, 0xA},
    {0xC, 0xB},
    {0x5, 0xC},
    {0x9, 0xD},
    {0x0, 0xE},
    {0x7, 0xF},
};

const std::map<char, char> strToNibble {
    {'0', 0x0},
    {'1', 0x1},
    {'2', 0x2},
    {'3', 0x3},
    {'4', 0x4},
    {'5', 0x5},
    {'6', 0x6},
    {'7', 0x7},
    {'8', 0x8},
    {'9', 0x9},
    {'a', 0xA},
    {'b', 0xB},
    {'c', 0xC},
    {'d', 0xD},
    {'e', 0xE},
    {'f', 0xF},
};

bool isVerbose = false;

const char C[2][2] = {
    {0x3, 0x2},
    {0x2, 0x3}
};

const char keyConstants[2] = {0x1, 0x2};

const size_t blkSize = 4;

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

char* invSub(const char* blk) {
    char* newBlk = new char[blkSize];
    for (size_t i = 0; i < blkSize; i++) {
        newBlk[i] = invSubstitutions.at(blk[i] & 0xF);
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

char* aes_decrypt_block(const char* message, const char* key) {
    char* keyAdd;
    char* substituted;
    char* shifted;
    char* multiplied;

    multiplied = mMult(message);
    shifted = shift(multiplied);
    substituted = invSub(shifted);
    keyAdd = addKey(substituted, key);
    if (isVerbose) {
        std::cout << "message: ";
        printHexArray(message, blkSize);
        std::cout << "key: ";
        printHexArray(key, blkSize);
        std::cout << "mMult(blk): ";
        printHexArray(multiplied, blkSize);
        std::cout << "shift(blk): ";
        printHexArray(shifted, blkSize);
        std::cout << "invSub(blk): ";
        printHexArray(substituted, blkSize);
        std::cout << "addKey(message, key): ";
        printHexArray(keyAdd, blkSize);
    }
    delete[] multiplied;
    delete[] substituted;
    delete[] shifted;

    return keyAdd;
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
    if (isVerbose) {
        std::cerr << "+==================+" << std::endl;
        std::cerr << "|    encrypting    |" << std::endl;
        std::cerr << "+==================+" << std::endl;
    }

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

char* aes_decrypt(const char* msg, size_t msgLen, const char* key, size_t iterations) {
    if (isVerbose) {
        std::cerr << "+==================+" << std::endl;
        std::cerr << "|    decrypting    |" << std::endl;
        std::cerr << "+==================+" << std::endl;
    }

    char** keys = new char*[iterations];
    char* keyCopy = new char[blkSize];

    // TODO make pretty
    for (size_t i = 0; i < blkSize; i++) {
        keyCopy[i] = key[i];
    }

    keys[0] = keyCopy;

    for (size_t i = 1; i <iterations; i++) {
        keys[i] = nextKey(keys[i - 1], i - 1);
    }

    char* encrypted_msg;

    // TODO make pretty
    encrypted_msg = new char[msgLen];
    for (size_t i = 0; i < msgLen; i++) {
        encrypted_msg[i] = msg[i];
    }

    for (size_t i = 0; i < iterations; i++) {
        if (isVerbose) {
            std::cout << "iteration " << i << " ===========" << std::endl;
        }

        for (size_t blkNum = 0; blkNum < msgLen/blkSize; blkNum++) {
            if (isVerbose) {
                std::cout << "block " << blkNum << std::endl;
            }
            char* blk = new char[blkSize];
            // extract next block
            for (size_t j = 0; j < blkSize; j++) {
                blk[j] = encrypted_msg[j + (blkNum * blkSize)];
            }
            blk = aes_decrypt_block(blk, keys[iterations - i - 1]);
            // copy block to encrypted message
            for (size_t j = 0; j < blkSize; j++) {
                encrypted_msg[j + (blkNum * blkSize)] = blk[j];
            }
            delete[] blk;
        }

        if (isVerbose && i < iterations - 1) {
            std::cout << "next key: ";
            printHexArray(keys[iterations - i - 2], blkSize);
        }
    }
    delete[] keys;

    return encrypted_msg;
}

int main(int argc, char* argv[]) {
    int iterations = 1;
    char* message = {};
    size_t messageLength = 0;
    char* key = {};
    size_t keyLen = 0;
    bool doEncrypt = true;
    // Loop through all command-line arguments
    for (int i = 1; i < argc; ++i) { // Start at 1 to skip program name
        if (std::strcmp(argv[i], "-v") == 0) {
            isVerbose = true;
        } else if (std::strcmp(argv[i], "-e") == 0) {
            doEncrypt = true;
        } else if (std::strcmp(argv[i], "-d") == 0) {
            doEncrypt = false;
        } else if (std::strcmp(argv[i], "-i") == 0) {
            // Check if there’s another argument after "-i"
            if (i + 1 < argc) {
                iterations = std::atoi(argv[i + 1]); // Convert the next argument to an integer
                i++; // Skip the next argument since we just processed it
            } else {
                std::cerr << "Error: -i flag requires an integer argument. defaulting to 1" << std::endl;
                return 1;
            }
        } else if (std::strcmp(argv[i], "-m") == 0) {
            // Check if there’s another argument after "-i"
            if (i + 1 < argc) {
                message = argv[i + 1]; // Convert the next argument to an integer
                messageLength = strlen(message);
                i++; // Skip the next argument since we just processed it
            } else {
                std::cerr << "message not set" << std::endl;
                return 1;
            }
        } else if (std::strcmp(argv[i], "-k") == 0) {
            // Check if there’s another argument after "-i"
            if (i + 1 < argc) {
                key = argv[i + 1]; // Convert the next argument to an integer
                keyLen = strlen(key);
                i++; // Skip the next argument since we just processed it
            } else {
                std::cerr << "key not set" << std::endl;
                return 1;
            }
        }
    }

    if (messageLength == 0) {
        std::cerr << "no message given" << std::endl;
        return 0;
    }

    if (keyLen == 0) {
        std::cerr << "no key given" << std::endl;
        return 0;
    } else if (keyLen < blkSize) {
        std::cerr << "key is too short" << std::endl;
        return 0;
    }

    for (size_t i = 0; i < messageLength; i++) {
        char c = tolower(message[i]);
        if (strToNibble.count(c) <= 0) {
            std::cerr << "Err: message contains non-hex value: " << c << std::endl;
            return 1;
        }
        message[i] = strToNibble.at(c);
    }

    for (size_t i = 0; i < keyLen; i++) {
        char c = tolower(key[i]);
        if (strToNibble.count(c) <= 0) {
            std::cerr << "Err: key contains non-hex value: " << c << std::endl;
            return 1;
        }
        key[i] = strToNibble.at(c);
    }

    char* result;

    if (doEncrypt) {
        result = aes_encrypt(message, messageLength, key, iterations);
    } else {
        result = aes_decrypt(message, messageLength, key, iterations);
    }

    printHexArray(result, messageLength + ((blkSize - (messageLength % blkSize)) % blkSize));
    delete[] result;
    return 0;
}
