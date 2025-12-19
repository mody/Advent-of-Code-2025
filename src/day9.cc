#include "point2d.h"

#include <fmt/core.h>
#include <fmt/ranges.h>

#include <cassert>
#include <iostream>
#include <string>
#include <unordered_set>

using Coord = int64_t;
using Point = Gfx_2d::Point<Coord>;
using Points = std::unordered_set<Point>;

void part1(Points const& points)
{
    Point a, b;
    int64_t area{0};

    for (auto it1{points.begin()}; it1 != points.end(); ++it1) {
        auto const& p1{*it1};
        for (auto it2{std::next(it1)}; it2 != points.end(); ++it2) {
            auto const& p2{*it2};
            const int64_t dx{std::abs(p2.x - p1.x) + 1};
            const int64_t dy{std::abs(p2.y - p1.y) + 1};
            const int64_t area2{dx*dy};
            //  fmt::print("({},{}) - ({},{}) = {}\n", p1.x, p1.y, p2.x, p2.y, area2);
            if (area2 > area) {
                area = area2;
                a = p1;
                b = p2;
            }
        }
    }

    fmt::print("1: {}\n", area);
}

int main()
{
    Points points;

    {
        std::string line;
        while (std::getline(std::cin, line)) {
            if (line.empty())
                break;

            const auto sep{line.find(',')};
            line.at(sep) = '\0';
            points.insert({std::stoll(&line[0]), std::stoll(&line[sep + 1])});
        }
    }

    part1(points);

    return 0;
}
