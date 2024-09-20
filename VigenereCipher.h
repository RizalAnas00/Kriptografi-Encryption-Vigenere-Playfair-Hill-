#ifndef VIGENERE_CIPHER_H
#define VIGENERE_CIPHER_H

#include <string>

class VigenereCipher {
public:
    static std::string Encrypt(const std::string& text, const std::string& key);
    static std::string Decrypt(const std::string& text, const std::string& key);

private:
    static char ShiftChar(char c, char k, bool encrypt);
    static std::string GenerateKey(const std::string& text, const std::string& key);
};

#endif // VIGENERE_CIPHER_H

