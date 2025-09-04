#include <Date.hpp>
#include <calender_system/GregorianCalendar.hpp>

#include <iostream>
#include <string>
#include <sstream>

void test_parse(const std::string& date_str, const char* format, bool strict, bool expect_success) {
    try {
        toolbox::Date date(toolbox::GREGORIAN, date_str, format, strict);
        if (expect_success) {
            std::cout << "OK" << std::endl;
        } else {
            std::cout << "NG" << std::endl;
        }
    } catch (const std::exception& e) {
        if (!expect_success) {
            std::cout << "OK" << std::endl;
            return;
        }
        std::cout << "Error: " << e.what() << std::endl;
        std::cout << "NG" << std::endl;
    }
}

int main() {
    toolbox::Date date;
    test_parse("2024-02-09", "%Y-%m-%d", true, true);
    test_parse("2024/2/9", "%Y/%M/%D", true, true);
}
