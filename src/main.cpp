#include <iostream>
#include <iomanip>
#include <string>
#include <sstream>

#include <Date.hpp>
#include <calendar_system/GregorianCalendar.hpp>
#include <calendar_system/NonProlepticGregorianCalendar.hpp>
#include <calendar_system/JulianCalendar.hpp>
#include <calendar_system/EthiopianCalendar.hpp>
#include <calendar_system/FrenchRepublicanCalendar.hpp>

void test_parse_gregorian(const std::string& date_str,
                          const char* format,
                          bool strict,
                          bool expect_success) {
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

void test_parse_non_proleptic_gregorian(const std::string& date_str,
                                        const char* format,
                                        bool strict,
                                        bool expect_success) {
    static int test_num = 0;
    try {
        toolbox::Date date(toolbox::NON_PROLEPTIC_GREGORIAN,
                           date_str,
                           format,
                           strict);
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
    struct ParseCase {
        const char* date;
        const char* format;
        bool strict;
        bool expect;
    };

    const ParseCase greg_cases[] = {
        {"2024-02-09", "%Y-%m-%d", true, true},
        {"2024/2/9", "%Y/%M/%D", true, true},
        {"B.C.24/02/09", "%E%y/%m/%d", true, true},
        {"BC24/02/09", "%e%y/%m/%d", true, true},
        {"", "", true, false},
        {"2024-02", "%Y-%m", true, false},
        {"2024-02", "%Y-%d", true, false},
        {"2024-1111", "%Y-%M1%D", true, false},
        {"2024-1111", "%Y-%M1%D", false, true},
        {"2024-2222", "%Y-%M2%D", true, true},
        {"2024-2222", "%Y-%M2%D", false, true},
        {"2024-12-12-02", "%Y-%M-%m-%d", false, false},
        {"111", "%Y%M%D", true, true},
        {"1111", "%Y%M1%D", true, true},
        {"11111", "%Y%M1%D", true, false},
        {"1011", "%Y%M%D", true, true},
        {"1011", "%Y%m%D", true, true},
        {"10101", "%Y%m%D", true, true},
        {"10101", "%Y%M%D", true, true},
        {"010101", "%Y%M%D", true, false}
    };

    const std::size_t greg_case_count =
        sizeof(greg_cases) / sizeof(greg_cases[0]);
    for (std::size_t i = 0; i < greg_case_count; ++i) {
        test_parse_gregorian(greg_cases[i].date,
                             greg_cases[i].format,
                             greg_cases[i].strict,
                             greg_cases[i].expect);
    }

    const ParseCase non_proleptic_cases[] = {
        {"2024-02-09", "%Y-%m-%d", true, true},
        {"2024/2/9", "%Y/%M/%D", true, true},
        {"B.C.24/02/09", "%E%y/%m/%d", true, false},
        {"BC24/02/09", "%e%y/%m/%d", true, false},
        {"", "", true, false},
        {"2024-02", "%Y-%m", true, false},
        {"2024-02", "%Y-%d", true, false},
        {"2024-1111", "%Y-%M1%D", true, false},
        {"2024-1111", "%Y-%M1%D", false, true},
        {"2024-2222", "%Y-%M2%D", true, true},
        {"2024-2222", "%Y-%M2%D", false, true},
        {"2024-12-12-02", "%Y-%M-%m-%d", false, false},
        {"111", "%Y%M%D", true, false},
        {"1111", "%Y%M1%D", true, false},
        {"11111", "%Y%M1%D", true, false},
        {"1582-114", "%Y-%M%D", true, true},
        {"1582114", "%Y%M%D", true, false},
        {"1582/10/14", "%Y/%M/%D", true, false},
        {"1582/10/15", "%Y/%M/%D", true, true},
        {"1582/10/%D15", "%Y/%M/%D%D", true, false},
        {"1582/10/%D15", "%Y/%M/%D%D", false, false}
    };

    const std::size_t non_proleptic_case_count =
        sizeof(non_proleptic_cases) / sizeof(non_proleptic_cases[0]);
    for (std::size_t i = 0; i < non_proleptic_case_count; ++i) {
        test_parse_non_proleptic_gregorian(non_proleptic_cases[i].date,
                                           non_proleptic_cases[i].format,
                                           non_proleptic_cases[i].strict,
                                           non_proleptic_cases[i].expect);
    }

    date = toolbox::Date(toolbox::GREGORIAN,
                         toolbox::GregorianCalendar::BC,
                         46,
                         12,
                         30);
    std::cout << date.to_string(toolbox::GREGORIAN, "%E%y-%m-%d")
              << std::endl;
    std::cout << date.get_raw_date() << std::endl;

    date = toolbox::Date(toolbox::JULIAN,
                         toolbox::JulianCalendar::AD,
                         1582,
                         10,
                         4);
    std::cout << date.to_string(toolbox::GREGORIAN, "%E%Y-%m-%d")
              << std::endl;
    std::cout << date.to_string(toolbox::JULIAN, "%E%Y-%m-%d")
              << std::endl;
    std::cout << date.get_raw_date() << std::endl;

    date = toolbox::Date(toolbox::GREGORIAN,
                         toolbox::GregorianCalendar::AD,
                         1582,
                         10,
                         15);
    std::cout << date.to_string(toolbox::GREGORIAN, "%E%Y-%m-%d")
              << std::endl;
    std::cout << date.to_string(toolbox::JULIAN, "%E%Y-%m-%d")
              << std::endl;
    std::cout << date.get_raw_date() << std::endl;

    date = toolbox::Date(toolbox::JULIAN,
                         toolbox::JulianCalendar::BC,
                         45,
                         1,
                         1);
    std::cout << date.to_string(toolbox::GREGORIAN, "%E%Y-%m-%d")
              << std::endl;
    std::cout << date.to_string(toolbox::JULIAN, "%E%Y-%m-%d")
              << std::endl;
    std::cout << date.get_raw_date() << std::endl;

    date = toolbox::Date::today();
    std::cout << "Today: "
              << date.to_string(toolbox::GREGORIAN, "%Y-%m-%d")
              << std::endl;
    std::cout << "Today: "
              << date.to_string(toolbox::JULIAN, "%Y-%m-%d")
              << std::endl;
    std::cout << "Today: "
              << date.to_string(toolbox::ETHIOPIAN, "%Y-%M-%d")
              << std::endl;

    date = toolbox::Date(toolbox::JULIAN,
                         toolbox::JulianCalendar::BC,
                         45,
                         3,
                         1);
    std::cout << "BC 45-03-01 (Julian) = " << date.get_raw_date()
              << std::endl;
    date = toolbox::Date(toolbox::JULIAN,
                         toolbox::JulianCalendar::BC,
                         7,
                         3,
                         1);
    std::cout << "BC 7-03-01 (Julian) = " << date.get_raw_date()
              << std::endl;
    date = toolbox::Date(toolbox::JULIAN,
                         toolbox::JulianCalendar::AD,
                         6,
                         3,
                         1);
    std::cout << "AD 6-03-01 (Julian) = " << date.get_raw_date()
              << std::endl;

    date = toolbox::Date(toolbox::JULIAN,
                         toolbox::GregorianCalendar::AD,
                         2024,
                         8,
                         31);
    std::cout << date.to_string(toolbox::JULIAN, "%Y-%m-%d")
              << std::endl;
    date += 1;
    std::cout << date.to_string(toolbox::JULIAN, "%Y-%m-%d")
              << std::endl;
    date -= 1;

    // french republican calendar
    date = toolbox::Date(toolbox::FRENCH_REPUBLICAN,
                         toolbox::FrenchRepublicanCalendar::AD,
                         1,
                         1,
                         1);
    std::cout << date.to_string(toolbox::FRENCH_REPUBLICAN, "%Y-%m-%d")
              << std::endl;
    std::cout << date.to_string(toolbox::FRENCH_REPUBLICAN, "%Y-%M-%D")
              << std::endl;
    std::cout << date.to_string(toolbox::GREGORIAN, "%Y-%m-%d")
              << std::endl;
    date = toolbox::Date(toolbox::FRENCH_REPUBLICAN,
                         toolbox::FrenchRepublicanCalendar::AD,
                         14,
                         4,
                         10);
    std::cout << date.to_string(toolbox::FRENCH_REPUBLICAN, "%Y-%m-%d")
              << std::endl;
    std::cout << date.to_string(toolbox::FRENCH_REPUBLICAN, "%Y-%M-%D")
              << std::endl;
    std::cout << date.to_string(toolbox::GREGORIAN, "%Y-%m-%d")
              << std::endl;
    date = toolbox::Date(toolbox::FRENCH_REPUBLICAN,
                         toolbox::FrenchRepublicanCalendar::AD,
                         3,
                         13,
                         6);
    std::cout << date.to_string(toolbox::FRENCH_REPUBLICAN, "%Y-%m-%d")
              << std::endl;
    std::cout << date.to_string(toolbox::FRENCH_REPUBLICAN, "%Y-%M-%D")
              << std::endl;
    std::cout << date.to_string(toolbox::GREGORIAN, "%Y-%m-%d")
              << std::endl;
    date++;
    std::cout << date.to_string(toolbox::FRENCH_REPUBLICAN, "%Y-%m-%d")
              << std::endl;
    std::cout << date.to_string(toolbox::FRENCH_REPUBLICAN, "%Y-%M-%D")
              << std::endl;
    std::cout << date.to_string(toolbox::GREGORIAN, "%Y-%m-%d")
              << std::endl;
    date = toolbox::Date(toolbox::FRENCH_REPUBLICAN,
                         "Raisin 1, 11",
                         "%d %M, %Y",
                         true);
    std::cout << date.to_string(toolbox::FRENCH_REPUBLICAN, "%Y-%m-%d")
              << std::endl;
    std::cout << date.to_string(toolbox::FRENCH_REPUBLICAN, "%Y-%M-%D")
              << std::endl;
    std::cout << date.to_string(toolbox::GREGORIAN, "%Y-%m-%d")
              << std::endl;

    try {
        date = toolbox::Date(toolbox::FRENCH_REPUBLICAN,
                             "Raisin 2, 11",
                             "%d %M, %Y",
                             true);
        std::cout << "NG" << std::endl;
    } catch (const std::exception& e) {
        (void)e;
        std::cout << "OK" << std::endl;
    }
    date = toolbox::Date(toolbox::FRENCH_REPUBLICAN,
                         "Jour de la Revolution 13, 11",
                         "%d %M, %Y",
                         false);
    std::cout << date.to_string(toolbox::FRENCH_REPUBLICAN, "%Y-%m-%d")
              << std::endl;
    std::cout << date.to_string(toolbox::FRENCH_REPUBLICAN, "%Y-%M-%D")
              << std::endl;
    std::cout << date.to_string(toolbox::GREGORIAN, "%Y-%m-%d")
              << std::endl;
    try {
        date = toolbox::Date(toolbox::FRENCH_REPUBLICAN,
                             "Jour de la Revolution 13, 10",
                             "%d %M, %Y",
                             false);
        std::cout << "NG" << std::endl;
    } catch (const std::exception& e) {
        (void)e;
        std::cout << "OK" << std::endl;
    }
}
