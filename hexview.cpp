#include <iostream>
#include <fstream>
#include <cstring>  // For strcmp()

char* hexConversionPadded(int num) {
    static char buffer[11];

    buffer[0] = '0';
    buffer[1] = '0';

    // Convert the 32-bit integer to 8 hex digits
    for (int i = 7; i >= 0; --i) {
        int shift = i * 4;  // Each nibble (half a bit) is 4 bits
                            // Shift is up to 28 bits (7 digits * 4)
        int nibble = (num >> shift) & 0xF;

        // The most significant nibble is i=9, the least i=0
        // If 2+(7-i), the most significant nibble goes in i=2
        buffer[2 + (7 - i)] = (nibble < 10) ? static_cast<char>(nibble + '0') : static_cast<char>(nibble - 10 + 'A');
    }

    // Null-terminate the string.
    buffer[10] = '\0';
    return buffer;
}

void convertFromASCII(char ch, char* hexBuffer) {
    const char hexDigits[] = "0123456789ABCDEF";

    // Convert the character to an unsigned value to handle characters > 127
    unsigned char value = static_cast<unsigned char>(ch);

    hexBuffer[0] = hexDigits[(value >> 4) & 0xF];  // High nibble; value / 2^4 & 1111 (masks out all but the lowest 4 bits)
    hexBuffer[1] = hexDigits[value & 0xF];         // Low nibble
    hexBuffer[2] = '\0';                           // Null terminator
}

int main(int argc, char* argv[]) {
    if (argc < 1 || argc > 6) {
        std::cout << "Use for hexview:\n";
        std::cout << "\t" << argv[0] << " [] [-n (bytes)] / [-s (bytes)] / [-n (bytes) -s (bytes)] source_file" << std::endl;
        return 0;
    }

    if ((strcmp(argv[1], "-n") == 0 && strcmp(argv[3], "-s") == 0) || (strcmp(argv[1], "-s") == 0 && strcmp(argv[3], "-n") == 0)) {
        std::ifstream inFile(argv[5], std::ios_base::binary);
        if (!inFile) {
            std::cout << "Cannot open file " << argv[5] << std::endl;
            return 0;
        }

        int endByte;
        if (strcmp(argv[1], "-n") == 0) {
            endByte = std::atoi(argv[2]);
        }
        else {
            endByte = std::atoi(argv[4]);
        }

        // File size
        /*inFile.seekg(0, std::ifstream::end);
        std::streampos endPosCheck = inFile.tellg();
        int endByteCheck = static_cast<int>(endPosCheck);*/

        int startByte;
        if (strcmp(argv[3], "-s") == 0) {
            startByte = std::atoi(argv[4]);
        }
        else {
            startByte = std::atoi(argv[2]);
        }

        inFile.seekg(startByte, std::ifstream::beg);
        std::streampos startPos = inFile.tellg();

        inFile.seekg(endByte, std::ifstream::cur);
        std::streamoff endPos = inFile.tellg();

        std::streamoff length = endPos - startPos;

        // Check 
        /*std::streamoff available = endByteCheck - startByte;
        if (length > available) {
            length = available;
        }*/

        length = (length > endPos ? endPos : length);

        inFile.seekg(startByte, std::ifstream::beg);

        char* buffer = new char[length];
        inFile.read(buffer, length);

        // std::cout.write(buffer, length);

        // std::ofstream outFile("out.txt");

        int index = 0;
        for (int i = 0; i < length; i+=16) {
            char* number = hexConversionPadded(i);
            std::cout << number << ": "; 
            // outFile << number << ": ";

            // Calculate the number of bytes to process
            int bytesLeft = static_cast<int>(length) - index;
            int bytesToProcess = (bytesLeft < 16) ? bytesLeft : 16;

            for (int k = index; k < index + bytesToProcess; ++k) {
                if ((k % 8 == 0) && (k != 0) && (k % 16 != 0)) {
                    std::cout << "| ";
                    // outFile << "| ";
                }

                char hexBuffer[3];
                convertFromASCII(buffer[k], hexBuffer);
                std::cout << hexBuffer << ' ';
                // outFile << hexBuffer << ' ';
            }

            // If fewer than 16 bytes are left to proceed
            // Fill the remaining hex space
            if (bytesToProcess < 16) {
                for (int padding = bytesToProcess; padding < 16; ++padding) {
                    if (padding % 8 == 0 && padding != 0) {
                        std::cout << "| ";
                        // outFile << "| ";
                    }

                    std::cout << "   ";  // 3 spaces
                    // outFile << "   ";
                }
            }

            std::cout << ' ';
            // outFile << ' ';

            for (int k = index; k < index + bytesToProcess; ++k) {
                std::cout << (static_cast<unsigned char>(buffer[k]) < 32 ? '.' : buffer[k]);
                // outFile << (static_cast<unsigned char>(buffer[k]) < 32 ? '.' : buffer[k]);
            }

            std::cout << (i < length - 16 ? "\n" : "");
            // outFile << (i < length - 16 ? "\n" : "");

            index += 16;
        }

        delete[] buffer;
        inFile.close();
    }
    else if (strcmp(argv[1], "-n") == 0) {
        std::ifstream inFile(argv[3], std::ios_base::binary);
        if (!inFile) {
            std::cout << "Cannot open file " << argv[3] << std::endl;
            return 0;
        }

        // File size
        inFile.seekg(0, std::ifstream::end);
        std::streamoff endPos = inFile.tellg();
        // int endByte = static_cast<int>(endPos);

        inFile.seekg(0, std::ifstream::beg);

        // Get length of needed file text
        std::streamoff length = std::atoi(argv[2]);

        // Check
        length = (length > endPos ? endPos : length);

        // Allocate memory
        char* buffer = new char[length];

        // Read data as a block
        inFile.read(buffer, length);

        // std::cout.write(buffer, length);

        int index = 0;
        for (int i = 0; i < length; i += 16) {
            char* number = hexConversionPadded(i);
            std::cout << number << ": ";

            // Calculate the number of bytes to process
            int bytesLeft = static_cast<int>(length) - index;
            int bytesToProcess = (bytesLeft < 16) ? bytesLeft : 16;

            for (int k = index; k < index + bytesToProcess; ++k) {
                if ((k % 8 == 0) && (k != 0) && (k % 16 != 0)) {
                    std::cout << "| ";
                }

                char hexBuffer[3];
                convertFromASCII(buffer[k], hexBuffer);
                std::cout << hexBuffer << ' ';
            }

            // If fewer than 16 bytes are left to proceed
            // Fill the remaining hex space
            if (bytesToProcess < 16) {
                for (int padding = bytesToProcess; padding < 16; ++padding) {
                    if (padding % 8 == 0 && padding != 0) {
                        std::cout << "| ";
                    }

                    std::cout << "   ";  // 3 spaces
                }
            }

            std::cout << ' ';

            for (int k = index; k < index + bytesToProcess; ++k) {
                std::cout << (static_cast<unsigned char>(buffer[k]) < 32 ? '.' : buffer[k]);
            }

            std::cout << (i < length - 16 ? "\n" : "");

            index += 16;
        }

        delete[] buffer;
        inFile.close();
    }
    else if (strcmp(argv[1], "-s") == 0) {
        std::ifstream inFile(argv[3], std::ios_base::binary);
        if (!inFile) {
            std::cout << "Cannot open file " << argv[3] << std::endl;
            return 0;
        }

        int startByte = std::atoi(argv[2]);
        inFile.seekg(startByte, std::ifstream::beg);  // Move to the needed byte
        std::streampos startPos = inFile.tellg();  // Get the position of the starting byte

        inFile.seekg(0, std::ifstream::end);
        std::streampos endPos = inFile.tellg();

        // Calculate the number of bytes
        std::streamoff length = endPos - startPos;

        // Check
        length = (length < 0 ? 0 : length);

        // Move to the needed byte
        inFile.seekg(startByte, std::ifstream::beg);

        char* buffer = new char[length];
        inFile.read(buffer, length);

        // std::cout.write(buffer, length);

        int index = 0;
        for (int i = 0; i < length; i += 16) {
            char* number = hexConversionPadded(i);
            std::cout << number << ": ";

            // Calculate the number of bytes to process
            int bytesLeft = static_cast<int>(length) - index;
            int bytesToProcess = (bytesLeft < 16) ? bytesLeft : 16;

            for (int k = index; k < index + bytesToProcess; ++k) {
                if ((k % 8 == 0) && (k != 0) && (k % 16 != 0)) {
                    std::cout << "| ";
                }

                char hexBuffer[3];
                convertFromASCII(buffer[k], hexBuffer);
                std::cout << hexBuffer << ' ';
            }

            // If fewer than 16 bytes are left to proceed
            // Fill the remaining hex space
            if (bytesToProcess < 16) {
                for (int padding = bytesToProcess; padding < 16; ++padding) {
                    if (padding % 8 == 0 && padding != 0) {
                        std::cout << "| ";
                    }

                    std::cout << "   ";  // 3 spaces
                }
            }

            std::cout << ' ';

            for (int k = index; k < index + bytesToProcess; ++k) {
                std::cout << (static_cast<unsigned char>(buffer[k]) < 32 ? '.' : buffer[k]);
            }

            std::cout << (i < length - 16 ? "\n" : "");

            index += 16;
        }

        delete[] buffer;
        inFile.close();
    }
    else {
        std::ifstream inFile(argv[1], std::ios_base::binary);
        if (!inFile) {
            std::cout << "Cannot open file " << argv[1] << std::endl;
            return 0;
        }

        // File size
        inFile.seekg(0, std::ifstream::end);
        std::streampos endPos = inFile.tellg();

        std::streamoff length = inFile.tellg();

        inFile.seekg(0, std::ifstream::beg);

        char* buffer = new char[length];
        inFile.read(buffer, length);

        int index = 0;
        for (int i = 0; i < length; i += 16) {
            char* number = hexConversionPadded(i);
            std::cout << number << ": ";

            // Calculate the number of bytes to process
            int bytesLeft = static_cast<int>(length) - index;
            int bytesToProcess = (bytesLeft < 16) ? bytesLeft : 16;

            for (int k = index; k < index + bytesToProcess; ++k) {
                if ((k % 8 == 0) && (k != 0) && (k % 16 != 0)) {
                    std::cout << "| ";
                }

                char hexBuffer[3];
                convertFromASCII(buffer[k], hexBuffer);
                std::cout << hexBuffer << ' ';
            }

            // If fewer than 16 bytes are left to proceed
            // Fill the remaining hex space
            if (bytesToProcess < 16) {
                for (int padding = bytesToProcess; padding < 16; ++padding) {
                    if (padding % 8 == 0 && padding != 0) {
                        std::cout << "| ";
                    }

                    std::cout << "   ";  // 3 spaces
                }
            }

            std::cout << ' ';

            for (int k = index; k < index + bytesToProcess; ++k) {
                std::cout << (static_cast<unsigned char>(buffer[k]) < 32 ? '.' : buffer[k]);
            }

            std::cout << (i < length - 16 ? "\n" : "");

            index += 16;
        }

        delete[] buffer;
        inFile.close();
    }

    return 0;
}
