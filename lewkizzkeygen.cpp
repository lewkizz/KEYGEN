#include <iostream>
#include <string>
#include <random>
#include <chrono>
#include <openssl/sha.h>
#include <openssl/hmac.h>

std::string generateSupaBaseKey() {
    std::random_device rd;
    std::mt19937_64 generator(rd());
    std::uniform_int_distribution<int> dist(0, 63);
    const std::string charset = "ABCDEFGHIJKLMNOPQRSTUVWXYZabcdefghijklmnopqrstuvwxyz0123456789+×÷=/_<>[]!@#£%^&*()-':;,?`~{}€$¥₩°•○●□■♤♡◇♧☆▪︎¤《》¡¿";
    auto nanos = std::chrono::duration_cast<std::chrono::nanoseconds>(
        std::chrono::high_resolution_clock::now().time_since_epoch()).count();
    std::string timeSeed = std::to_string(nanos).substr(0, 8);
    std::string key;
    key.reserve(32);
    for (int i = 0; i < 24; i++) key += charset[dist(generator) % charset.length()];
    key.insert(12, timeSeed);
    return key;
}

std::string sha512Hash(const std::string& input) {
    unsigned char hash[SHA512_DIGEST_LENGTH];
    SHA512(reinterpret_cast<const unsigned char*>(input.c_str()), input.length(), hash);
    std::string output;
    output.reserve(2 * SHA512_DIGEST_LENGTH);
    for (int i = 0; i < SHA512_DIGEST_LENGTH; i++) {
        char buf[3];
        sprintf(buf, "%02x", hash[i]);
        output += buf;
    }
    return output; // 512 bits
}

std::string hmacSha512(const std::string& key, const std::string& data) {
    unsigned char* result;
    unsigned int len = SHA512_DIGEST_LENGTH;
    result = HMAC(EVP_sha512(), key.c_str(), key.length(), 
                  reinterpret_cast<const unsigned char*>(data.c_str()), data.length(), nullptr, nullptr);
    std::string output;
    output.reserve(2 * len);
    for (unsigned int i = 0; i < len; i++) {
        char buf[3];
        sprintf(buf, "%02x", result[i]);
        output += buf;
    }
    return output; // 512 bits
}

std::string toSecureFormat(const std::string& hexKey) {
    const std::string secureKeywords = "hiMYiLOveheiNEkenANDMONstRnrGEEhaLmAO95420";
    std::string secure;
    secure.reserve(512); // Double length for extra madness
    for (size_t i = 0; i < hexKey.length(); i += 2) {
        int byte = std::stoi(hexKey.substr(i, 2), nullptr, 16);
        secure += secureKeywords[byte % secureKeywords.length()];
        secure += secureKeywords[(byte + 13) % secureKeywords.length()]; // Double up for chaos
    }
    return secure;
}

std::string generate1024BitSecureSupaKey() {
    std::string baseKey = generateSupaBaseKey();
    std::string hash1 = sha512Hash(baseKey + "HEINEKENWSFXCA383I");
    std::string hash2 = hmacSha512(baseKey + "LEWKIZZHDKEIXN82749YHWH", hash1);
    std::string combined = hash1 + hash2; // 256 hex chars = 1024 bits
    return toSecureFormat(combined);
}

int main() {
    std::string superSecureKey = generate1024BitSecureSupaKey();
    std::cout << "Yourkey: " << superSecureKey << std::endl;
    std::cout << "Length: " << superSecureKey.length() << std::endl;
    return 0;
}