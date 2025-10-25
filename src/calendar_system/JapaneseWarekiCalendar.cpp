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

// Parse a date token like "ユリウス暦645年7月29日" or
// "グレゴリオ暦1716年8月9日" and convert to serial using provided
// calendars. Returns true on success.
bool parse_japanese_calendar_date(const std::string &token,
                                  const toolbox::JulianCalendar &julian,
                                  const toolbox::GregorianCalendar &greg,
                                  int &out_serial) {
    // Look for known prefixes
    std::size_t pos = std::string::npos;
    bool is_julian = false;
    if ((pos = token.find("ユリウス暦")) != std::string::npos) {
        is_julian = true;
    } else if ((pos = token.find("グレゴリオ暦")) != std::string::npos) {
        is_julian = false;
    } else {
        return false;
    }
    // after prefix comes e.g. 645年7月29日
    std::string rest = token.substr(pos + (is_julian ? 5 : 6));
    int year = 0, month = 0, day = 0;
    std::size_t p = 0;
    // read integer year
    while (p < rest.size() && !std::isdigit(rest[p])) ++p;
    if (p == rest.size()) return false;
    std::size_t q = p;
    while (q < rest.size() && std::isdigit(rest[q])) ++q;
    year = std::atoi(rest.substr(p, q - p).c_str());
    // find month
    p = q;
    while (p < rest.size() && !std::isdigit(rest[p])) ++p;
    if (p == rest.size()) return false;
    q = p;
    while (q < rest.size() && std::isdigit(rest[q])) ++q;
    month = std::atoi(rest.substr(p, q - p).c_str());
    // find day
    p = q;
    while (p < rest.size() && !std::isdigit(rest[p])) ++p;
    if (p == rest.size()) return false;
    q = p;
    while (q < rest.size() && std::isdigit(rest[q])) ++q;
    day = std::atoi(rest.substr(p, q - p).c_str());
    try {
        if (is_julian) {
            out_serial = julian.to_serial_date(toolbox::JulianCalendar::AD,
                                              year, month, day);
        } else {
            out_serial = greg.to_serial_date(toolbox::GregorianCalendar::AD,
                                             year, month, day);
        }
    } catch (...) {
        return false;
    }
    return true;
}

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
                    eserial = tmp - 1;
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
    (void)date_str;
    (void)format;
    (void)strict;
    throw std::invalid_argument(
        "JapaneseWarekiCalendar::to_serial_date failed: parsing date string not"
        " implemented");
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
    const EraRange *chosen = matches[0];
    for (std::size_t i = 0; i < matches.size(); ++i) {
        if (matches[i]->is_southern) {
            chosen = matches[i];
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
    int dummy_era = 0;
    int s_y = 0, s_m = 0, s_d = 0;
    int g_y = 0, g_m = 0, g_d = 0;

    // start_serial should be valid here (we skipped entries with unknown
    // start).
    greg.from_serial_date(chosen->start_serial, dummy_era, s_y, s_m, s_d);
    greg.from_serial_date(serial_date, dummy_era, g_y, g_m, g_d);

    int offset_year = g_y - s_y + 1;
    // If current month/day is before start month/day then subtract one year.
    if (g_m < s_m || (g_m == s_m && g_d < s_d)) {
        --offset_year;
    }
    if (offset_year < 1) offset_year = 1;

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
