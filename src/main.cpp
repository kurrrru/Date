#include <cstddef>
#include <iomanip>
#include <iostream>
#include <sstream>
#include <stdexcept>
#include <string>

#include <Date.hpp>
#include <calendar_system/EthiopianCalendar.hpp>
#include <calendar_system/FrenchRepublicanCalendar.hpp>
#include <calendar_system/GregorianCalendar.hpp>
#include <calendar_system/JapaneseEra.hpp>
#include <calendar_system/JulianCalendar.hpp>
#include <calendar_system/NonProlepticGregorianCalendar.hpp>

int main() {
    toolbox::Date date = toolbox::Date::today();
    std::cout << "Today: "
            << date.to_string(toolbox::GREGORIAN, "%Y-%m-%d")
            << std::endl;
    std::cout << "Today: "
            << date.to_string(toolbox::JULIAN, "%Y-%m-%d")
            << std::endl;
    std::cout << "Today: "
            << date.to_string(toolbox::ETHIOPIAN, "%Y-%M-%d")
            << std::endl;
    std::cout << "Today: "
            << date.to_string(toolbox::JAPANESE_WAREKI, "%E%Y-%m-%d")
            << std::endl;
    return 0;
}