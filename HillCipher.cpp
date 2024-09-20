#include "HillCipher.h"
#include <stdexcept>
#include <iostream>
#include <vector>
#include <cmath>

// Function to convert letters to numbers (A=0, B=1, ..., Z=25)
std::vector<std::vector<int>> HillCipher::convertKeyToNumbers(const std::string& key, int matrixSize) {
    int targetLength = matrixSize * matrixSize;

    std::string paddedKey = key;

    // Pad the key with random characters if it's too short
    while (paddedKey.size() < targetLength) {
        paddedKey += 'A' + (rand() % 26); // Add random uppercase letters
    }

    std::vector<std::vector<int>> keyMatrix(matrixSize, std::vector<int>(matrixSize));

    // Fill the matrix with key values (case insensitive but preserve casing)
    for (int i = 0; i < matrixSize; ++i) {
        for (int j = 0; j < matrixSize; ++j) {
            char c = paddedKey[i * matrixSize + j];

            if (std::isupper(c)) {
                keyMatrix[i][j] = c - 'A';  // A=0, B=1, ..., Z=25
            }
            else if (std::islower(c)) {
                keyMatrix[i][j] = c - 'a';  // a=0, b=1, ..., z=25
            }
            else {
                throw std::invalid_argument("Key must contain only letters.");
            }
        }
    }

    return keyMatrix;
}

// Encrypt function
std::string HillCipher::Encrypt(const std::string& plaintext, const std::string& key) {
    int matrixSize;
    if (key.size() >= 9) {
        matrixSize = 3; // 3x3 matrix
    }
    else if (key.size() >= 4) {
        matrixSize = 2; // 2x2 matrix
    }
    else {
        throw std::invalid_argument("Key length must be at least 4 characters.");
    }

    std::vector<std::vector<int>> keyMatrix = convertKeyToNumbers(key, matrixSize);
    std::string ciphertext;
    std::vector<int> paddedText;

    // Convert plaintext to numbers and preserve case
    for (char c : plaintext) {
        if (std::isupper(c)) {
            paddedText.push_back(c - 'A');  // A=0, B=1, ..., Z=25
        }
        else if (std::islower(c)) {
            paddedText.push_back(c - 'a');  // a=0, b=1, ..., z=25
        }
        else {
            throw std::invalid_argument("Plaintext must contain only letters.");
        }
    }

    // Ensure paddedText size is a multiple of matrixSize (padding with 'A' or 'a')
    while (paddedText.size() % matrixSize != 0) {
        paddedText.push_back(0); // Padding with 'A' (uppercase by default)
    }

    // Encrypt
    for (size_t i = 0; i < paddedText.size(); i += matrixSize) {
        for (int j = 0; j < matrixSize; ++j) {
            int sum = 0;
            for (int k = 0; k < matrixSize; ++k) {
                sum += keyMatrix[j][k] * paddedText[i + k];
            }
            ciphertext += (char)(mod(sum, 26) + 'A'); // Convert back to letters (uppercase for simplicity)
        }
    }
    return ciphertext;
}


std::string HillCipher::Decrypt(const std::string& ciphertext, const std::string& key) {
    int matrixSize;
    if (key.size() >= 9) {
        matrixSize = 3; // 3x3 matrix
    }
    else if (key.size() >= 4) {
        matrixSize = 2; // 2x2 matrix
    }
    else {
        throw std::invalid_argument("Key length must be at least 4 characters.");
    }

    std::vector<std::vector<int>> keyMatrix = convertKeyToNumbers(key, matrixSize);
    std::vector<std::vector<int>> invKeyMatrix = inverseMatrix(keyMatrix, 26);
    if (invKeyMatrix.empty()) {
        throw std::runtime_error("Inverse key matrix does not exist.");
    }

    std::string plaintext;
    std::vector<int> paddedText;

    // Convert ciphertext to numbers and preserve case
    for (char c : ciphertext) {
        if (std::isupper(c)) {
            paddedText.push_back(c - 'A');  // A=0, B=1, ..., Z=25
        }
        else if (std::islower(c)) {
            paddedText.push_back(c - 'a');  // a=0, b=1, ..., z=25
        }
        else {
            throw std::invalid_argument("Ciphertext must contain only letters.");
        }
    }

    // Decrypt
    for (size_t i = 0; i < paddedText.size(); i += matrixSize) {
        for (int j = 0; j < matrixSize; ++j) {
            int sum = 0;
            for (int k = 0; k < matrixSize; ++k) {
                sum += invKeyMatrix[j][k] * paddedText[i + k];
            }
            plaintext += (char)(mod(sum, 26) + 'A'); // Convert back to letters (uppercase for simplicity)
        }
    }
    return plaintext;
}

// Mod function
int HillCipher::mod(int a, int b) {
    return (a % b + b) % b; // Ensure non-negative
}

// Determinant function
int HillCipher::determinant(const std::vector<std::vector<int>>& matrix) {
    int n = matrix.size();
    if (n == 1) {
        return matrix[0][0];
    }

    int det = 0;
    for (int p = 0; p < n; p++) {
        std::vector<std::vector<int>> submatrix(n - 1, std::vector<int>(n - 1));
        for (int i = 1; i < n; i++) {
            for (int j = 0; j < n; j++) {
                if (j < p) {
                    submatrix[i - 1][j] = matrix[i][j];
                }
                else if (j > p) {
                    submatrix[i - 1][j - 1] = matrix[i][j];
                }
            }
        }
        det += (p % 2 == 0 ? 1 : -1) * matrix[0][p] * determinant(submatrix);
    }
    return mod(det, 26);
}

// Find modular inverse
int HillCipher::findModInverse(int a, int m) {
    a = mod(a, m);
    for (int x = 1; x < m; x++) {
        if (mod(a * x, m) == 1) {
            return x;
        }
    }
    return -1; // No modular inverse exists
}

// Inverse matrix
std::vector<std::vector<int>> HillCipher::inverseMatrix(const std::vector<std::vector<int>>& matrix, int mod) {
    int n = matrix.size();
    int det = determinant(matrix);
    int invDet = findModInverse(det, mod);

    if (invDet == -1) return {}; // No inverse exists

    std::vector<std::vector<int>> adjugate(n, std::vector<int>(n));

    for (int i = 0; i < n; i++) {
        for (int j = 0; j < n; j++) {
            std::vector<std::vector<int>> submatrix(n - 1, std::vector<int>(n - 1));
            for (int row = 0; row < n; row++) {
                for (int col = 0; col < n; col++) {
                    if (row != i && col != j) {
                        submatrix[row < i ? row : row - 1][col < j ? col : col - 1] = matrix[row][col];
                    }
                }
            }
            int sign = (i + j) % 2 == 0 ? 1 : -1;
            adjugate[j][i] = HillCipher::mod((determinant(submatrix) * sign * invDet), mod);
        }
    }
    return adjugate;
}
