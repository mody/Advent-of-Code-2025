#include "point2d.h"

#include <fmt/core.h>
#include <fmt/ranges.h>

#include <iostream>
#include <unordered_set>

using Coord = int32_t;
using Point = Gfx_2d::Point<Coord>;

using Map = std::unordered_set<Point, boost::hash<Point>>;


void part1(Map const& world)
{
    unsigned movable{0};

    for (auto const& px : world) {
        unsigned around{0};
        for (auto const& dir :
             {Gfx_2d::North, Gfx_2d::South, Gfx_2d::West, Gfx_2d::East, Gfx_2d::NW, Gfx_2d::NE, Gfx_2d::SW, Gfx_2d::SE})
        {
            if (world.contains(px + dir)) {
                ++around;
            }
        }

        if (around < 4) {
            ++movable;
        }
    }

    fmt::print("1: {}\n", movable);
}


void part2(Map world)
{
    unsigned removable{0};

    for (;;) {
        Map candidates;

        for (auto const& px : world) {
            unsigned around{0};
            for (auto const& dir :
                 {Gfx_2d::North,
                  Gfx_2d::South,
                  Gfx_2d::West,
                  Gfx_2d::East,
                  Gfx_2d::NW,
                  Gfx_2d::NE,
                  Gfx_2d::SW,
                  Gfx_2d::SE})
            {
                if (world.contains(px + dir)) {
                    ++around;
                }
            }

            if (around < 4) {
                ++removable;
                candidates.insert(px);
            }
        }

        if (candidates.empty())
            break;

        for (auto const& px : candidates) {
            world.erase(px);
        }
        candidates.clear();
    }

    fmt::print("2: {}\n", removable);
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
                    world.insert({x, y});
                }
                ++x;
            }
        }
    }

    part1(world);
    part2(world);

    return 0;
}
