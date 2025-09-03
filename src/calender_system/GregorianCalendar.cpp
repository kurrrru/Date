#include <calender_system/GregorianCalendar.hpp>

#include <iostream>

namespace {

bool is_leap(int year);
int last_day_of_month(int year, int month);

}  // namespace

namespace toolbox {
    
GregorianCalendar::GregorianCalendar() {
}

GregorianCalendar::GregorianCalendar(const GregorianCalendar& other) {
    (void)other;
}

GregorianCalendar& GregorianCalendar::operator=(const GregorianCalendar& other) {
    (void)other;
    return *this;
}

GregorianCalendar::~GregorianCalendar() {
}

int GregorianCalendar::to_serial_date(int era,
        int year, int month, int day) const {
    (void)era;
    if (month < 1 || month > 12) {
        throw std::out_of_range("month must be 1-12");
    }
    if (day < 1 || day > last_day_of_month(year, month)) {
        throw std::out_of_range("day is out of range for month");
    }
    year -= !!(month <= 2);
    const int era_year = (year >= 0 ? year : year - 399) / 400;
    const unsigned int yoe = static_cast<unsigned int>(year - era_year * 400);
    const unsigned int doy = (153 * (month > 2 ? month - 3 : month + 9) + 2) / 5 + day - 1;
    const unsigned int doe = yoe * 365 + yoe / 4 - yoe / 100 + doy;
    std::cout << "Debug: year=" << year << ", month=" << month << ", day=" << day << std::endl;
    std::cout << "Debug: era_year=" << era_year << ", yoe=" << yoe
            << ", doy=" << doy << ", doe=" << doe << std::endl;

    return era_year * 146097 + static_cast<int>(doe) - 719468;
}

int GregorianCalendar::to_serial_date(const std::string& date_str,
        const char* format) const {
    // later
    (void)date_str;
    (void)format;
    return 0;
}

void GregorianCalendar::from_serial_date(int serial_date,
        int& era, int& year, int& month, int& day) const {
    (void)era;
    // Hinnant's algorithm
    int z = serial_date + 719468;
    const int era_year = (z >= 0 ? z : z - 146096) / 146097;
    const unsigned int doe = static_cast<unsigned int>(z - era_year * 146097);
    const unsigned int yoe = (doe - doe / 1460 + doe / 36524 - doe / 146096) / 365;
    year = static_cast<int>(yoe) + era_year * 400;
    const unsigned int doy = doe - (365 * yoe + yoe / 4 - yoe / 100);
    const unsigned int mp = (5 * doy + 2) / 153;
    day = doy - (153 * mp + 2) / 5 + 1;
    month = mp < 10 ? mp + 3 : mp - 9;
    year += !!(month <= 2);
}

void GregorianCalendar::from_serial_date(int serial_date,
        std::string& date_str, const char* format) const {
    // later
    (void)serial_date;
    (void)date_str;
    (void)format;
}

void GregorianCalendar::from_serial_date(int serial_date,
        int& day_of_week) const {
    day_of_week = serial_date >= -4 ?
            (serial_date + 4) % 7 : (serial_date + 5) % 7 + 6;
}

}  // namespace toolbox

namespace {
bool is_leap(int year) {
    // return (year % 4 == 0) && (year % 100 != 0 || year % 400 == 0);
    return !(year & 0x3) && (year % 100 != 0 || year % 400 == 0);
}

int last_day_of_month(int year, int month) {
    static const int last_day[12] = {
        31, 28, 31, 30, 31, 30, 31, 31, 30, 31, 30, 31
    };
    if (month == 2 && is_leap(year)) {
        return 29;
    }
    return last_day[month - 1];
}

}  // namespace
