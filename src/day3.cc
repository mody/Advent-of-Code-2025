#include <fmt/core.h>

#include <cassert>
#include <iostream>
#include <ranges>
#include <string>
#include <vector>

using Strings = std::vector<std::string>;

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

        sum += ((c1 - '0') * 10) + (c2 - '0');
    }

    fmt::print("1: {}\n", sum);
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

    return 0;
}
