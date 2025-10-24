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

// Load era ranges from data/data.csv. If file missing, returns empty vector.
std::vector<EraRange> load_era_ranges() {
    std::vector<EraRange> ranges;
    std::ifstream ifs("data/data.csv");
    if (!ifs) {
        return ranges;
    }
    toolbox::JulianCalendar julian;
    toolbox::GregorianCalendar greg;
    std::string line;
    // Skip header
    std::getline(ifs, line);
    int idx = 0;
    while (std::getline(ifs, line)) {
        if (line.empty()) {
            ++idx;
            continue;
        }
        std::istringstream ss(line);
        std::string kanji, reading, start_tok, end_tok;
        std::getline(ss, kanji, ',');
        std::getline(ss, reading, ',');
        std::getline(ss, start_tok, ',');
        std::getline(ss, end_tok, ',');
        // trim
        while (!start_tok.empty() && (start_tok[0] == ' ')) {
            start_tok.erase(0, 1);
        }
        while (!end_tok.empty() && (end_tok[0] == ' ')) {
            end_tok.erase(0, 1);
        }

        EraRange er;
        er.era = static_cast<toolbox::JapaneseEra>(idx);
        er.name_kanji = kanji;
    er.is_southern = (kanji.find("南朝") != std::string::npos ||
              kanji.find("南") != std::string::npos);
        int sserial = std::numeric_limits<int>::min();
        int eserial = std::numeric_limits<int>::max();
        if (!start_tok.empty()) {
            // handle cases like "(大覚寺統: ユリウス暦1331年9月11日) ..."
            std::string token = start_tok;
            // if contains '(', try to find first calendar date inside
            std::size_t pos = token.find("ユリウス暦");
            if (pos == std::string::npos) {
                pos = token.find("グレゴリオ暦");
            }
            if (pos != std::string::npos) {
                // take substring from pos to end of token
                std::string sub = token.substr(pos);
                int tmp;
                if (parse_japanese_calendar_date(sub, julian, greg, tmp)) {
                    sserial = tmp;
                }
            }
        }
        if (!end_tok.empty() && end_tok != "None") {
            std::string token = end_tok;
            std::size_t pos = token.find("ユリウス暦");
            if (pos == std::string::npos) {
                pos = token.find("グレゴリオ暦");
            }
            if (pos != std::string::npos) {
                std::string sub = token.substr(pos);
                int tmp;
                if (parse_japanese_calendar_date(sub, julian, greg, tmp)) {
                    eserial = tmp - 1;  // make inclusive end
                }
            }
        }
        // if parsing failed: leave as extreme values
        er.start_serial = sserial;
        er.end_serial = eserial;
        ranges.push_back(er);
        ++idx;
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
    // Interpret year/month/day relative to era start: we compute serial of era
    // start and then add (year-1) etc. For historical calendars this is
    // non-trivial; here we simply compute the serial of the corresponding
    // Gregorian/Julian date by attempting to map era start's calendar type
    // (already encoded in source data) via load. For simplicity, if
    // month/day/year are given, assume they are Gregorian dates for modern
    // eras; otherwise throw. For now, treat input as Gregorian AD for
    // conversion.
    toolbox::GregorianCalendar greg;
    try {
        return greg.to_serial_date(GregorianCalendar::AD, year, month, day);
    } catch (std::exception &e) {
        throw std::invalid_argument(
            std::string("JapaneseWarekiCalendar::to_serial_date failed: ") +
            e.what());
    }
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
    era = static_cast<int>(chosen->era);
    // Compute year/month/day using GregorianCalendar for now (approximation).
    toolbox::GregorianCalendar greg;
    int eera, y, m, d;
    greg.from_serial_date(serial_date, eera, y, m, d);
    year = y;
    month = m;
    day = d;
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
