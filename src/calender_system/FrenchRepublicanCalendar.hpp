#pragma once

#include <string>

#include <calender_system/ICalendarSystem.hpp>

namespace toolbox {

class FrenchRepublicanCalendar : public ICalendarSystem {
 public:
    FrenchRepublicanCalendar();
    FrenchRepublicanCalendar(const FrenchRepublicanCalendar& other);
    FrenchRepublicanCalendar& operator=(
        
    const FrenchRepublicanCalendar& other);
    ~FrenchRepublicanCalendar();

    int to_serial_date(int era, int year, int month, int day) const;
    int to_serial_date(const std::string& date_str,
        const char* format, bool strict) const;
    void from_serial_date(int serial_date,
        int& era, int& year, int& month, int& day) const;
    void from_serial_date(int serial_date,
        std::string& date_str, const char* format) const;
    void from_serial_date(int serial_date,
        int& day_of_week) const;  // 0=Sun, 1=Mon, ..., 6=Sat
};

}  // namespace toolbox
