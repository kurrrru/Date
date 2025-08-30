#include <Date.hpp>

toolbox::Date::Date() : _serial_date(0) {}

toolbox::Date::Date(const Date& other) : _serial_date(other._serial_date) {}

toolbox::Date& toolbox::Date::operator=(const Date& other) {
    if (this != &other) {
        _serial_date = other._serial_date;
    }
    return *this;
}

toolbox::Date::~Date() {}

toolbox::Date::Date(int serial_date) : _serial_date(serial_date) {}

toolbox::Date::Date(toolbox::CalendarSystem cal_sys, const std::string& era,
        int day, int month, int year) {
    _serial_date = convert_to_serial_date(cal_sys, era, year, month, day);
}

toolbox::Date::Date(toolbox::CalendarSystem cal_sys, const std::string& date_str,
        const char* format) {
    _serial_date = convert_to_serial_date(cal_sys, date_str, format);
}

std::string toolbox::Date::to_string(CalendarSystem cal_sys,
        const char* format) const {
    std::string date_str;
    convert_from_serial_date(cal_sys, date_str, format);
    return date_str;
}

int toolbox::Date::get_raw_date() const {
    return _serial_date;
}

int toolbox::Date::get_day(toolbox::CalendarSystem cal_sys) const {
    std::string era;
    int year, month, day;
    convert_form_serial_date(cal_sys, era, year, month, day);
    return day;
}

int toolbox::Date::get_month(toolbox::CalendarSystem cal_sys) const {
    std::string era;
    int year, month, day;
    convert_form_serial_date(cal_sys, era, year, month, day);
    return month;
}

int toolbox::Date::get_year(toolbox::CalendarSystem cal_sys) const {
    std::string era;
    int year, month, day;
    convert_form_serial_date(cal_sys, era, year, month, day);
    return year;
}

int toolbox::Date::get_weekday(toolbox::CalendarSystem cal_sys) const {
    int day_of_week;
    convert_from_serial_date(cal_sys, day_of_week);
    return day_of_week;
}

toolbox::Date& toolbox::Date::operator++() {
    ++_serial_date;
    return *this;
}

toolbox::Date toolbox::Date::operator++(int) {
    Date old(*this);
    ++_serial_date;
    return old;
}

toolbox::Date& toolbox::Date::operator--() {
    --_serial_date;
    return *this;
}

toolbox::Date toolbox::Date::operator--(int) {
    Date old(*this);
    --_serial_date;
    return old;
}

toolbox::Date toolbox::Date::operator+(const int delta) const {
    return Date(_serial_date + delta);
}

toolbox::Date toolbox::Date::operator-(const int delta) const {
    return Date(_serial_date - delta);
}

int toolbox::Date::operator-(const Date& other) const {
    return _serial_date - other._serial_date;
}

toolbox::Date& toolbox::Date::operator+=(const int delta) {
    _serial_date += delta;
    return *this;
}

toolbox::Date& toolbox::Date::operator-=(const int delta) {
    _serial_date -= delta;
    return *this;
}

void toolbox::Date::convert_form_serial_date(toolbox::CalendarSystem cal_sys,
        std::string& era, int& year, int& month, int& day) const {
    switch (cal_sys) {
        case toolbox::GREGORIAN:
            // toolbox::CalendarSystem::Gregorian::from_serial_date(_serial_date,
            //         era, year, month, day);
            break;
        case toolbox::JULIAN:
            // toolbox::CalendarSystem::Julian::from_serial_date(_serial_date,
            //         era, year, month, day);
            break;
        case toolbox::JAPANESE_WAREKI:
            // toolbox::CalendarSystem::JapaneseWareki::from_serial_date(_serial_date,
            //         era, year, month, day);
            break;
        case toolbox::FRENCH_REVOLUTIONARY:
            // toolbox::CalendarSystem::FrenchRevolutionary::from_serial_date(_serial_date,
            //         era, year, month, day);
            break;
        default:
            throw std::invalid_argument("Invalid calendar system");
        }
    // later
}

void toolbox::Date::convert_from_serial_date(toolbox::CalendarSystem cal_sys,
        std::string& date_str, const char* format) const {
    // later
}

void toolbox::Date::convert_from_serial_date(toolbox::CalendarSystem cal_sys,
        int& day_of_week) const {
    // later
}

int toolbox::Date::convert_to_serial_date(toolbox::CalendarSystem cal_sys,
        const std::string& era, int year, int month, int day) const {
    // later
    return 0;
}

int toolbox::Date::convert_to_serial_date(toolbox::CalendarSystem cal_sys,
        const std::string& date_str, const char* format) const {
    // later
    return 0;
}
