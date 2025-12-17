#include "point2d.h"

#include <fmt/core.h>
#include <fmt/ranges.h>

#include <algorithm>
#include <cmath>
#include <iostream>
#include <map>
#include <unordered_map>
#include <unordered_set>

using Coord = int32_t;
using Point = Gfx_2d::Point<Coord>;

using Map = std::unordered_map<Point, char, boost::hash<Point>>;

std::pair<Coord, Coord> measure(Map const& mapa)
{
    Coord max_x = std::numeric_limits<Coord>::min();
    Coord max_y = std::numeric_limits<Coord>::min();
    for (auto const& [p, _] : mapa) {
        max_x = std::max(max_x, p.x);
        max_y = std::max(max_y, p.y);
    }
    ++max_x;
    ++max_y;
    return {max_x, max_y};
}

void part1(Map const& world)
{
    unsigned movable{0};

    for (auto const& [px, _] : world) {
        unsigned around{0};
        for (auto const& dir :
             {Gfx_2d::North, Gfx_2d::South, Gfx_2d::West, Gfx_2d::East, Gfx_2d::NW, Gfx_2d::NE, Gfx_2d::SW, Gfx_2d::SE})
        {
            if (world.contains(px + dir)) {
                ++around;
            }
        }

        if (around < 4) {
            fmt::print("Movable: ({}, {}) with {} around\n", px.x, px.y, around);
            ++movable;
        }
    }

    fmt::print("1: {}\n", movable);
}

int main()
{
    Map world;

    {
        std::string line;
        for (Coord y{0}; std::getline(std::cin, line); ++y) {
            if (line.empty())
                break;

            Coord x{0};
            for (auto const& c : line) {
                if (c == '@') {
                    world.insert({{x, y}, c});
                }
                ++x;
            }
        }
    }

    part1(world);

    return 0;
}
