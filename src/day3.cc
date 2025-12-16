#include <fmt/core.h>

#include <algorithm>
#include <cassert>
#include <deque>
#include <iostream>
#include <ranges>
#include <string>
#include <vector>

using Strings = std::vector<std::string>;

uint64_t extractNumbers(std::string_view s, unsigned digits)
{
    std::deque<char> result;

    size_t from{0};
    for (unsigned d{1}; d <= digits; ++d) {

        const auto top{std::ranges::max_element(s.substr(from, s.size() - from - (digits - d)))};
        assert(top != s.cend());

        result.push_back(*top);
        from = std::distance(s.cbegin(), top) + 1;
    }

    uint64_t sum{0};
    for (const auto& [idx, c] : result | std::views::reverse | std::views::enumerate) {
        sum += (c - '0') * std::pow(10, idx);
    }

    return sum;
}


void part1(Strings const& input)
{
    uint64_t sum{0};

    for (auto const& line : input) {
        char c1{0}, c2{0};
        size_t pos1{0};

        std::string_view v{line};

        for (auto const& [idx, c] : std::views::enumerate(v.substr(0, v.size() - 1))) {
            if (c > c1) {
                c1 = c;
                pos1 = idx;
            }
        }

        for (auto const& c : v.substr(pos1+1)) {
            c2 = std::max(c2, c);
        }

        auto x{((c1 - '0') * 10) + (c2 - '0')};
        sum += x;
    }

    fmt::print("1: {}\n", sum);
}

void part2(Strings const& input)
{
    uint64_t sum{0};

    for (auto const& line : input) {
        auto v{extractNumbers(line, 12)};
        sum += v;
    }

    fmt::print("2: {}\n", sum);
}

int main()
{
    Strings input;

    std::string line;
    while(std::getline(std::cin, line)) {
        if (line.empty()) {
            break;
        }
        input.push_back(std::move(line));
    }

    part1(input);
    part2(input);

    return 0;
}
