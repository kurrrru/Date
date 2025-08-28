#pragma once

#include <string>
#include <iostream>

class Date {
 public:
    Date();
    Date(const Date& other);
    Date& operator=(const Date& other);
    ~Date();

    explicit Date(int serial_date);
    Date(int day, int month, int year);
    Date(const std::string& date_str, const char* format = "%Y-%m-%d");

    std::string to_string(const char* format = "%Y-%m-%d") const;

    int get_raw_date() const;
    int get_day() const;
    int get_month() const;
    int get_year() const;
    int get_weekday() const; // 0=Sun, 1=Mon, ..., 6=Sat

    Date& operator++(); // 前置インクリメント
    Date operator++(int); // 後置インクリメント
    Date& operator--(); // 前置デクリメント
    Date operator--(int); // 後置デクリメント

    Date operator+(const int delta) const;
    Date operator-(const int delta) const;
    int operator-(const Date& other) const;
    Date& operator+=(const int delta);
    Date& operator-=(const int delta);

 private:
    int _serial_date; // 0 mean 1970-01-01 (Unix epoch)
};
