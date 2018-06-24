#include "util.h"
#include <algorithm>

std::vector<std::string> Split(const std::string& s, char delim) {
    std::vector<std::string> v;
    auto i   = 0;
    auto pos = s.find(delim);

    while (pos != std::string::npos) {
        v.push_back(s.substr(i, pos - i));
        i   = ++pos;
        pos = s.find(delim, pos);

        if (pos == std::string::npos) v.push_back(s.substr(i, s.length()));
    }
    return v;
}

std::string Join(const std::vector<std::string>& elements,
                 const char* const separator) {
    switch (elements.size()) {
        case 0:
            return "";
        case 1:
            return elements[0];
        default:
            std::ostringstream os;
            std::copy(elements.begin(), elements.end() - 1,
                      std::ostream_iterator<std::string>(os, separator));
            os << *elements.rbegin();
            return os.str();
    }
}

std::string StringToHex(const std::string& input) {
    static const char* const lut = "0123456789ABCDEF";
    size_t len                   = input.length();

    std::string output;
    output.reserve(2 * len);
    for (size_t i = 0; i < len; ++i) {
        const unsigned char c = input[i];
        output.push_back(lut[c >> 4]);
        output.push_back(lut[c & 15]);
    }
    return output;
}

bool HexToString(const std::string& input, std::string* output) {
    static const char* const lut = "0123456789ABCDEF";
    size_t len                   = input.length();
    if (len & 1) return false;

    output->reserve(len / 2);
    for (size_t i = 0; i < len; i += 2) {
        char a        = input[i];
        const char* p = std::lower_bound(lut, lut + 16, a);
        if (*p != a) return false;

        char b        = input[i + 1];
        const char* q = std::lower_bound(lut, lut + 16, b);
        if (*q != b) return false;

        output->push_back(((p - lut) << 4) | (q - lut));
    }
    return true;
}
