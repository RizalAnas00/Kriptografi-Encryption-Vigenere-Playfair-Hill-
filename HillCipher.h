#ifndef HILLCIPHER_H
#define HILLCIPHER_H

#include <vector>
#include <string>

class HillCipher {
public:
    static std::string Encrypt(const std::string& plaintext, const std::string& key);
    static std::string Decrypt(const std::string& ciphertext, const std::string& key);
private:
    static int mod(int a, int b);
    static std::vector<std::vector<int>> inverseMatrix(const std::vector<std::vector<int>>& matrix, int mod);
    static int determinant(const std::vector<std::vector<int>>& matrix);
    static int findModInverse(int a, int m);
    static std::vector<std::vector<int>> convertKeyToNumbers(const std::string& key, int matrixSize);
};

#endif // HILLCIPHER_H
