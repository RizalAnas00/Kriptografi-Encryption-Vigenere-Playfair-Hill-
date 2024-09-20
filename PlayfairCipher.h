#ifndef PLAYFAIR_CIPHER_H
#define PLAYFAIR_CIPHER_H

#include <string>
#include <vector>

class PlayfairCipher {
public:
    static std::string Encrypt(const std::string& text, const std::string& key);
    static std::string Decrypt(const std::string& text, const std::string& key);

private:
    static std::vector<std::vector<char>> GenerateKeySquare(const std::string& key);
    static std::string FormatText(const std::string& text);
    static std::pair<int, int> FindPosition(const std::vector<std::vector<char>>& keySquare, char c);
    static std::string EncryptDecryptPair(const std::pair<int, int>& pos1, const std::pair<int, int>& pos2, const std::vector<std::vector<char>>& keySquare, bool encrypt);
    static std::string DecryptPair(const std::pair<int, int>& pos1, const std::pair<int, int>& pos2, const std::vector<std::vector<char>>& keySquare);
};

#endif // PLAYFAIR_CIPHER_H
