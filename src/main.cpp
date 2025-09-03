#include <Date.hpp>
#include <calender_system/GregorianCalendar.hpp>

#include <iostream>
#include <string>
#include <sstream>

int main() {
    toolbox::Date date1(toolbox::GREGORIAN, toolbox::GregorianCalendar::AD, 2025, 9, 2);
    std::stringstream ss;
    ss << "Date1: " << date1.get_year(toolbox::GREGORIAN) << "-"
        << date1.get_month(toolbox::GREGORIAN) << "-"
        << date1.get_day(toolbox::GREGORIAN) << std::endl;
    std::cout << ss.str();
    std::cout << "Raw date (days since 1970-01-01): " << date1.get_raw_date() << std::endl;
    std::cout << "day of week (0=Sun,...,6=Sat): " << date1.get_weekday(toolbox::GREGORIAN) << std::endl;
    date1--;
    std::cout << "After decrement: " << date1.get_year(toolbox::GREGORIAN) << "-"
        << date1.get_month(toolbox::GREGORIAN) << "-"
        << date1.get_day(toolbox::GREGORIAN) << std::endl;
    std::cout << "Raw date (days since 1970-01-01): " << date1.get_raw_date() << std::endl;
    std::cout << "day of week (0=Sun,...,6=Sat): " << date1.get_weekday(toolbox::GREGORIAN) << std::endl;
    std::cout << "Formatted date: " << date1.to_string(toolbox::GREGORIAN, "%Y(%E)-%M-%D (%W), %y(%e)-%m-%d (%w)") << std::endl;

    toolbox::GregorianCalendar gc;
    int era, year, month, day;
    gc.from_serial_date(0, era, year, month, day);
    std::cout << "1970-01-01 in Gregorian Calendar: " << year << "-" << month << "-" << day << std::endl;

    return 0;
}