#include "VigenereCipher.h"

std::string VigenereCipher::Encrypt(const std::string& text, const std::string& key) {
    std::string newKey = GenerateKey(text, key);
    std::string cipherText = "";

    for (size_t i = 0, j = 0; i < text.length(); ++i) {
        char c = text[i];
        if (isalpha(c)) {
            cipherText += ShiftChar(c, newKey[j], true);
            j++;  // Hanya tambah j jika c adalah huruf
        }
        else {
            cipherText += c;  // Simpan karakter non-huruf tanpa perubahan
        }
    }

    return cipherText;
}

std::string VigenereCipher::Decrypt(const std::string& text, const std::string& key) {
    std::string newKey = GenerateKey(text, key);
    std::string plainText = "";

    for (size_t i = 0, j = 0; i < text.length(); ++i) {
        char c = text[i];
        if (isalpha(c)) {
            plainText += ShiftChar(c, newKey[j], false);
            j++;  // Hanya tambah j jika c adalah huruf
        }
        else {
            plainText += c;  // Simpan karakter non-huruf tanpa perubahan
        }
    }

    return plainText;
}

char VigenereCipher::ShiftChar(char c, char k, bool encrypt) {
    int shift;

    if (isupper(c)) {
        k = toupper(k);  // Pastikan kunci tetap huruf kapital jika c adalah huruf besar
        shift = (encrypt) ? (k - 'A') : ('A' - k);
        return (char)((((c - 'A' + shift + 26) % 26) + 'A'));
    }
    else if (islower(c)) {
        k = tolower(k);  // Pastikan kunci menjadi huruf kecil jika c adalah huruf kecil
        shift = (encrypt) ? (k - 'a') : ('a' - k);
        return (char)((((c - 'a' + shift + 26) % 26) + 'a'));
    }

    return c;  // Untuk karakter non-alfabetik
}

std::string VigenereCipher::GenerateKey(const std::string& text, const std::string& key) {
    std::string newKey = key;
    while (newKey.length() < text.length()) {
        newKey += key;
    }
    return newKey.substr(0, text.length());
}