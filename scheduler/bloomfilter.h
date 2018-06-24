#ifndef _BLOOMFILTER_H_
#define _BLOOMFILTER_H_
#include <glog/logging.h>
#include <math.h>
#include <cstdint>
#include <functional>
#include <memory>
#include <string>
#include <vector>
#include "proto/spider.pb.h"

class BloomFilter {
public:
    BloomFilter(const int32_t n, const double false_positive_p);
    void Insert(const std::string &key);
    bool KeyMatch(const std::string &key);

private:
    std::vector<char> m_bits;
    int32_t m_k;
    int32_t m_m;
    int32_t m_n;
    double m_p;
    std::hash<std::string> m_hasher{};
};

BloomFilter::BloomFilter(const int32_t n, const double false_positive_p)
    : m_bits(), m_k(0), m_m(0), m_n(n), m_p(false_positive_p) {
    m_k = static_cast<int32_t>(-std::log(m_p) / std::log(2));
    m_m = static_cast<int32_t>(m_k * n * 1.0 / std::log(2));
    m_bits.resize((m_m + 7) / 8, 0);
}

void BloomFilter::Insert(const std::string &key) {
    uint32_t hash_val    = m_hasher(key);
    const uint32_t delta = (hash_val >> 17) | (hash_val << 15);
    for (int i = 0; i < m_k; ++i) {
        const uint32_t bit_pos = hash_val % m_m;
        m_bits[bit_pos / 8] |= 1 << (bit_pos % 8);
        hash_val += delta;
    }
}

bool BloomFilter::KeyMatch(const std::string &key) {
    uint32_t hash_val    = m_hasher(key);
    const uint32_t delta = (hash_val >> 17) | (hash_val << 15);
    for (int i = 0; i < m_k; ++i) {
        const uint32_t bit_pos = hash_val % m_m;
        if ((m_bits[bit_pos / 8] & (1 << (bit_pos % 8))) == 0) {
            return false;
        }
        hash_val += delta;
    }
    return true;
}

// LOG(INFO) << "original hash_val:" << hash_val;
// LOG(INFO) << "hashval:" << hash_val;
// LOG(INFO) << "bit:" << bit_pos / 8;
// LOG(INFO) << "key" << key << ":  hash_val" << hash_val;
#endif  // _BLOOMFILTER_H_
