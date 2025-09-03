#include <calender_system/GregorianCalendar.hpp>

#include <iostream>
#include <iomanip>
#include <sstream>
#include <string>

#include <string.hpp>

namespace {

bool is_leap(int year);
int last_day_of_month(int year, int month);
std::string to_string_Ee(int era, bool uppercase);
std::string to_string_Yy(int year, bool uppercase);
std::string to_string_Mm(int month, bool uppercase);
std::string to_string_Dd(int day, bool uppercase);
std::string to_string_Ww(int day_of_week, bool uppercase);

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
    if (era < 0 || era >= END_OF_ERA) {
        throw std::out_of_range("GregorianCalendar::to_serial_date failed:"
            "Invalid era: " + toolbox::to_string(era));
    }
    if (year < 0) {
        throw std::out_of_range("GregorianCalendar::to_serial_date failed:"
            "year must be positive");
    } else if (year == 0) {
        throw std::out_of_range("GregorianCalendar::to_serial_date failed:"
            "year 0 does not exist in Gregorian calendar");
    }
    if (era == BC) {
        year = 1 - year;
    }
    if (month < 1 || month > 12) {
        throw std::out_of_range("GregorianCalendar::to_serial_date failed:"
            "month must be in 1..12");
    }
    if (day < 1 || day > last_day_of_month(year, month)) {
        throw std::out_of_range("GregorianCalendar::to_serial_date failed:"
            "day is out of range for month " + toolbox::to_string(month)
            + " of year " + toolbox::to_string(year));
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
    era = year <= 0 ? BC : AD;
    if (year <= 0) {
        year = 1 - year;
    }
}

void GregorianCalendar::from_serial_date(int serial_date,
        std::string& date_str, const char* format) const {
    int era, year, month, day;
    from_serial_date(serial_date, era, year, month, day);
    int day_of_week;
    from_serial_date(serial_date, day_of_week);
    std::ostringstream ss;
    // very simple implementation
    for (std::size_t i = 0; format[i]; ++i) {
        if (format[i] == '%') {
            switch (format[++i]) {
                case 'E':
                case 'e':
                    ss << to_string_Ee(era, std::isupper(format[i]));
                    break;
                case 'Y':
                case 'y':
                    ss << to_string_Yy(year, std::isupper(format[i]));
                    break;
                case 'M':
                case 'm':
                    ss << to_string_Mm(month, std::isupper(format[i]));
                    break;
                case 'D':
                case 'd':
                    ss << to_string_Dd(day, std::isupper(format[i]));
                    break;
                case 'W':
                case 'w':
                    ss << to_string_Ww(day_of_week, std::isupper(format[i]));
                    break;
                case '%':
                    ss << '%';
                    break;
                default:
                    throw std::invalid_argument("GregorianCalendar::from_serial_date failed:"
                        "Invalid format specifier: %" + toolbox::to_string(format[i]));
            }
        } else {
            ss << format[i];
        }
    }
    date_str = ss.str();
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

std::string to_string_Ee(int era, bool uppercase) {
    static const char* era_str_E[] = {
        [toolbox::GregorianCalendar::BC] = "B.C.",
        [toolbox::GregorianCalendar::AD] = "A.D.",
    };
    static const char* era_str_e[] = {
        [toolbox::GregorianCalendar::BC] = "BC",
        [toolbox::GregorianCalendar::AD] = "AD",
    };
    if (era < 0 || era >= toolbox::GregorianCalendar::END_OF_ERA) {
        throw std::out_of_range("era_to_string: Invalid era: " + toolbox::to_string(era));
    }
    return uppercase ? era_str_E[era] : era_str_e[era];
}

std::string to_string_Yy(int year, bool uppercase) {
    if (year < 0) {
        throw std::out_of_range("year_to_string: year must be positive");
    } else if (year == 0) {
        throw std::out_of_range("year_to_string: year 0 does not exist in Gregorian calendar");
    }
    std::ostringstream oss;
    if (uppercase) {
        oss << year;
    } else {
        oss << std::setw(2) << std::setfill('0') << (year % 100);
    }
    return oss.str();
}

std::string to_string_Mm(int month, bool uppercase) {
    if (month < 1 || month > 12) {
        throw std::out_of_range("month_to_string: month must be in 1..12");
    }
    std::ostringstream oss;
    if (uppercase) {
        oss << month;
    } else {
        oss << std::setw(2) << std::setfill('0') << month;
    }
    return oss.str();
}

std::string to_string_Dd(int day, bool uppercase) {
    if (day < 1 || day > 31) {
        throw std::out_of_range("day_to_string: day must be in 1..31");
    }
    std::ostringstream oss;
    if (uppercase) {
        oss << day;
    } else {
        oss << std::setw(2) << std::setfill('0') << day;
    }
    return oss.str();
}

std::string to_string_Ww(int day_of_week, bool uppercase) {
    static const char* day_of_week_str_W[] = {
        "Sunday", "Monday", "Tuesday", "Wednesday",
        "Thursday", "Friday", "Saturday"
    };
    static const char* day_of_week_str_w[] = {
        "Sun.", "Mon.", "Tue.", "Wed.",
        "Thu.", "Fri.", "Sat."
    };
    if (day_of_week < 0 || day_of_week > 6) {
        throw std::out_of_range("day_of_week_to_string: day_of_week must be in 0..6");
    }
    return uppercase ? day_of_week_str_W[day_of_week] : day_of_week_str_w[day_of_week];
}

}  // namespace
