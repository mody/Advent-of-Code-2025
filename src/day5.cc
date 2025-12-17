#include <fmt/core.h>

#include <cassert>
#include <iostream>
#include <string>
#include <vector>

struct Range
{
    uint64_t from{0}, to{0};

    bool contains(uint64_t v) const { return from <= v && v <= to; }
};


int main()
{
    std::vector<Range> recipes;

    std::string line;
    while (std::getline(std::cin, line)) {
        if (line.empty())
            break;

        const auto sep{line.find('-')};
        line.at(sep) = '\0';
        recipes.push_back({.from = std::stoull(&line[0]), .to = std::stoull(&line[sep + 1])});
    }

    uint64_t cnt1{0};

    while (std::getline(std::cin, line)) {
        if (line.empty())
            break;

        const uint64_t v{std::stoull(&line[0])};
        for (auto const& r : recipes) {
            if (r.contains(v)) {
                ++cnt1;
                break;
            }
        }
    }

    fmt::print("1: {}\n", cnt1);

    return 0;
}
