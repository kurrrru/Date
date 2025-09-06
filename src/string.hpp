#pragma once

#include <string>
#include <sstream>

namespace toolbox {

inline std::string to_string(int value) {
    std::ostringstream oss;
    oss << value;
    return oss.str();
}

inline int stoi(const std::string &s) {
    std::stringstream ss(s);
    int num;
    ss >> num;
    if (ss.fail() || !ss.eof()) {
        throw std::invalid_argument("Invalid integer: '" + s + "'");
    }
    return num;
}

}  // namespace toolbox
