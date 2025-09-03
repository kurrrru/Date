#pragma once

#include <string>
#include <sstream>

namespace toolbox {

std::string to_string(int value) {
    std::ostringstream oss;
    oss << value;
    return oss.str();
}

}  // namespace toolbox
