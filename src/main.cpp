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
    test_parse("B.C.24/02/09", "%E%y/%m/%d", true, true);
    test_parse("BC24/02/09", "%e%y/%m/%d", true, true);
    test_parse("", "", true, false);
    test_parse("2024-02", "%Y-%m", true, false);
    test_parse("2024-02", "%Y-%d", true, false);
    test_parse("2024-1111", "%Y-%M1%D", true, false);
    test_parse("2024-1111", "%Y-%M1%D", false, true);
    test_parse("2024-2222", "%Y-%M2%D", true, true);
    test_parse("2024-2222", "%Y-%M2%D", false, true);
    test_parse("2024-12-12-02", "%Y-%M-%m-%d", false, false);
    test_parse("111", "%Y%M%D", true, true);
    test_parse("1111", "%Y%M1%D", true, true);
    test_parse("11111", "%Y%M1%D", true, false);
    test_parse("1011", "%Y%M%D", true, true);
    test_parse("1011", "%Y%m%D", true, true);
    test_parse("10101", "%Y%m%D", true, true);
    test_parse("10101", "%Y%M%D", true, true);
    test_parse("010101", "%Y%M%D", true, false);

}
