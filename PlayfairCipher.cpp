#include "PlayfairCipher.h"
#include <unordered_set>
#include <algorithm>

std::string PlayfairCipher::Encrypt(const std::string& text, const std::string& key) {
    std::vector<std::vector<char>> keySquare = GenerateKeySquare(key);
    std::string formattedText = FormatText(text);
    std::string cipherText = "";

    for (size_t i = 0; i < formattedText.length(); i += 2) {
        char first = formattedText[i];
        char second = formattedText[i + 1];

        std::pair<int, int> pos1 = FindPosition(keySquare, toupper(first));
        std::pair<int, int> pos2 = FindPosition(keySquare, toupper(second));

        // Enkripsi dan simpan casing asli
        std::string encryptedPair = EncryptDecryptPair(pos1, pos2, keySquare, true);
        cipherText += (isupper(first) ? toupper(encryptedPair[0]) : tolower(encryptedPair[0]));
        cipherText += (isupper(second) ? toupper(encryptedPair[1]) : tolower(encryptedPair[1]));
    }

    return cipherText;
}

std::string PlayfairCipher::Decrypt(const std::string& text, const std::string& key) {
    std::vector<std::vector<char>> keySquare = GenerateKeySquare(key);
    std::string plainText = "";

    for (size_t i = 0; i < text.length(); i += 2) {
        char first = text[i];
        char second = text[i + 1];

        std::pair<int, int> pos1 = FindPosition(keySquare, toupper(first));
        std::pair<int, int> pos2 = FindPosition(keySquare, toupper(second));

        // Dekripsi dan simpan casing asli
        std::string decryptedPair = EncryptDecryptPair(pos1, pos2, keySquare, false);
        plainText += (isupper(first) ? toupper(decryptedPair[0]) : tolower(decryptedPair[0]));
        plainText += (isupper(second) ? toupper(decryptedPair[1]) : tolower(decryptedPair[1]));
    }

    return plainText;
}

std::vector<std::vector<char>> PlayfairCipher::GenerateKeySquare(const std::string& key) {
    std::unordered_set<char> used;
    std::vector<std::vector<char>> keySquare(5, std::vector<char>(5));
    int row = 0, col = 0;

    for (char k : key) {
        if (k == 'J') k = 'I'; // J is treated as I
        char upperK = toupper(k); // Convert to uppercase for key square consistency
        if (used.find(upperK) == used.end()) {
            keySquare[row][col++] = upperK;
            used.insert(upperK);
            if (col == 5) { row++; col = 0; }
        }
    }

    for (char ch = 'A'; ch <= 'Z'; ++ch) {
        if (ch == 'J') continue; // Skip J
        if (used.find(ch) == used.end()) {
            keySquare[row][col++] = ch;
            used.insert(ch);
            if (col == 5) { row++; col = 0; }
        }
    }

    return keySquare;
}

std::string PlayfairCipher::FormatText(const std::string& text) {
    std::string formatted = "";
    for (char c : text) {
        if (isalpha(c)) {
            formatted += c; // Preserve original casing
        }
        else if (isspace(c)) {
            // Mengabaikan spasi
            continue;
        }
    }

    // Tambahkan 'X' jika panjang teks ganjil
    if (formatted.length() % 2 != 0) formatted += 'X';

    return formatted;
}

std::pair<int, int> PlayfairCipher::FindPosition(const std::vector<std::vector<char>>& keySquare, char c) {
    for (int row = 0; row < 5; ++row) {
        for (int col = 0; col < 5; ++col) {
            if (keySquare[row][col] == toupper(c)) return { row, col };
        }
    }
    return { -1, -1 }; // Should never happen if input is valid
}

std::string PlayfairCipher::EncryptDecryptPair(const std::pair<int, int>& pos1, const std::pair<int, int>& pos2, const std::vector<std::vector<char>>& keySquare, bool encrypt) {
    std::string result = "";
    if (pos1.first == pos2.first) {
        int shift = (encrypt) ? 1 : -1;
        result += keySquare[pos1.first][(pos1.second + shift + 5) % 5];
        result += keySquare[pos2.first][(pos2.second + shift + 5) % 5];
    }
    else if (pos1.second == pos2.second) {
        int shift = (encrypt) ? 1 : -1;
        result += keySquare[(pos1.first + shift + 5) % 5][pos1.second];
        result += keySquare[(pos2.first + shift + 5) % 5][pos2.second];
    }
    else {
        result += keySquare[pos1.first][pos2.second];
        result += keySquare[pos2.first][pos1.second];
    }
    return result;
}
