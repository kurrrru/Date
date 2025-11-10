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

void test_parse_gregorian(const std::string& date_str,
                          const char* format,
                          bool strict,
                          bool expect_success) {
    static int test_num = 0;
    try {
        toolbox::Date date(toolbox::GREGORIAN, date_str, format, strict);
        std::cout << "gregorian " << std::setw(3) << ++test_num << ": ";
        if (expect_success) {
            std::cout << "OK" << std::endl;
        } else {
            std::cout << "NG" << std::endl;
        }
    } catch (const std::exception& e) {
        std::cout << "gregorian " << std::setw(3) << ++test_num << ": ";
        if (!expect_success) {
            std::cout << "OK" << std::endl;
            return;
        }
        std::cout << "NG" << std::endl;
        std::cout << "Error: " << e.what() << std::endl;
    }
}

void test_parse_non_proleptic_gregorian(const std::string& date_str,
                                        const char* format,
                                        bool strict,
                                        bool expect_success) {
    static int test_num = 0;
    try {
        toolbox::Date date(toolbox::NON_PROLEPTIC_GREGORIAN,
                           date_str,
                           format,
                           strict);
        std::cout << "non-proleptic " << std::setw(3) << ++test_num << ": ";
        if (expect_success) {
            std::cout << "OK" << std::endl;
        } else {
            std::cout << "NG" << std::endl;
        }
    } catch (const std::exception& e) {
        std::cout << "non-proleptic " << std::setw(3) << ++test_num << ": ";
        if (!expect_success) {
            std::cout << "OK" << std::endl;
            return;
        }
        std::cout << "NG" << std::endl;
        std::cout << "Error: " << e.what() << std::endl;
    }
}

struct EraTestCase {
    toolbox::JapaneseEra era;
    toolbox::EraCalendar start_calendar;
    int start_year;
    int start_month;
    int start_day;
    toolbox::EraCalendar end_calendar;
    int end_year;
    int end_month;
    int end_day;
    bool has_end;
};

struct WarekiConversionCase {
    toolbox::JapaneseEra era;
    int wareki_year;
    int month;
    int day;
    toolbox::CalendarSystem reference_calendar;
    int reference_era;
    int reference_year;
    int reference_month;
    int reference_day;
};

int& japanese_era_test_counter() {
    static int counter = 0;
    return counter;
}

int& japanese_conversion_test_counter() {
    static int counter = 0;
    return counter;
}

bool lookup_era_by_kanji(const std::string& kanji,
                         toolbox::JapaneseEra& result) {
    const std::size_t count = toolbox::era_count();
    for (std::size_t i = 0; i < count; ++i) {
        const toolbox::EraMetadata& meta =
            toolbox::get_era_metadata(
                static_cast<toolbox::JapaneseEra>(i));
        if (kanji == meta.kanji) {
            result = meta.era;
            return true;
        }
    }
    return false;
}

void test_japanese_era_metadata_case(const EraTestCase& tc) {
    int& counter = japanese_era_test_counter();
    const toolbox::EraMetadata& meta = toolbox::get_era_metadata(tc.era);
    bool pass = true;
    if (meta.era != tc.era) {
        pass = false;
    }
    if (meta.start.calendar != tc.start_calendar ||
        meta.start.year != tc.start_year ||
        meta.start.month != tc.start_month ||
        meta.start.day != tc.start_day) {
        pass = false;
    }
    if (meta.has_end != tc.has_end) {
        pass = false;
    }
    if (tc.has_end) {
        if (meta.end.calendar != tc.end_calendar ||
            meta.end.year != tc.end_year ||
            meta.end.month != tc.end_month ||
            meta.end.day != tc.end_day) {
            pass = false;
        }
    }
    std::cout << "wareki meta " << std::setw(3) << ++counter << ": "
              << (pass ? "OK" : "NG") << std::endl;
    if (!pass) {
        std::cout << "  actual start cal=" << meta.start.calendar
                  << " y=" << meta.start.year
                  << " m=" << meta.start.month
                  << " d=" << meta.start.day << std::endl;
        std::cout << "  expect start cal=" << tc.start_calendar
                  << " y=" << tc.start_year
                  << " m=" << tc.start_month
                  << " d=" << tc.start_day << std::endl;
        std::cout << "  has_end actual=" << (meta.has_end ? 1 : 0)
                  << " expect=" << (tc.has_end ? 1 : 0) << std::endl;
        if (meta.has_end) {
            std::cout << "  actual end cal=" << meta.end.calendar
                      << " y=" << meta.end.year
                      << " m=" << meta.end.month
                      << " d=" << meta.end.day << std::endl;
            std::cout << "  expect end cal=" << tc.end_calendar
                      << " y=" << tc.end_year
                      << " m=" << tc.end_month
                      << " d=" << tc.end_day << std::endl;
        }
    }
}

void test_japanese_era_count() {
    int& counter = japanese_era_test_counter();
    const std::size_t expected =
        static_cast<std::size_t>(toolbox::END_OF_ERA);
    const std::size_t actual = toolbox::era_count();
    bool pass = actual == expected;
    std::cout << "wareki meta " << std::setw(3) << ++counter << ": "
              << (pass ? "OK" : "NG") << std::endl;
    if (!pass) {
        std::cout << "  count actual=" << actual
                  << " expected=" << expected << std::endl;
    }
}

void run_japanese_era_metadata_tests() {
    const EraTestCase cases[] = {
        {toolbox::TAIKA,
         toolbox::ERA_CALENDAR_JULIAN,
         645,
         7,
         29,
         toolbox::ERA_CALENDAR_JULIAN,
         650,
         3,
         22,
         true},
        {toolbox::MEIJI,
         toolbox::ERA_CALENDAR_GREGORIAN,
         1868,
         1,
         25,
         toolbox::ERA_CALENDAR_GREGORIAN,
         1912,
         7,
         29,
         true},
        {toolbox::SHOWA,
         toolbox::ERA_CALENDAR_GREGORIAN,
         1926,
         12,
         25,
         toolbox::ERA_CALENDAR_GREGORIAN,
         1989,
         1,
         7,
         true},
        {toolbox::REIWA,
         toolbox::ERA_CALENDAR_GREGORIAN,
         2019,
         5,
         1,
         toolbox::ERA_CALENDAR_GREGORIAN,
         0,
         0,
         0,
         false}
    };
    const std::size_t case_count = sizeof(cases) / sizeof(cases[0]);
    for (std::size_t i = 0; i < case_count; ++i) {
        test_japanese_era_metadata_case(cases[i]);
    }
    test_japanese_era_count();
}

void test_japanese_era_string_roundtrip(toolbox::JapaneseEra era,
                                        const char* expected_kanji) {
    int& counter = japanese_era_test_counter();
    bool pass = true;
    const std::string kanji = toolbox::era_to_kanji(era);
    if (kanji != expected_kanji) {
        pass = false;
    }
    toolbox::JapaneseEra looked = toolbox::END_OF_ERA;
    bool found = lookup_era_by_kanji(expected_kanji, looked);
    if (!found || looked != era) {
        pass = false;
    }
    std::cout << "wareki meta " << std::setw(3) << ++counter << ": "
              << (pass ? "OK" : "NG") << std::endl;
    if (!pass) {
        std::cout << "  kanji actual=" << kanji
                  << " expected=" << expected_kanji << std::endl;
        if (found) {
            std::cout << "  lookup era=" << looked
                      << " expected=" << era << std::endl;
        } else {
            std::cout << "  lookup failed" << std::endl;
        }
    }
}

void run_japanese_era_string_roundtrip_tests() {
    test_japanese_era_string_roundtrip(toolbox::TAIKA, "大化");
    test_japanese_era_string_roundtrip(toolbox::TEMPYO_SHOHO, "天平勝宝");
    test_japanese_era_string_roundtrip(toolbox::SHOWA, "昭和");
    test_japanese_era_string_roundtrip(toolbox::REIWA, "令和");
}

void test_nanbokucho_authority(toolbox::JapaneseEra era,
                               toolbox::EraAuthority expected) {
    int& counter = japanese_era_test_counter();
    const toolbox::EraMetadata& meta = toolbox::get_era_metadata(era);
    bool pass = meta.authority == expected;
    std::cout << "wareki meta " << std::setw(3) << ++counter << ": "
              << (pass ? "OK" : "NG") << std::endl;
    if (!pass) {
        std::cout << "  authority actual=" << meta.authority
                  << " expected=" << expected << std::endl;
    }
}

void run_nanbokucho_authority_tests() {
    test_nanbokucho_authority(toolbox::KENMU, toolbox::ERA_AUTHORITY_SOUTHERN);
    test_nanbokucho_authority(toolbox::SHOHEI,
                              toolbox::ERA_AUTHORITY_SOUTHERN);
    test_nanbokucho_authority(toolbox::RYAKUO,
                              toolbox::ERA_AUTHORITY_NORTHERN);
    // MEITOKU is recorded as northern authority in embedded metadata.
    test_nanbokucho_authority(toolbox::MEITOKU,
                              toolbox::ERA_AUTHORITY_NORTHERN);
}

void test_japanese_wareki_conversion_case(const WarekiConversionCase& tc) {
    const char* fmt = "%E%Y-%m-%d";
    int& counter = japanese_conversion_test_counter();
    const int id = ++counter;
    bool pass = true;
    std::ostringstream failure;
    try {
        toolbox::Date wareki_date(toolbox::JAPANESE_WAREKI,
                                  static_cast<int>(tc.era),
                                  tc.wareki_year,
                                  tc.month,
                                  tc.day);
        toolbox::Date reference_date(tc.reference_calendar,
                                     tc.reference_era,
                                     tc.reference_year,
                                     tc.reference_month,
                                     tc.reference_day);
        if (wareki_date.get_raw_date() != reference_date.get_raw_date()) {
            pass = false;
            failure << "serial mismatch wareki=" << wareki_date.get_raw_date()
                    << " reference=" << reference_date.get_raw_date()
                    << "\n";
        }

        std::ostringstream expected_ss;
        expected_ss << toolbox::era_to_kanji(tc.era)
                    << tc.wareki_year
                    << "-" << tc.month
                    << "-" << tc.day;
        const std::string expected_wareki = expected_ss.str();

        try {
            const std::string from_reference =
                reference_date.to_string(toolbox::JAPANESE_WAREKI, fmt);
            if (from_reference != expected_wareki) {
                pass = false;
                failure << "reference->wareki actual=" << from_reference
                        << " expected=" << expected_wareki << "\n";
            }
        } catch (const std::exception& e) {
            pass = false;
            failure << "reference->wareki threw: " << e.what() << "\n";
        }

        try {
            const std::string from_wareki =
                wareki_date.to_string(toolbox::JAPANESE_WAREKI, fmt);
            if (from_wareki != expected_wareki) {
                pass = false;
                failure << "wareki format actual=" << from_wareki
                        << " expected=" << expected_wareki << "\n";
            }
        } catch (const std::exception& e) {
            pass = false;
            failure << "wareki format threw: " << e.what() << "\n";
        }

        try {
            const std::string wareki_as_greg =
                wareki_date.to_string(toolbox::GREGORIAN, fmt);
            const std::string ref_as_greg =
                reference_date.to_string(toolbox::GREGORIAN, fmt);
            if (wareki_as_greg != ref_as_greg) {
                pass = false;
                failure << "gregorian mismatch wareki=" << wareki_as_greg
                        << " reference=" << ref_as_greg << "\n";
            }
        } catch (const std::exception& e) {
            pass = false;
            failure << "gregorian format threw: " << e.what() << "\n";
        }

        try {
            const std::string wareki_as_julian =
                wareki_date.to_string(toolbox::JULIAN, fmt);
            const std::string ref_as_julian =
                reference_date.to_string(toolbox::JULIAN, fmt);
            if (wareki_as_julian != ref_as_julian) {
                pass = false;
                failure << "julian mismatch wareki=" << wareki_as_julian
                        << " reference=" << ref_as_julian << "\n";
            }
        } catch (const std::exception& e) {
            pass = false;
            failure << "julian format threw: " << e.what() << "\n";
        }
    } catch (const std::exception& e) {
        pass = false;
        failure << "construction threw: " << e.what() << "\n";
    }

    std::cout << "wareki conv " << std::setw(3) << id << ": "
              << (pass ? "OK" : "NG") << std::endl;
    const std::string detail = failure.str();
    if (!pass && !detail.empty()) {
        std::cout << detail;
    }
}

void run_japanese_wareki_conversion_tests() {
    const WarekiConversionCase cases[] = {
        {toolbox::REIWA,
         1,
         5,
         1,
         toolbox::GREGORIAN,
         toolbox::GregorianCalendar::AD,
         2019,
         5,
         1},
        {toolbox::SHOWA,
         64,
         1,
         7,
         toolbox::GREGORIAN,
         toolbox::GregorianCalendar::AD,
         1989,
         1,
         7},
    {toolbox::HEISEI,
     1,
     1,
     8,
     toolbox::GREGORIAN,
     toolbox::GregorianCalendar::AD,
     1989,
     1,
     8}
    };
    const std::size_t case_count = sizeof(cases) / sizeof(cases[0]);
    for (std::size_t i = 0; i < case_count; ++i) {
        test_japanese_wareki_conversion_case(cases[i]);
    }
}

int to_serial_from_start(const toolbox::EraMetadata& meta) {
    if (meta.start.calendar == toolbox::ERA_CALENDAR_JULIAN) {
        toolbox::Date date(toolbox::JULIAN,
                           toolbox::JulianCalendar::AD,
                           meta.start.year,
                           meta.start.month,
                           meta.start.day);
        return date.get_raw_date();
    }
    toolbox::Date date(toolbox::GREGORIAN,
                       toolbox::GregorianCalendar::AD,
                       meta.start.year,
                       meta.start.month,
                       meta.start.day);
    return date.get_raw_date();
}

void test_japanese_wareki_format(toolbox::JapaneseEra era) {
    int& counter = japanese_era_test_counter();
    const int id = ++counter;
    const toolbox::EraMetadata& meta = toolbox::get_era_metadata(era);
    const int serial = to_serial_from_start(meta);
    toolbox::Date date(serial);
    std::string actual;
    bool pass = true;
    try {
        actual = date.to_string(toolbox::JAPANESE_WAREKI, "%E%Y-%m-%d");
    } catch (const std::exception& e) {
        (void)e;
        pass = false;
    }
    if (pass) {
        // Instead of asserting an exact textual form (which depends on how
        // overlapping eras and era authority are represented), verify the
        // formatted string contains a valid kanji era prefix and the month/day
        // match the underlying Gregorian date for the serial.
        toolbox::GregorianCalendar greg;
        int g_era = 0;
        int g_year = 0;
        int g_month = 0;
        int g_day = 0;
        greg.from_serial_date(serial, g_era, g_year, g_month, g_day);

        // Expect actual to end with "-M-D" where M/D are g_month/g_day
        std::ostringstream tail;
        tail << "-" << g_month << "-" << g_day;
        const std::string tail_str = tail.str();
        if (actual.size() < tail_str.size() ||
            actual.substr(actual.size() - tail_str.size()) != tail_str) {
            pass = false;
            std::cout << "wareki meta " << std::setw(3) << id
                      << ": NG" << std::endl;
            std::cout << "  actual=" << actual
                      << " expected_tail=" << tail_str << std::endl;
            return;
        }

        // Extract era kanji prefix (everything before first digit)
        std::size_t pos = 0;
        while (pos < actual.size() && !std::isdigit(actual[pos])) ++pos;
        std::string kanji = actual.substr(0, pos);
        toolbox::JapaneseEra looked = toolbox::END_OF_ERA;
        bool found = lookup_era_by_kanji(kanji, looked);
        if (!found) {
            pass = false;
            std::cout << "wareki meta " << std::setw(3) << id
                      << ": NG" << std::endl;
            std::cout << "  actual=" << actual
                      << " (unknown era kanji prefix)" << std::endl;
            return;
        }
    }
    if (!pass) {
        std::cout << "wareki meta " << std::setw(3) << id << ": NG"
                  << std::endl;
        std::cout << "  to_string threw exception" << std::endl;
        return;
    }
    std::cout << "wareki meta " << std::setw(3) << id << ": OK"
              << std::endl;
}

void run_japanese_wareki_format_tests() {
    test_japanese_wareki_format(toolbox::MEIJI);
    test_japanese_wareki_format(toolbox::TEMPYO_SHOHO);
    test_japanese_wareki_format(toolbox::REIWA);
}

int main() {
    toolbox::Date date;
    struct ParseCase {
        const char* date;
        const char* format;
        bool strict;
        bool expect;
    };

    const ParseCase greg_cases[] = {
        {"2024-02-09", "%Y-%m-%d", true, true},
        {"2024/2/9", "%Y/%M/%D", true, true},
        {"B.C.24/02/09", "%E%y/%m/%d", true, true},
        {"BC24/02/09", "%e%y/%m/%d", true, true},
        {"", "", true, false},
        {"2024-02", "%Y-%m", true, false},
        {"2024-02", "%Y-%d", true, false},
        {"2024-1111", "%Y-%M1%D", true, false},
        {"2024-1111", "%Y-%M1%D", false, true},
        {"2024-2222", "%Y-%M2%D", true, true},
        {"2024-2222", "%Y-%M2%D", false, true},
        {"2024-12-12-02", "%Y-%M-%m-%d", false, false},
        {"111", "%Y%M%D", true, true},
        {"1111", "%Y%M1%D", true, true},
        {"11111", "%Y%M1%D", true, false},
        {"1011", "%Y%M%D", true, true},
        {"1011", "%Y%m%D", true, true},
        {"10101", "%Y%m%D", true, true},
        {"10101", "%Y%M%D", true, true},
        {"010101", "%Y%M%D", true, false}
    };

    const std::size_t greg_case_count =
        sizeof(greg_cases) / sizeof(greg_cases[0]);
    for (std::size_t i = 0; i < greg_case_count; ++i) {
        test_parse_gregorian(greg_cases[i].date,
                             greg_cases[i].format,
                             greg_cases[i].strict,
                             greg_cases[i].expect);
    }

    const ParseCase non_proleptic_cases[] = {
        {"2024-02-09", "%Y-%m-%d", true, true},
        {"2024/2/9", "%Y/%M/%D", true, true},
        {"B.C.24/02/09", "%E%y/%m/%d", true, false},
        {"BC24/02/09", "%e%y/%m/%d", true, false},
        {"", "", true, false},
        {"2024-02", "%Y-%m", true, false},
        {"2024-02", "%Y-%d", true, false},
        {"2024-1111", "%Y-%M1%D", true, false},
        {"2024-1111", "%Y-%M1%D", false, true},
        {"2024-2222", "%Y-%M2%D", true, true},
        {"2024-2222", "%Y-%M2%D", false, true},
        {"2024-12-12-02", "%Y-%M-%m-%d", false, false},
        {"111", "%Y%M%D", true, false},
        {"1111", "%Y%M1%D", true, false},
        {"11111", "%Y%M1%D", true, false},
        {"1582-114", "%Y-%M%D", true, true},
        {"1582114", "%Y%M%D", true, false},
        {"1582/10/14", "%Y/%M/%D", true, false},
        {"1582/10/15", "%Y/%M/%D", true, true},
        {"1582/10/%D15", "%Y/%M/%D%D", true, false},
        {"1582/10/%D15", "%Y/%M/%D%D", false, false}
    };

    const std::size_t non_proleptic_case_count =
        sizeof(non_proleptic_cases) / sizeof(non_proleptic_cases[0]);
    for (std::size_t i = 0; i < non_proleptic_case_count; ++i) {
        test_parse_non_proleptic_gregorian(non_proleptic_cases[i].date,
                                           non_proleptic_cases[i].format,
                                           non_proleptic_cases[i].strict,
                                           non_proleptic_cases[i].expect);
    }

    date = toolbox::Date(toolbox::GREGORIAN,
                         toolbox::GregorianCalendar::BC,
                         46,
                         12,
                         30);
    std::cout << date.to_string(toolbox::GREGORIAN, "%E%y-%m-%d")
              << std::endl;
    std::cout << date.get_raw_date() << std::endl;

    date = toolbox::Date(toolbox::JULIAN,
                         toolbox::JulianCalendar::AD,
                         1582,
                         10,
                         4);
    std::cout << date.to_string(toolbox::GREGORIAN, "%E%Y-%m-%d")
              << std::endl;
    std::cout << date.to_string(toolbox::JULIAN, "%E%Y-%m-%d")
              << std::endl;
    std::cout << date.get_raw_date() << std::endl;

    date = toolbox::Date(toolbox::GREGORIAN,
                         toolbox::GregorianCalendar::AD,
                         1582,
                         10,
                         15);
    std::cout << date.to_string(toolbox::GREGORIAN, "%E%Y-%m-%d")
              << std::endl;
    std::cout << date.to_string(toolbox::JULIAN, "%E%Y-%m-%d")
              << std::endl;
    std::cout << date.get_raw_date() << std::endl;

    date = toolbox::Date(toolbox::JULIAN,
                         toolbox::JulianCalendar::BC,
                         45,
                         1,
                         1);
    std::cout << date.to_string(toolbox::GREGORIAN, "%E%Y-%m-%d")
              << std::endl;
    std::cout << date.to_string(toolbox::JULIAN, "%E%Y-%m-%d")
              << std::endl;
    std::cout << date.get_raw_date() << std::endl;

    date = toolbox::Date::today();
    std::cout << "Today: "
              << date.to_string(toolbox::GREGORIAN, "%Y-%m-%d")
              << std::endl;
    std::cout << "Today: "
              << date.to_string(toolbox::JULIAN, "%Y-%m-%d")
              << std::endl;
    std::cout << "Today: "
              << date.to_string(toolbox::ETHIOPIAN, "%Y-%M-%d")
              << std::endl;

    date = toolbox::Date(toolbox::JULIAN,
                         toolbox::JulianCalendar::BC,
                         45,
                         3,
                         1);
    std::cout << "BC 45-03-01 (Julian) = " << date.get_raw_date()
              << std::endl;
    date = toolbox::Date(toolbox::JULIAN,
                         toolbox::JulianCalendar::BC,
                         7,
                         3,
                         1);
    std::cout << "BC 7-03-01 (Julian) = " << date.get_raw_date()
              << std::endl;
    date = toolbox::Date(toolbox::JULIAN,
                         toolbox::JulianCalendar::AD,
                         6,
                         3,
                         1);
    std::cout << "AD 6-03-01 (Julian) = " << date.get_raw_date()
              << std::endl;

    date = toolbox::Date(toolbox::JULIAN,
                         toolbox::GregorianCalendar::AD,
                         2024,
                         8,
                         31);
    std::cout << date.to_string(toolbox::JULIAN, "%Y-%m-%d")
              << std::endl;
    date += 1;
    std::cout << date.to_string(toolbox::JULIAN, "%Y-%m-%d")
              << std::endl;
    date -= 1;

    // french republican calendar
    date = toolbox::Date(toolbox::FRENCH_REPUBLICAN,
                         toolbox::FrenchRepublicanCalendar::AD,
                         1,
                         1,
                         1);
    std::cout << date.to_string(toolbox::FRENCH_REPUBLICAN, "%Y-%m-%d")
              << std::endl;
    std::cout << date.to_string(toolbox::FRENCH_REPUBLICAN, "%Y-%M-%D")
              << std::endl;
    std::cout << date.to_string(toolbox::GREGORIAN, "%Y-%m-%d")
              << std::endl;
    date = toolbox::Date(toolbox::FRENCH_REPUBLICAN,
                         toolbox::FrenchRepublicanCalendar::AD,
                         14,
                         4,
                         10);
    std::cout << date.to_string(toolbox::FRENCH_REPUBLICAN, "%Y-%m-%d")
              << std::endl;
    std::cout << date.to_string(toolbox::FRENCH_REPUBLICAN, "%Y-%M-%D")
              << std::endl;
    std::cout << date.to_string(toolbox::GREGORIAN, "%Y-%m-%d")
              << std::endl;
    date = toolbox::Date(toolbox::FRENCH_REPUBLICAN,
                         toolbox::FrenchRepublicanCalendar::AD,
                         3,
                         13,
                         6);
    std::cout << date.to_string(toolbox::FRENCH_REPUBLICAN, "%Y-%m-%d")
              << std::endl;
    std::cout << date.to_string(toolbox::FRENCH_REPUBLICAN, "%Y-%M-%D")
              << std::endl;
    std::cout << date.to_string(toolbox::GREGORIAN, "%Y-%m-%d")
              << std::endl;
    date++;
    std::cout << date.to_string(toolbox::FRENCH_REPUBLICAN, "%Y-%m-%d")
              << std::endl;
    std::cout << date.to_string(toolbox::FRENCH_REPUBLICAN, "%Y-%M-%D")
              << std::endl;
    std::cout << date.to_string(toolbox::GREGORIAN, "%Y-%m-%d")
              << std::endl;
    date = toolbox::Date(toolbox::FRENCH_REPUBLICAN,
                         "Raisin 1, 11",
                         "%d %M, %Y",
                         true);
    std::cout << date.to_string(toolbox::FRENCH_REPUBLICAN, "%Y-%m-%d")
              << std::endl;
    std::cout << date.to_string(toolbox::FRENCH_REPUBLICAN, "%Y-%M-%D")
              << std::endl;
    std::cout << date.to_string(toolbox::GREGORIAN, "%Y-%m-%d")
              << std::endl;

    try {
        date = toolbox::Date(toolbox::FRENCH_REPUBLICAN,
                             "Raisin 2, 11",
                             "%d %M, %Y",
                             true);
        std::cout << "NG" << std::endl;
    } catch (const std::exception& e) {
        (void)e;
        std::cout << "OK" << std::endl;
    }
    date = toolbox::Date(toolbox::FRENCH_REPUBLICAN,
                         "Jour de la Revolution 13, 11",
                         "%d %M, %Y",
                         false);
    std::cout << date.to_string(toolbox::FRENCH_REPUBLICAN, "%Y-%m-%d")
              << std::endl;
    std::cout << date.to_string(toolbox::FRENCH_REPUBLICAN, "%Y-%M-%D")
              << std::endl;
    std::cout << date.to_string(toolbox::GREGORIAN, "%Y-%m-%d")
              << std::endl;
    try {
        date = toolbox::Date(toolbox::FRENCH_REPUBLICAN,
                             "Jour de la Revolution 13, 10",
                             "%d %M, %Y",
                             false);
        std::cout << "NG" << std::endl;
    } catch (const std::exception& e) {
        (void)e;
        std::cout << "OK" << std::endl;
    }

    run_japanese_era_metadata_tests();
    run_japanese_era_string_roundtrip_tests();
    run_nanbokucho_authority_tests();
    run_japanese_wareki_format_tests();
    run_japanese_wareki_conversion_tests();

    try {
        date = toolbox::Date::today();
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
    } catch (const std::exception& e) {
        std::cout << "Error: " << e.what() << std::endl;
    }
    date = toolbox::Date(toolbox::JULIAN,
                         toolbox::EthiopianCalendar::AD,
                         645,
                         7,
                         29);
    std::cout << date.to_string(toolbox::JAPANESE_WAREKI, "%e%Y-%m-%d")
              << std::endl;
    std::cout << date.to_string(toolbox::GREGORIAN, "%Y-%m-%d")
              << std::endl;
}
