#include <Date.hpp>
#include <calender_system/GregorianCalendar.hpp>
#include <calender_system/NonProlepticGregorianCalendar.hpp>
#include <calender_system/JulianCalendar.hpp>

#include <iostream>
#include <iomanip>
#include <string>
#include <sstream>

void test_parse_gregorian(const std::string& date_str, const char* format, bool strict, bool expect_success) {
    static int test_num = 0;
    try {
        toolbox::Date date(toolbox::GREGORIAN, date_str, format, strict);
        std::cout << "gregorian " << std::setw(3) << ++test_num << ": ";
        if (expect_success) {
            std::cout << "OK" << std::endl;
        } else {
            std::cout << "NG" << std::endl;
        }
    } catch (const std::exception& e) {
        std::cout << "gregorian " << std::setw(3) << ++test_num << ": ";
        if (!expect_success) {
            std::cout << "OK" << std::endl;
            return;
        }
        std::cout << "NG" << std::endl;
        std::cout << "Error: " << e.what() << std::endl;
    }
}

void test_parse_non_proleptic_gregorian(const std::string& date_str, const char* format, bool strict, bool expect_success) {
    static int test_num = 0;
    try {
        toolbox::Date date(toolbox::NON_PROLEPTIC_GREGORIAN, date_str, format, strict);
        std::cout << "non-proleptic " << std::setw(3) << ++test_num << ": ";
        if (expect_success) {
            std::cout << "OK" << std::endl;
        } else {
            std::cout << "NG" << std::endl;
        }
    } catch (const std::exception& e) {
        std::cout << "non-proleptic " << std::setw(3) << ++test_num << ": ";
        if (!expect_success) {
            std::cout << "OK" << std::endl;
            return;
        }
        std::cout << "NG" << std::endl;
        std::cout << "Error: " << e.what() << std::endl;
    }
}

int main() {
    toolbox::Date date;
    test_parse_gregorian("2024-02-09", "%Y-%m-%d", true, true);
    test_parse_gregorian("2024/2/9", "%Y/%M/%D", true, true);
    test_parse_gregorian("B.C.24/02/09", "%E%y/%m/%d", true, true);
    test_parse_gregorian("BC24/02/09", "%e%y/%m/%d", true, true);
    test_parse_gregorian("", "", true, false);
    test_parse_gregorian("2024-02", "%Y-%m", true, false);
    test_parse_gregorian("2024-02", "%Y-%d", true, false);
    test_parse_gregorian("2024-1111", "%Y-%M1%D", true, false);
    test_parse_gregorian("2024-1111", "%Y-%M1%D", false, true);
    test_parse_gregorian("2024-2222", "%Y-%M2%D", true, true);
    test_parse_gregorian("2024-2222", "%Y-%M2%D", false, true);
    test_parse_gregorian("2024-12-12-02", "%Y-%M-%m-%d", false, false);
    test_parse_gregorian("111", "%Y%M%D", true, true);
    test_parse_gregorian("1111", "%Y%M1%D", true, true);
    test_parse_gregorian("11111", "%Y%M1%D", true, false);
    test_parse_gregorian("1011", "%Y%M%D", true, true);
    test_parse_gregorian("1011", "%Y%m%D", true, true);
    test_parse_gregorian("10101", "%Y%m%D", true, true);
    test_parse_gregorian("10101", "%Y%M%D", true, true);
    test_parse_gregorian("010101", "%Y%M%D", true, false);

    test_parse_non_proleptic_gregorian("2024-02-09", "%Y-%m-%d", true, true);
    test_parse_non_proleptic_gregorian("2024/2/9", "%Y/%M/%D", true, true);
    test_parse_non_proleptic_gregorian("B.C.24/02/09", "%E%y/%m/%d", true, false);
    test_parse_non_proleptic_gregorian("BC24/02/09", "%e%y/%m/%d", true, false);
    test_parse_non_proleptic_gregorian("", "", true, false);
    test_parse_non_proleptic_gregorian("2024-02", "%Y-%m", true, false);
    test_parse_non_proleptic_gregorian("2024-02", "%Y-%d", true, false);
    test_parse_non_proleptic_gregorian("2024-1111", "%Y-%M1%D", true, false);
    test_parse_non_proleptic_gregorian("2024-1111", "%Y-%M1%D", false, true);
    test_parse_non_proleptic_gregorian("2024-2222", "%Y-%M2%D", true, true);
    test_parse_non_proleptic_gregorian("2024-2222", "%Y-%M2%D", false, true);
    test_parse_non_proleptic_gregorian("2024-12-12-02", "%Y-%M-%m-%d", false, false);
    test_parse_non_proleptic_gregorian("111", "%Y%M%D", true, false);
    test_parse_non_proleptic_gregorian("1111", "%Y%M1%D", true, false);
    test_parse_non_proleptic_gregorian("11111", "%Y%M1%D", true, false);
    test_parse_non_proleptic_gregorian("1582-114", "%Y-%M%D", true, true);
    test_parse_non_proleptic_gregorian("1582114", "%Y%M%D", true, false);
    test_parse_non_proleptic_gregorian("1582/10/14", "%Y/%M/%D", true, false);
    test_parse_non_proleptic_gregorian("1582/10/15", "%Y/%M/%D", true, true);

    date = toolbox::Date(toolbox::GREGORIAN, toolbox::GregorianCalendar::BC, 46, 12, 30);
    std::cout << date.to_string(toolbox::GREGORIAN, "%E%y-%m-%d") << std::endl;
    std::cout << date.get_raw_date() << std::endl;

    date = toolbox::Date(toolbox::JULIAN, toolbox::JulianCalendar::AD, 1582, 10, 4);
    std::cout << date.to_string(toolbox::GREGORIAN, "%E%Y-%m-%d") << std::endl;
    std::cout << date.to_string(toolbox::JULIAN, "%E%Y-%m-%d") << std::endl;
    std::cout << date.get_raw_date() << std::endl;

    date = toolbox::Date(toolbox::GREGORIAN, toolbox::GregorianCalendar::AD, 1582, 10, 15);
    std::cout << date.to_string(toolbox::GREGORIAN, "%E%Y-%m-%d") << std::endl;
    std::cout << date.to_string(toolbox::JULIAN, "%E%Y-%m-%d") << std::endl;
    std::cout << date.get_raw_date() << std::endl;
    
    date = toolbox::Date(toolbox::JULIAN, toolbox::JulianCalendar::BC, 45, 1, 1);
    std::cout << date.to_string(toolbox::GREGORIAN, "%E%Y-%m-%d") << std::endl;
    std::cout << date.to_string(toolbox::JULIAN, "%E%Y-%m-%d") << std::endl;
    std::cout << date.get_raw_date() << std::endl;

    date = toolbox::Date::today();
    std::cout << "Today: " << date.to_string(toolbox::GREGORIAN, "%Y-%m-%d") << std::endl;
    std::cout << "Today: " << date.to_string(toolbox::JULIAN, "%Y-%m-%d") << std::endl;

}
