#include <calender_system/GregorianCalendar.hpp>

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

int GregorianCalendar::to_serial_date(const std::string& era,
        int year, int month, int day) const {
    (void)era;
    // Hinnant's algorithm
    year -= month <= 2;
    const int era_year = (year >= 0 ? year : year - 399) / 400;
    const unsigned int yoe = static_cast<unsigned int>(year - era_year * 400);
    const unsigned int doy = (153 * (month > 2 ? month - 3 : month + 9) + 2) / 5 + day - 1;
    const unsigned int doe = yoe * 365 + yoe / 4 - yoe / 100 + doy;
    return era_year * 146097 + static_cast<int>(doe) - 719468;
}

int GregorianCalendar::to_serial_date(const std::string& date_str,
        const char* format) const {
    // later
}

void GregorianCalendar::from_serial_date(int serial_date,
        std::string& era, int& year, int& month, int& day) const {
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
    if (month <= 2) {
        year++;
    }
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
    // Zeller's Algorithm
    std::string era;
    int d, m, y;
    from_serial_date(serial_date, era, y, m, d);
    if (m < 3) {
        m += 12;
        y -= 1;
    }
    day_of_week = (d + (13 * (m + 1)) / 5 + y + (y / 4) - (y / 100) + (y / 400)) % 7;
    day_of_week = (day_of_week + 5) % 7;  // 0=Sun, 1=Mon, ..., 6=Sat
}

}  // namespace toolbox