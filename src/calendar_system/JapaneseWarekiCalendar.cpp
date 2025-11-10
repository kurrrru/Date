#include "calendar_system/JapaneseWarekiCalendar.hpp"

#include <cctype>
#include <cstring>
#include <cstdlib>
#include <fstream>
#include <limits>
#include <sstream>
#include <stdexcept>
#include <string>
#include <vector>
#include <iostream>

#include <string.hpp>

#include "calendar_system/GregorianCalendar.hpp"
#include "calendar_system/JapaneseEra.hpp"
#include "calendar_system/JulianCalendar.hpp"

namespace {
// Simple struct to hold era range information loaded from data/data.csv.
struct EraRange {
    toolbox::JapaneseEra era;
    int start_serial;
    int end_serial;  // inclusive end; use large value for open-ended
    std::string name_kanji;
    bool is_southern;  // used for Nanboku-cho disambiguation
};

// Build era ranges from embedded era metadata. This replaces the previous
// runtime CSV loader so that the program does not depend on data/data.csv
// at runtime. Each EraMetadata entry is converted to an EraRange with
// computed start/end serials using the appropriate calendar system.
std::vector<EraRange> load_era_ranges() {
    std::vector<EraRange> ranges;
    toolbox::JulianCalendar julian;
    toolbox::GregorianCalendar greg;

    const std::size_t count = toolbox::era_count();
    for (std::size_t idx = 0; idx < count; ++idx) {
        const toolbox::EraMetadata &md = toolbox::get_era_metadata(
            static_cast<toolbox::JapaneseEra>(idx));
        EraRange er;
        er.era = md.era;
        er.name_kanji = md.kanji ? std::string(md.kanji) : std::string();
        er.is_southern = (md.authority == toolbox::ERA_AUTHORITY_SOUTHERN);

        int sserial = std::numeric_limits<int>::min();
        int eserial = std::numeric_limits<int>::max();

        // Compute start serial
        try {
            if (md.start.calendar == toolbox::ERA_CALENDAR_JULIAN) {
                int era_flag = (md.start.year <= 0)
                    ? toolbox::JulianCalendar::BC
                    : toolbox::JulianCalendar::AD;
                sserial = julian.to_serial_date(era_flag,
                                                md.start.year,
                                                md.start.month,
                                                md.start.day);
            } else {
                int era_flag = (md.start.year <= 0)
                    ? toolbox::GregorianCalendar::BC
                    : toolbox::GregorianCalendar::AD;
                sserial = greg.to_serial_date(era_flag,
                                              md.start.year,
                                              md.start.month,
                                              md.start.day);
            }
        } catch (...) {
            sserial = std::numeric_limits<int>::min();
        }

        // Compute end serial (exclusive in metadata -> make inclusive here)
        if (md.has_end) {
            try {
                int tmp = std::numeric_limits<int>::max();
                if (md.end.calendar == toolbox::ERA_CALENDAR_JULIAN) {
                    int era_flag = (md.end.year <= 0)
                        ? toolbox::JulianCalendar::BC
                        : toolbox::JulianCalendar::AD;
                    tmp = julian.to_serial_date(era_flag,
                                                md.end.year,
                                                md.end.month,
                                                md.end.day);
                } else {
                    int era_flag = (md.end.year <= 0)
                        ? toolbox::GregorianCalendar::BC
                        : toolbox::GregorianCalendar::AD;
                    tmp = greg.to_serial_date(era_flag,
                                              md.end.year,
                                              md.end.month,
                                              md.end.day);
                }
                // metadata end is inclusive-exclusion boundary; use inclusive
                if (tmp != std::numeric_limits<int>::max()) {
                    eserial = tmp;
                }
            } catch (...) {
                eserial = std::numeric_limits<int>::max();
            }
        }

        er.start_serial = sserial;
        er.end_serial = eserial;
        ranges.push_back(er);
    }
    return ranges;
}

}  // namespace

namespace toolbox {

JapaneseWarekiCalendar::JapaneseWarekiCalendar() {
}

JapaneseWarekiCalendar::JapaneseWarekiCalendar(
        const JapaneseWarekiCalendar& other) {
    (void)other;
}

JapaneseWarekiCalendar& JapaneseWarekiCalendar::operator=(
        const JapaneseWarekiCalendar& other) {
    (void)other;
    return *this;
}

JapaneseWarekiCalendar::~JapaneseWarekiCalendar() {
}

int JapaneseWarekiCalendar::to_serial_date(int era,
                                           int year,
                                           int month,
                                           int day) const {
    // Build era table lazily
    const std::vector<EraRange> ranges = load_era_ranges();
    if (era < 0 || static_cast<std::size_t>(era) >= ranges.size()) {
        throw std::out_of_range(
            "JapaneseWarekiCalendar::to_serial_date failed: invalid era");
    }
    const EraRange &er = ranges[era];
    if (er.start_serial == std::numeric_limits<int>::min()) {
        throw std::out_of_range(
            "JapaneseWarekiCalendar::to_serial_date failed: era start unknown");
    }

    if (year <= 0) {
        throw std::out_of_range(
            "JapaneseWarekiCalendar::to_serial_date failed: year must be >= 1");
    }

    // Use embedded EraMetadata to determine which calendar to use and to
    // map the era-relative year to an absolute calendar year.
    const toolbox::EraMetadata &md = toolbox::get_era_metadata(
        static_cast<toolbox::JapaneseEra>(era));

    // Era-relative year N corresponds to calendar year: start.year + (N - 1).
    int target_year = md.start.year + (year - 1);

    toolbox::GregorianCalendar greg;
    toolbox::JulianCalendar julian;
    int serial = 0;
    try {
        if (md.start.calendar == toolbox::ERA_CALENDAR_JULIAN) {
            int era_flag = (target_year <= 0)
                ? toolbox::JulianCalendar::BC
                : toolbox::JulianCalendar::AD;
            serial = julian.to_serial_date(era_flag, target_year, month, day);
        } else {
            int era_flag = (target_year <= 0)
                ? toolbox::GregorianCalendar::BC
                : toolbox::GregorianCalendar::AD;
            serial = greg.to_serial_date(era_flag, target_year, month, day);
        }
    } catch (std::exception &e) {
        throw std::invalid_argument(
            std::string("JapaneseWarekiCalendar::to_serial_date failed: ") +
            e.what());
    }

    // Validate against era bounds: start (inclusive) and end (inclusive if
    // set).
    if (serial < er.start_serial) {
        throw std::out_of_range(
            "JapaneseWarekiCalendar::to_serial_date failed: date is before "
            "era start");
    }
    if (er.end_serial != std::numeric_limits<int>::max() &&
        serial > er.end_serial) {
        throw std::out_of_range(
            "JapaneseWarekiCalendar::to_serial_date failed: date is after "
            "era end");
    }
    return serial;
}

int JapaneseWarekiCalendar::to_serial_date(const std::string& date_str,
                                           const char* format,
                                           bool strict) const {
    if (!format) {
        throw std::invalid_argument(
            "JapaneseWarekiCalendar::to_serial_date failed: format is null");
    }

    const std::string& input = date_str;
    const bool allow_trailing_whitespace = !strict;
    std::size_t pos = 0;

    bool era_found = false;
    bool year_found = false;
    bool month_found = false;
    bool day_found = false;
    toolbox::JapaneseEra era_value = toolbox::END_OF_ERA;
    int year = 0;
    int month = 0;
    int day = 0;

    auto parse_number = [&](const char* field_name) -> int {
        std::size_t start = pos;
        while (pos < input.size() &&
               std::isdigit(static_cast<unsigned char>(input[pos]))) {
            ++pos;
        }
        if (start == pos) {
            throw std::invalid_argument(
                std::string("JapaneseWarekiCalendar::to_serial_date failed: "
                            "expected numeric ") + field_name);
        }
        try {
            return toolbox::stoi(input.substr(start, pos - start));
        } catch (const std::exception&) {
            throw std::invalid_argument(
                std::string("JapaneseWarekiCalendar::to_serial_date failed: "
                            "invalid numeric ") + field_name);
        }
    };

    for (std::size_t i = 0; format[i]; ++i) {
        char ch = format[i];
        if (ch != '%') {
            if (pos >= input.size() || input[pos] != ch) {
                throw std::invalid_argument(
                    "JapaneseWarekiCalendar::to_serial_date failed: literal"
                    " mismatch in date_str");
            }
            ++pos;
            continue;
        }

        char spec = format[++i];
        if (!spec) {
            throw std::invalid_argument(
                "JapaneseWarekiCalendar::to_serial_date failed: incomplete "
                "format specifier");
        }

        switch (spec) {
            case 'E':
            case 'e': {
                if (era_found) {
                    throw std::invalid_argument(
                        "JapaneseWarekiCalendar::to_serial_date failed: era "
                        "specified multiple times");
                }
                std::size_t matched_len = 0;
                toolbox::JapaneseEra matched = toolbox::END_OF_ERA;
                const std::size_t count = toolbox::era_count();
                for (std::size_t idx = 0; idx < count; ++idx) {
                    const toolbox::EraMetadata& md =
                        toolbox::get_era_metadata(
                            static_cast<toolbox::JapaneseEra>(idx));
                    if (!md.kanji) {
                        continue;
                    }
                    const std::string era_name(md.kanji);
                    if (era_name.empty()) {
                        continue;
                    }
                    const std::size_t len = era_name.size();
                    if (pos + len > input.size()) {
                        continue;
                    }
                    if (input.compare(pos, len, era_name) == 0 &&
                        len > matched_len) {
                        matched_len = len;
                        matched = md.era;
                    }
                }
                if (matched == toolbox::END_OF_ERA) {
                    throw std::invalid_argument(
                        "JapaneseWarekiCalendar::to_serial_date failed: era "
                        "not found in date_str");
                }
                era_value = matched;
                era_found = true;
                pos += matched_len;
                break;
            }
            case 'Y':
            case 'y': {
                if (year_found) {
                    throw std::invalid_argument(
                        "JapaneseWarekiCalendar::to_serial_date failed: year "
                        "specified multiple times");
                }
                year = parse_number("year");
                year_found = true;
                break;
            }
            case 'M':
            case 'm': {
                if (month_found) {
                    throw std::invalid_argument(
                        "JapaneseWarekiCalendar::to_serial_date failed: month"
                        " specified multiple times");
                }
                month = parse_number("month");
                month_found = true;
                break;
            }
            case 'D':
            case 'd': {
                if (day_found) {
                    throw std::invalid_argument(
                        "JapaneseWarekiCalendar::to_serial_date failed: day "
                        "specified multiple times");
                }
                day = parse_number("day");
                day_found = true;
                break;
            }
            case '%': {
                if (pos >= input.size() || input[pos] != '%') {
                    throw std::invalid_argument(
                        "JapaneseWarekiCalendar::to_serial_date failed: '%' expected in date_str");
                }
                ++pos;
                break;
            }
            default:
                throw std::invalid_argument(
                    "JapaneseWarekiCalendar::to_serial_date failed: invalid "
                    "format specifier");
        }
    }

    if (!era_found || !year_found || !month_found || !day_found) {
        throw std::invalid_argument(
            "JapaneseWarekiCalendar::to_serial_date failed: incomplete "
            "date components");
    }

    if (pos < input.size()) {
        if (allow_trailing_whitespace) {
            while (pos < input.size() &&
                   std::isspace(static_cast<unsigned char>(input[pos]))) {
                ++pos;
            }
        }
        if (pos != input.size()) {
            throw std::invalid_argument(
                "JapaneseWarekiCalendar::to_serial_date failed: trailing "
                "characters remain in date_str");
        }
    }

    if (era_value == toolbox::END_OF_ERA) {
        throw std::invalid_argument(
            "JapaneseWarekiCalendar::to_serial_date failed: era missing");
    }

    return to_serial_date(static_cast<int>(era_value), year, month, day);
}

void JapaneseWarekiCalendar::from_serial_date(int serial_date,
                                              int& era,
                                              int& year,
                                              int& month,
                                              int& day) const {
    const std::vector<EraRange> ranges = load_era_ranges();
    // find matching era(s)
    std::vector<const EraRange*> matches;
    for (std::size_t i = 0; i < ranges.size(); ++i) {
        const EraRange &er = ranges[i];
        if (er.start_serial == std::numeric_limits<int>::min()) continue;
        int s = er.start_serial;
        int e = er.end_serial == std::numeric_limits<int>::max()
            ? std::numeric_limits<int>::max()
            : er.end_serial;
        if (serial_date >= s && serial_date <= e) {
            matches.push_back(&er);
        }
    }
    if (matches.empty()) {
        throw std::out_of_range(
            "JapaneseWarekiCalendar::from_serial_date failed: era not found for"
            " serial date");
    }
    // If multiple matches (Nanboku-cho), prefer southern court
    const EraRange *chosen = matches.back();
    for (std::size_t i = 0; i < matches.size(); ++i) {
        if (matches[matches.size() - 1 - i]->is_southern) {
            chosen = matches[matches.size() - 1 - i];
            break;
        }
    }
    // era: return the ordinal index of the era (which-era), not BC/AD.
    era = static_cast<int>(chosen->era);

    // Compute year/month/day relative to the era start.
    // Era start serial is available in chosen->start_serial (from
    // load_era_ranges). Convert both start_serial and serial_date to
    // Gregorian Y/M/D and compute offset in years. Year is 1-based: the
    // start date is year 1.
    toolbox::GregorianCalendar greg;
    toolbox::JulianCalendar julian;
    int dummy_era = 0;
    int s_y = 0, s_m = 0, s_d = 0;
    int g_y = 0, g_m = 0, g_d = 0;

    const int greg_start = greg.to_serial_date(
        toolbox::GregorianCalendar::AD, 1582, 10, 15);

    // start_serial should be valid here (we skipped entries with unknown
    // start).
    if (serial_date < greg_start) {
        julian.from_serial_date(chosen->start_serial, dummy_era, s_y, s_m, s_d);
        julian.from_serial_date(serial_date, dummy_era, g_y, g_m, g_d);
    } else {
        greg.from_serial_date(chosen->start_serial, dummy_era, s_y, s_m, s_d);
        greg.from_serial_date(serial_date, dummy_era, g_y, g_m, g_d);
    }

    int offset_year = g_y - s_y + 1;

    year = offset_year;
    month = g_m;
    day = g_d;
}

void JapaneseWarekiCalendar::from_serial_date(int serial_date,
                                              std::string& date_str,
                                              const char* format) const {
    if (!format) {
        throw std::invalid_argument(
            "JapaneseWarekiCalendar::from_serial_date failed: format is null");
    }
    int era, year, month, day;
    try {
        from_serial_date(serial_date, era, year, month, day);
    } catch (std::out_of_range &e) {
        // Need to produce an error message including adjacent era ranges
        const std::vector<EraRange> ranges = load_era_ranges();
        // find surrounding eras
        const EraRange *prev = NULL;
        const EraRange *next = NULL;
        for (std::size_t i = 0; i < ranges.size(); ++i) {
            const EraRange &er = ranges[i];
            if (er.start_serial != std::numeric_limits<int>::min() &&
                er.start_serial < serial_date) {
                prev = &er;
            }
            if (er.start_serial != std::numeric_limits<int>::min() &&
                er.start_serial > serial_date) {
                next = &er;
                break;
            }
        }
        std::string msg =
            "JapaneseWarekiCalendar::from_serial_date failed: "
            "no era for date. ";
        if (prev) {
            msg += "Previous era: ";
            msg += prev->name_kanji.empty() ? "(unknown)" : prev->name_kanji;
            msg += "; ";
        }
        if (next) {
            msg += "Next era: ";
            msg += next->name_kanji.empty() ? "(unknown)" : next->name_kanji;
        }
        throw std::out_of_range(msg);
    }
    // Format using simple replacements: %E -> era kanji, %Y/%y -> year,
    // %M/%m -> month, %D/%d -> day
    std::string out;
    for (std::size_t i = 0; format[i]; ++i) {
        if (format[i] == '%') {
            char c = format[++i];
            switch (c) {
                case 'E':
                case 'e': {
                    out += era_to_kanji(static_cast<JapaneseEra>(era));
                    break;
                }
                case 'Y':
                case 'y': {
                    std::ostringstream ss;
                    ss << year;
                    out += ss.str();
                    break;
                }
                case 'M':
                case 'm': {
                    std::ostringstream ss;
                    ss << month;
                    out += ss.str();
                    break;
                }
                case 'D':
                case 'd': {
                    std::ostringstream ss;
                    ss << day;
                    out += ss.str();
                    break;
                }
                case '%':
                    out += '%';
                    break;
                default:
                    throw std::invalid_argument(
                        "JapaneseWarekiCalendar::from_serial_date failed: "
                        "invalid format specifier");
            }
        } else {
            out += format[i];
        }
    }
    date_str = out;
}

void JapaneseWarekiCalendar::from_serial_date(int serial_date,
                                              int& day_of_week) const {
    // Use Gregorian weekday as approximation
    toolbox::GregorianCalendar greg;
    greg.from_serial_date(serial_date, day_of_week);
}

}  // namespace toolbox
