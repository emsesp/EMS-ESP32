
#ifndef WSTRING_H
#define WSTRING_H

#include <string>

// Reproduces Arduino's String class
class String {
  public:
    String & operator+=(const char * rhs) {
        _str += rhs;
        return *this;
    }

    size_t length() const {
        return _str.size();
    }

    String(const char * str = "")
        : _str(str) {
    }

    const char * c_str() const {
        return _str.c_str();
    }

    bool operator==(const char * s) const {
        return _str == s;
    }

    friend std::ostream & operator<<(std::ostream & lhs, const ::String & rhs) {
        lhs << rhs._str;
        return lhs;
    }

    bool isEmpty() {
        return _str.empty();
    }

    long toInt() const {
        return std::stol(_str);
    }

    bool equals(const char * s) {
        return _str == s;
    }


  private:
    std::string _str;
};

class StringSumHelper;

inline bool operator==(const std::string & lhs, const ::String & rhs) {
    return lhs == rhs.c_str();
}


size_t strlcpy(char * __restrict dst, const char * __restrict src, size_t dsize);
size_t strlcat(char * dst, const char * src, size_t siz);

#define strlen_P strlen
#define strncpy_P strncpy
#define strcmp_P strcmp
#define strcpy_P strcpy

#endif
