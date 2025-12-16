#include <fmt/core.h>

#include <boost/algorithm/string/classification.hpp>
#include <boost/algorithm/string/split.hpp>

#include <cassert>
#include <deque>
#include <iostream>
#include <set>
#include <string>
#include <string_view>
#include <vector>

using Value = uint64_t;
using Values = std::vector<std::pair<Value, Value>>;

void part1(Values const& values)
{
    Value sum{0};

    for (auto const& [v1, v2] : values) {
        for (Value i{v1}; i <= v2; ++i) {
            const std::string s{std::to_string(i)};
            if (s.size() & 0x1)
                continue; // only even size

            bool invalid{true};
            for (size_t p1{0}, p2{s.size() / 2u}; p2 < s.size(); ++p1, ++p2) {
                if (s.at(p1) != s.at(p2)) {
                    invalid = false;
                    break;
                }
            }
            if (invalid) {
                sum += i;
                fmt::print("({}-{}) - {}\n", v1, v2, i);
            }
        }
    }

    fmt::print("1: {}\n", sum);
}


void part2(Values const& values)
{
    Value sum{0};

    for (auto const& [v1, v2] : values) {
        for (Value i{v1}; i <= v2; ++i) {
            const std::string s{std::to_string(i)};
            std::string_view sv(s);

            for (unsigned sz{1}; sz <= (s.size() / 2); sz += 1) {
                if (s.size() % sz)
                    continue;

                std::set<std::string_view> parts;

                for (size_t from{0}; from < sv.size(); from += sz) {
                    parts.insert(sv.substr(from, sz));
                    if (parts.size() > 1)
                        break;
                }

                if (parts.size() == 1) {
                    sum += i;
                    break;
                }
            }
        }
    }

    fmt::print("2: {}\n", sum);
}

int main()
{
    std::string line;
    std::getline(std::cin, line);

    if (line.empty())
        return 1;

    Values values;

    std::deque<std::string> items;
    boost::algorithm::split(items, line, boost::is_any_of(", -"), boost::algorithm::token_compress_on);

    while(!items.empty()) {
        Value v1{std::stoull(items.front())};
        items.pop_front();

        Value v2{std::stoull(items.front())};
        items.pop_front();

        values.push_back({v1, v2});
    }

    //  part1(values);
    part2(values);

    return 0;
}
