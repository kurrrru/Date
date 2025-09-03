#pragma once

#include <string>

#include <calender_system/ICalendarSystem.hpp>

namespace toolbox {

class GregorianCalendar : public ICalendarSystem {
 public:
    GregorianCalendar();
    GregorianCalendar(const GregorianCalendar& other);
    GregorianCalendar& operator=(const GregorianCalendar& other);
    ~GregorianCalendar();

    int to_serial_date(int era, int year, int month, int day) const;
    int to_serial_date(const std::string& date_str, const char* format) const;
    void from_serial_date(int serial_date,
            int& era, int& year, int& month, int& day) const;
    void from_serial_date(int serial_date,
            std::string& date_str, const char* format) const;
    void from_serial_date(int serial_date,
            int& day_of_week) const;  // 0=Sun, 1=Mon, ..., 6=Sat

    enum Era {
        BC,
        AD,
        END_OF_ERA
    };
};

}  // namespace toolbox
