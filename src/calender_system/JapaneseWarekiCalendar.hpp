#pragma once

#include <string>

#include <calender_system/ICalendarSystem.hpp>

namespace toolbox {

class JapaneseWarekiCalendar : public ICalendarSystem {
 public:
    JapaneseWarekiCalendar();
    JapaneseWarekiCalendar(const JapaneseWarekiCalendar& other);
    JapaneseWarekiCalendar& operator=(const JapaneseWarekiCalendar& other);
    ~JapaneseWarekiCalendar();

    int to_serial_date(const std::string& era,
            int year, int month, int day) const override;
    int to_serial_date(const std::string& date_str,
            const char* format) const override;
    void from_serial_date(int serial_date,
            std::string& era, int& year, int& month, int& day) const override;
    void from_serial_date(int serial_date,
            std::string& date_str, const char* format) const override;
    void from_serial_date(int serial_date,
            int& day_of_week) const override;  // 0=Sun, 1=Mon, ..., 6=Sat
};

}  // namespace toolbox
