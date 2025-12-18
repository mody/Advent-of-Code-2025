#include "point2d.h"

#include <cstdint>
#include <fmt/core.h>

#include <cassert>
#include <iostream>
#include <string>
#include <unordered_map>
#include <unordered_set>

using Coord = int32_t;
using Point = Gfx_2d::Point<Coord>;
using Map = std::unordered_map<Point, char>;

void part1(Map const& world, Point const& start)
{
    std::unordered_set<Point> s1, s2;
    s1.insert(start + Gfx_2d::Down);

    unsigned splits{0};

    for (;;) {
        s2.clear();
        for (auto px : s1) {
            Point next{px + Gfx_2d::Down};
            if (!world.contains(next))
                continue;  // out of map
            if (world.at(next) == '.') {
                s2.insert(next);
            }
            else if (world.at(next) == '^') {
                s2.insert(next + Gfx_2d::Left);
                s2.insert(next + Gfx_2d::Right);
                ++splits;
            }
        }

        if (s2.empty())
            break;
        std::swap(s1, s2);
    }

    fmt::print("1: {}\n", splits);
}

class Part2
{
    std::unordered_map<Point, uint64_t> cache;
    Map const& world;

public:
    Part2(Map const& w)
        : world{w}
    {
    }

    uint64_t beam(Point const& start)
    {
        Point px{start};
        for (;; px += Gfx_2d::Down) {
            if (!world.contains(px))
                return 1;

            if (world.at(px) == '^') {
                break;
            }
        }
        assert(world.at(px) == '^');

        if (cache.contains(px)) {
            return cache.at(px);
        }

        uint64_t v{0};
        for (auto const& dir : {Gfx_2d::Left, Gfx_2d::Right}) {
            Point next{px + dir};
            v += beam(next);
        }

        cache.insert({px, v});
        return v;
    }
};


int main()
{
    Map world;
    Point start;

    {
        Coord y{0};

        std::string line;
        while (std::getline(std::cin, line)) {
            if (line.empty()) {
                break;
            }

            Coord x{0};
            for (char c : line) {
                if (c == 'S') {
                    start = Point{x, y};
                }
                world.insert({{x++, y}, c});
            }
            ++y;
        }
    }

    part1(world, start);

    Part2 d{world};
    fmt::print("2: {}\n", d.beam(start));

    return 0;
}
